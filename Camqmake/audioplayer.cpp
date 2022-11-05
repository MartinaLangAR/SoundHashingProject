#include "audioplayer.h"
#include <iostream>
#include <fmod.h>
#include <fmod_errors.h>
#include <fmod.hpp>
#include <fmod_codec.h>
#include <fmod_common.h>
#include <fmod_dsp.h>
#include <fmod_dsp_effects.h>
#include <fmod_output.h>
#include <iostream>
#include <curses.h>
#include<dirent.h>
#include <chrono>
#include <thread>

#define FMOD_DEFAULT 0x00000000

int getdir (std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        std::string file_name = "audio/" + std::string(dirp->d_name);
        if (file_name != "audio/." and file_name != "audio/.."){
        files.push_back(std::string(file_name));

        }
    }
    closedir(dp);
}

AudioPlayer::AudioPlayer()
{

    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }

    // Create the channel group.
    result = system->createChannelGroup("Frequencies", &channelGroup);

    soundpointer.reserve(160);  //<-------------------------------------------- Amount of soundfiles manually set here!
    channels.reserve(160);
    for (int i=0; i< soundpointer.capacity(); i++){
        soundpointer[i] = nullptr;
        channels[i] = nullptr;
    }
}

void AudioPlayer::create_sounds(){
    std::vector<std::string> file_names;
    getdir("audio", file_names);
    for (int i=0; i <= file_names.size(); i++) {
       auto path = file_names[i].c_str();
       std::cout << path << std::endl;
       system->createSound(path, FMOD_DEFAULT, nullptr, &soundpointer[i]);
       soundpointer[i]->setMode(FMOD_LOOP_NORMAL);
       soundpointer[i]->setLoopCount(-1);
       //std::cout << "Sound-Pointer is set to " << soundpointer[i] << std::endl;
    }
};

bool AudioPlayer::play_hashcode(uint hashcode) {
    //plays file according to hashcode in endless loop
    //returns wether it was already playing
    bool is_active;
    channels[hashcode]->isPlaying(&is_active);
    if (is_active){
        std::cout << "Sound " << hashcode << " is already playing!" << std::endl;
        return 1;
    }
    std::cout << "about to play " << soundpointer[hashcode] << std::endl;
    result = system->playSound(soundpointer[hashcode], nullptr, false, &channels[hashcode]);
    return 0;
}

void AudioPlayer::stop_channel(uint hashcode) {
    channels[hashcode]->setPaused(true);
}
