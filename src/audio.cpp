#include "audio.hpp"

Audio::Audio() {

	load_sounds();
}

// load sounds
void Audio::load_sounds() {
	sounds[static_cast<int>(SoundAsset::FootGrass)] = LoadSound("assets/sfx/Grass1.wav");
	sounds[static_cast<int>(SoundAsset::FootStone)] = LoadSound("assets/sfx/Concrete1.wav");
	sounds[static_cast<int>(SoundAsset::Attack)] = LoadSound("assets/sx/07_human_atk_sword_2.wav");
	sounds[static_cast<int>(SoundAsset::Death)] = LoadSound("assets/sfx/24_orc_death_spin.wav");
	sounds[static_cast<int>(SoundAsset::Coins)] = LoadSound("assets/sfx/handleCoins.ogg");
	sounds[static_cast<int>(SoundAsset::Laser)] = LoadSound("assets/sfx/retro_laser.mp3");

	music[static_cast<int>(MusicAsset::LightAmbience)] = LoadMusicStream("assets/tracks/light-ambience.mp3");
	music[static_cast<int>(MusicAsset::DarkAmbience)] = LoadMusicStream("assets/tracks/dark-ambience.mp3");
}

// play sound
void Audio::play_sound(SoundAsset track) {
	PlaySound(sounds[static_cast<int>(track)]);
}

// start a new music stream
void Audio::play_music(MusicAsset track) {
	PlayMusicStream(music[static_cast<int>(track)]);
}

// load next chunk of music to buffer
void Audio::update_music(MusicAsset track) {
	UpdateMusicStream(music[static_cast<int>(track)]);

}
// stop music
void Audio::stop_music(MusicAsset track) {
	PlayMusicStream(music[static_cast<int>(track)]);
}

// shutdwon audio
void Audio::shutdown_audio() {

	for (auto& sound : sounds) {

		UnloadSound(sound);
	}

	for (auto& m : music) {

		StopMusicStream(m); 
		UnloadMusicStream(m);
	}

	//CloseAudioDevice();
}