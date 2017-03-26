#pragma once
#include "sound engine.h"

//Sound Stuff
Sound bg_music;

void initializeSounds()
{
	char* mediaPath = new char[100];
	mediaPath = "../DODGEBOMB_REP/assets/media/MenuTheme.wav";
	bg_music.load(mediaPath);
}

void playSounds(int _id)
{
	int soundID = _id;

	switch (soundID)
	{
	case 1: 
		bg_music.updateSound({ 0.0f, 4.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		bg_music.play();
		break;
	default:
		break;
	}
}