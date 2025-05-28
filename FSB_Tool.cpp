#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void ERRCHECK(FMOD_RESULT result)
{
#ifdef _DEBUG
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
#endif
}

int main(int argc, const char** argv) {

    FMOD::System *system = nullptr;
    FMOD::Sound *sound = nullptr;
    FMOD_RESULT result = FMOD_OK;
    unsigned int version = 0;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio_file_path>" << std::endl;
        return -1;
    }
    const fs::path filePath = fs::absolute(argv[1]);

    if (!fs::exists(filePath)) {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return -1;
    }
    
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

#ifdef _DEBUG
    result = FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG, FMOD_DEBUG_MODE_FILE, 0, "fmodlog.txt");
    ERRCHECK(result);
#endif

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
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

    //pretty much all of this was just to get each subsound name lol
    result = sound->release();
    ERRCHECK(result);

    result = system->release();
    ERRCHECK(result);

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

        result = system->createSound(filePath.string().c_str(), FMOD_DEFAULT, 0, &sound);
        ERRCHECK(result);

        result = sound->getSubSound(i, &subsound);
        ERRCHECK(result);
        
        result = system->playSound(subsound, 0, false, &channel);
        ERRCHECK(result);

        bool playing = true;
        while (playing) {
            result = system->update();
            ERRCHECK(result);
            result = channel->isPlaying(&playing);
            ERRCHECK(result);
        }

        result = subsound->release();
        ERRCHECK(result);
        result = sound->release();
        ERRCHECK(result);
        result = system->release();
        ERRCHECK(result);
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
