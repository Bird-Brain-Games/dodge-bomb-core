#pragma once
/*Campbell Hamilton 100582048
january 2017
*/
#include "FMOD\fmod.hpp"
#include "Windows.h"
#include "iostream"
#include "FMOD\fmod_errors.h"
// 50ms update for interface
// Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

void checkResult(FMOD_RESULT result);

class SoundEngine
{
public:
	//de/constructors
	SoundEngine();
	~SoundEngine();
	
	//initializes the variables
	bool init();
	//updates the variables
	void update();

	//engine variables
	FMOD_RESULT		 result;
	unsigned int     version;

	FMOD::System    *system;
	void            *driverData;

	//listener variables
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	FMOD_VECTOR vel;
	FMOD_VECTOR listenerpos;

private:
	//whether its been initialized or not
	bool initialized;
};

//loads sounds
class Sound
{
public:
	//the sound 
	Sound();
	~Sound();

	//loads the specificied file
	bool load(char * filename);
	//plays the sound loaded
	void play();
	//updates the sounds position and calls the engines update function
	void updateSound(FMOD_VECTOR _pos, FMOD_VECTOR _vel);
	//changes what method of rollof is being used.
	void switchSoundMode(FMOD_MODE);
	//pauses the sound
	void pause();
	//sound variables
	FMOD::Sound   *sound = NULL;
	FMOD::Channel *channel = NULL;
	FMOD_VECTOR pos;
	FMOD_VECTOR vel;

	//a variable that allows us to access the sound engine
	static SoundEngine sys;
};

