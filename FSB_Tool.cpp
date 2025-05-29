// FMOD headers
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"

// FSBANK headers
#include "FSBANK/fsbank.h"
#include "FSBANK/fsbank_errors.h"

// Standard C++ headers
#include <iostream>
#include <fstream>

// Boost libraries
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/locale.hpp>

namespace fs = boost::filesystem;

void ERRCHECK(FMOD_RESULT result) {
#ifdef _DEBUG
    if (result != FMOD_OK) {
        wprintf(L"FMOD error! (%d) %s\n", result, FMOD_WErrorString(result));
        exit(-1);
    }
#endif
}

void ERRCHECK(FSBANK_RESULT result) {
#ifdef _DEBUG
    if (result != FSBANK_OK) {
        wprintf(L"FSBANK error! (%d) %s\n", result, FSBank_WErrorString(result));
        exit(-1);
    }
#endif
}

void dumpFSB(const fs::path& filePath) {
    FMOD::System* system = nullptr;
    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result;
    unsigned int version = 0;

    result = FMOD::System_Create(&system);
    ERRCHECK(result);


    //only on fmodl.dll
#ifdef _DEBUG
    result = FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG, FMOD_DEBUG_MODE_FILE, nullptr, "fmodlog.txt");
    ERRCHECK(result);
#endif

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION) {
        wprintf(L"Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }

    result = system->init(32, FMOD_INIT_NORMAL, nullptr);
    ERRCHECK(result);


    std::string utf8FilePath = boost::locale::conv::utf_to_utf<char>(filePath.wstring());
    result = system->createSound(utf8FilePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    ERRCHECK(result);


    int numSubSounds = 0;
    result = sound->getNumSubSounds(&numSubSounds);
    ERRCHECK(result);

    std::vector<std::string> SoundNames;
    for (int i = 0; i < numSubSounds; ++i) {
        FMOD::Sound* subsound = nullptr;
        result = sound->getSubSound(i, &subsound);
        ERRCHECK(result);

        std::vector<char> name(256);
        result = subsound->getName(name.data(), static_cast<int>(name.size()));
        ERRCHECK(result);
        SoundNames.emplace_back(name.data());

        result = subsound->release();
        ERRCHECK(result);
    }

    result = sound->release();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    // Export each sub sound as a WAV
    for (int i = 0; i < numSubSounds; ++i) {
        std::string filename = SoundNames[i] + ".wav";

        FMOD::System* system;
        FMOD::Sound* sound;
        FMOD::Sound* subsound;
        FMOD::Channel* channel;

        result = FMOD::System_Create(&system);
        ERRCHECK(result);

        result = system->setOutput(FMOD_OUTPUTTYPE_WAVWRITER_NRT);
        ERRCHECK(result);

        result = system->init(32, FMOD_INIT_STREAM_FROM_UPDATE, (void*)filename.c_str());
        ERRCHECK(result);

        result = system->createSound(utf8FilePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
        ERRCHECK(result);

        result = sound->getSubSound(i, &subsound);
        ERRCHECK(result);

        result = system->playSound(subsound, nullptr, false, &channel);
        ERRCHECK(result);

        bool playing = true;
        while (playing) {
            result = system->update();
            ERRCHECK(result);
            result = channel->isPlaying(&playing);
            ERRCHECK(result);
        }

        subsound->release();
        sound->release();
        system->release();
    }
}

void createFSB(const boost::filesystem::path& filePath) {
    FSBANK_RESULT result;
    std::vector<std::wstring> fileNames;
    std::string ext = boost::algorithm::to_lower_copy(filePath.extension().string());

    if (ext == ".txt") {
        std::wifstream fileList(filePath.string());
        if (!fileList) {
            std::cerr << "Failed to open file list: " << filePath << "\n";
            return;
        }

        fileList.imbue(boost::locale::generator().generate("en_US.UTF-8"));

        std::wstring line;
        while (std::getline(fileList, line)) {
            if (!line.empty()) {
                fileNames.push_back(boost::filesystem::path(line).wstring());
            }
        }
    }
    else {
        fileNames.push_back(filePath.wstring());
    }

    //Init FSBank
    result = FSBank_Init(FSBANK_FSBVERSION_FSB5, FSBANK_INIT_NORMAL, 1, nullptr);
    ERRCHECK(result);

    //vector array of soundbanks (for each file)
    std::vector<FSBANK_SUBSOUND> subsounds(fileNames.size());
    //converted strings
    std::vector<std::string> utf8Strings;
    //ptrs for them
    std::vector<const char*> cfileNames;

    for (size_t i = 0; i < fileNames.size(); ++i) {

        utf8Strings.push_back(boost::locale::conv::utf_to_utf<char>(fileNames[i]));
        cfileNames.push_back(utf8Strings.back().c_str());

        subsounds[i] = {};
        subsounds[i].fileNames = &cfileNames.back();
        subsounds[i].numFiles = 1;
        subsounds[i].overrideFlags = FSBANK_BUILD_DISABLESYNCPOINTS;
    }

    if (fileNames.size() == 1) {
        auto outputPath = boost::nowide::narrow(filePath.filename().replace_extension(L".fsb").wstring());
        result = FSBank_Build(subsounds.data(), static_cast<int>(subsounds.size()), FSBANK_FORMAT_VORBIS, FSBANK_BUILD_DEFAULT | FSBANK_BUILD_DONTLOOP, 100, nullptr, outputPath.c_str());
    }
    else {
        result = FSBank_Build(subsounds.data(), static_cast<int>(subsounds.size()), FSBANK_FORMAT_VORBIS, FSBANK_BUILD_DEFAULT | FSBANK_BUILD_DONTLOOP, 100, nullptr, "Output.fsb");
    }

    ERRCHECK(result);

    result = FSBank_Release();
    ERRCHECK(result);
}


int wmain(int argc, wchar_t** argv) {
    std::wstring mode;
    fs::path filePath;

    if (argc == 2) {
        filePath = fs::absolute(argv[1]);
        std::wstring ext = filePath.extension().wstring();
        boost::algorithm::to_lower(ext);
        if (ext == L".fsb") {
            mode = L"dump";
        }
    }

    if (mode != L"dump") {
        if (argc < 3) {
            std::wcerr << L"Usage: " << argv[0] << L" <create|dump> <FSB/List>" << std::endl;
            return -1;
        }

        mode = argv[1];
        filePath = fs::absolute(argv[2]);
    }

    if (!fs::exists(filePath)) {
        std::wcerr << L"File does not exist: " << filePath.wstring() << std::endl;
        return -1;
    }

    // Check modes
    if (mode == L"dump") {
        dumpFSB(filePath);
    }
    else if (mode == L"create") {
        createFSB(filePath);
    }
    else {
        std::wcerr << L"Invalid mode. Use 'create' or 'dump'." << std::endl;
        return -1;
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
