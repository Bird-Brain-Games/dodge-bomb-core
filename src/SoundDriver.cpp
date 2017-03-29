#include "SoundDriver.h"

SoundDriver::SoundDriver() {
	audio_init = true;
}

SoundDriver::~SoundDriver() {
	audio_init = false;
}

void SoundDriver::loadSounds()//Loads sounds into the engine
{
	menuTheme_music.load("assets/media/MenuTheme.wav");
	menuSelect_fx.load("assets/media/select_fx.wav");
	menuSelect_fx.switchSoundMode(FMOD_LOOP_OFF);
	menuSwitch_fx.load("assets/media/switch_fx.wav");
	menuSwitch_fx.switchSoundMode(FMOD_LOOP_OFF);

}

void SoundDriver::playSounds(int _id)//Plays sound as denoted by referenced value
{
	int soundID = _id;

	switch (soundID)
	{
	case 1://Main Menu
		menuTheme_music.updateSound({ 0.0f, 4.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		menuTheme_music.play();
		break;
	case 2: //Menu Switch
		menuSwitch_fx.updateSound({ 0.0f, 4.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		menuSwitch_fx.play();
		break;
	case 3: //Menu Select
		menuSelect_fx.updateSound({ 0.0f, 4.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		menuSelect_fx.play();
		break;
	default:
		break;
	}
}

void SoundDriver::pauseSounds(int _id)//Plays sound as denoted by referenced value
{
	int soundID = _id;

	switch (soundID)
	{
	case 1://Main Menu
		menuTheme_music.pause();
		break;
	case 2://Menu Select
		menuSelect_fx.pause();
		break;
	default:
		break;
	}
}