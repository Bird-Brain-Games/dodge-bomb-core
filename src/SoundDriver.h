#pragma once
#include "sound engine.h"

//Music Declarations


class SoundDriver
{
public:
	SoundDriver();
	~SoundDriver();

	void loadSounds();//Loads sounds into the engine

	void playSounds(int _id);//Plays sound as denoted by referenced value

	void pauseSounds(int _id);//Plays sound as denoted by referenced value

	Sound menuTheme_music;

	//FX Declarations
	Sound menuSelect_fx;
	Sound menuSwitch_fx;

private:
	bool audio_init;

};



