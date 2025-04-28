#ifndef AUDIO_H
#define AUDIO_H
// #pragma once

#include <array>
#include "raylib.h"

enum class SoundAsset { FootGrass, FootStone, Attack, Death, Coins, Laser };
enum class MusicAsset { LightAmbience, DarkAmbience };

static constexpr int MAX_SOUNDS = 6;
static constexpr int MAX_MUSIC = 2;

class Audio 
{
    private:    
        std::array<Sound, MAX_SOUNDS> sounds;
        std::array<Music, MAX_MUSIC> music;

    public:
        Audio();

        void load_sounds();

        void play_sound(SoundAsset track);
        
        void update_music(MusicAsset track);

        void play_music(MusicAsset track);

        void stop_music(MusicAsset track);

        void shutdown_audio();
};

#endif // AUDIO_H