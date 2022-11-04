#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <fmod.hpp>
#include <vector>
#include <stdlib.h>


class AudioPlayer
{
public:
    FMOD_RESULT result;
    FMOD::System *system = nullptr;
    FMOD::ChannelGroup *channelGroup = nullptr;
    std::vector<FMOD::Sound*> soundpointer;
    std::vector<FMOD::Channel*> channels;
    AudioPlayer();
    void create_sounds(); //every soundpointer gets initialized
    bool play_hashcode(uint hashcode);
};

#endif // AUDIOPLAYER_H
