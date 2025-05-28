#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"

#include "FSBANK/fsbank.h"
#include "FSBANK/fsbank_errors.h"

#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;

void ERRCHECK(FMOD_RESULT result) {
#ifdef _DEBUG
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
#endif
}

void ERRCHECK(FSBANK_RESULT result) {
#ifdef _DEBUG
    if (result != FSBANK_OK) {
        printf("FSBANK error! (%d) %s\n", result, FSBank_ErrorString(result));
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
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }

    result = system->init(32, FMOD_INIT_NORMAL, nullptr);
    ERRCHECK(result);

    result = system->createSound(filePath.string().c_str(), FMOD_DEFAULT, 0, &sound);
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

        result = system->createSound(filePath.string().c_str(), FMOD_DEFAULT, nullptr, &sound);
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

void createFSB(const fs::path& filePath) {
    FSBANK_RESULT result;
    std::vector<std::string> fileNames;

    std::string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".txt") {
        std::ifstream fileList(filePath);
        if (!fileList) {
            std::cerr << "Failed to open file list: " << filePath << "\n";
            return;
        }

        std::string line;
        while (std::getline(fileList, line)) {
            if (!line.empty()) {
                fileNames.push_back(line);
            }
        }
    }
    else {
        fileNames.push_back(filePath.string());
    }

    result = FSBank_Init(FSBANK_FSBVERSION_FSB5, FSBANK_INIT_NORMAL, 1, nullptr);
    ERRCHECK(result);

    // Create one FSBANK_SUBSOUND per file
    std::vector<FSBANK_SUBSOUND> subsounds(fileNames.size());
    std::vector<const char*> cstrs; // Store c-strings separately to ensure they remain valid
    cstrs.reserve(fileNames.size());

    for (size_t i = 0; i < fileNames.size(); ++i) {
        cstrs.push_back(fileNames[i].c_str());

        subsounds[i] = {};
        subsounds[i].fileNames = &cstrs.back();
        subsounds[i].numFiles = 1;
        subsounds[i].overrideFlags = FSBANK_BUILD_DISABLESYNCPOINTS;
        //subsounds[i].overrideQuality = 100;
        //subsounds[i].desiredSampleRate = 48000;
        //subsounds[i].percentOptimizedRate = 100;
    }
    //FSBANK_FORMAT_OPUS FSBANK_FORMAT_VORBIS
    result = FSBank_Build(subsounds.data(),static_cast<int>(subsounds.size()),FSBANK_FORMAT_OPUS,FSBANK_BUILD_DEFAULT | FSBANK_BUILD_DONTLOOP,100,nullptr,"out.fsb");
    ERRCHECK(result);

    result = FSBank_Release();
    ERRCHECK(result);
}

int main(int argc, const char** argv) {

    std::string mode;
    fs::path filePath;

    if (argc == 2) {
        filePath = fs::absolute(argv[1]);

        std::string ext = filePath.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".fsb") {
            mode = "dump";
        }
    }

    if (mode != "dump") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <create|dump> <FSB/List>" << std::endl;
            return -1;
        }

        mode = argv[1];
        filePath = fs::absolute(argv[2]);
    }


    if (!fs::exists(filePath)) {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return -1;
    }

    //check modes
    if (mode == "dump") {
        dumpFSB(filePath);
    }
    else if (mode == "create") {
        createFSB(filePath);
    }
    else {
        std::cerr << "Invalid mode. Use 'create' or 'dump'." << std::endl;
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
