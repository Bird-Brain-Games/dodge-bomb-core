/*Campbell Hamilton 100582048
january 2017
*/
#include "sound engine.h"

//initializes our sound engine as a global variable
SoundEngine Sound::sys;

//checks whether functions returned an error or not
void checkResult(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::string msg = FMOD_ErrorString(result);
	}
}

//initializes the sound engine to default variables
SoundEngine::SoundEngine()
{
	initialized = false;

	system = NULL;
	driverData = NULL;

	forward = { 0.0f, 0.0f, 1.0f };
	up = { 0.0f, 1.0f, 0.0f };
	listenerpos = { 0.0f, 5.0f, 0.0f };
	vel = { 0.0f, 0.0f, 0.0f };

}

// deconstructs the sound engine
SoundEngine::~SoundEngine()
{
	if (initialized)
	{
		result = system->close(); checkResult(result);
		result = system->release(); checkResult(result);
	}
}

//Loads all the variables with the correct values
bool SoundEngine::init()
{
	//checks whether we have initialized already
	if (!initialized)
	{
		//Create a System object and initialize.
		result = FMOD::System_Create(&system); checkResult(result);
		result = system->getVersion(&version); checkResult(result);

		if (version < FMOD_VERSION) // error checking
		{
			std::cout << "FMOD lib version does not match. \n";
			return false;
		}

		//init the system
		result = system->init(100, FMOD_INIT_NORMAL, driverData);
		checkResult(result);//error checking

		if (result != FMOD_OK)
		{
			std::cout << "Failed to initialize sound card drivers\n";
			return false;
		}

		//Set the distance unit (meters/feet etc).
		result = system->set3DSettings(1.0, 1.0, 3.0f);
		checkResult(result);
		initialized = true;
	}
	return true;

}

//updates the sound engine
void SoundEngine::update()
{
	//Sets the listerns position
	result = system->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up);
	checkResult(result);

	//updates the system
	result = system->update();
	checkResult(result);
}

//Updates the sound's position and calls the engines update function
void Sound::updateSound(FMOD_VECTOR _pos, FMOD_VECTOR _vel)
{
	sys.result = channel->set3DAttributes(&_pos, &_vel);
	checkResult(sys.result);
	sys.update();
}

//Initializes sound variables to the default values
Sound::Sound()
{
	sound = NULL; // the sounds where loading
	channel = NULL; //limited. at least 32. 0 = null
	pos = { 0.0f, 0.0f, 0.0f };
	vel = { 0.0f, 0.0f, 0.0f };
}

//deconstructs the sound
Sound::~Sound()
{
	sys.result = sound->release(); checkResult(sys.result);
}

//loads the sound from the file path specificied
bool Sound::load(char * filename, bool isLoop)
{
	//initializes the sys
	sys.init();

	//creates the sound from the given filename
	sys.result = sys.system->createSound(filename, FMOD_3D, 0, &sound); checkResult(sys.result);


	if (sys.result != FMOD_OK)//checks whether the file opened properly or not
	{
		std::cout << "could not open the file" << filename << std::endl;
		checkResult(sys.result);
		return false;
	}

	//sets how the sound behaves as its position changes
	sys.result = sound->set3DMinMaxDistance(0.5f, 50.0f); checkResult(sys.result);

	//Sets how the sound will play (loop, single play etc)
	if (isLoop)
	{
		sys.result = sound->setMode(FMOD_LOOP_NORMAL); 
		checkResult(sys.result);
	}
	else
	{
		sys.result = sound->setMode(FMOD_LOOP_OFF);
		checkResult(sys.result);
	}
	
	return true;
}

//plays the sound
void Sound::play()
{
	//plays the sound
	sys.result = sys.system->playSound(sound, 0, true, &channel); checkResult(sys.result);
	//sets the sound position
	sys.result = channel->set3DAttributes(&pos, &vel); checkResult(sys.result);
	//Unpauses the sound
	sys.result = channel->setPaused(false); checkResult(sys.result);
}

void Sound::pause()
{
	sys.result = channel->setPaused(true); checkResult(sys.result);
}

//changes how the sound rolloff works
void Sound::setRolloff(bool isLinear, float min, float max)
{
	channel->set3DMinMaxDistance(min, max);

	if (isLinear)
		channel->setMode(FMOD_3D_LINEARROLLOFF);
	else
		channel->setMode(FMOD_3D_INVERSEROLLOFF);
}