/*Campbell Hamilton 100582048
january 2017
*/
#include "sound engine.h"
#include <iostream>
#include "FMOD\fmod_errors.h"

//initializes our sound engine as a global variable
SoundEngine Sound::sys;

//checks whether functions returned an error or not
void checkResult(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::string msg = FMOD_ErrorString(result);
		std::cerr << msg << std::endl;
	}
}

//initializes the sound engine to default variables
SoundEngine::SoundEngine()
{
	initialized = false;

	system = NULL;
	driverData = NULL;

	forward			= { 0.0f, 0.0f, 1.0f };
	up				= { 0.0f, 1.0f, 0.0f };
	listenerpos		= { 23.0f, 55.0f, 0.0f };
	vel				= { 0.0f, 0.0f, 0.0f };

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
		result = system->set3DSettings(1.0, 10.0, 1.0f);
		checkResult(result);
		initialized = true;

		
	}
	FMOD_SPEAKERMODE* speakerMode = nullptr;
	result = system->getSoftwareFormat(NULL, speakerMode, NULL);
	checkResult(result);
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

void Sound::setPosition(FMOD_VECTOR _pos)
{
	setPosition(_pos, vel);
}

void Sound::setPosition(FMOD_VECTOR _pos, FMOD_VECTOR _vel)
{
	pos = _pos;
	vel = _vel;

	if (isPlaying())
	{
		sys.result = channel->set3DAttributes(&pos, &vel);
		checkResult(sys.result);
	}
}

void Sound::setPosition(glm::vec3 _pos)
{
	setPosition(_pos, glm::vec3(vel.x, vel.y, vel.z));
}

void Sound::setPosition(glm::vec3 _pos, glm::vec3 _vel)
{
	FMOD_VECTOR _fPos, _fVel;
	_fPos = { _pos.x, _pos.y, _pos.z };
	_fVel = { _vel.x, _vel.y, _vel.z };
	setPosition(_fPos, _fVel);
}

//Initializes sound variables to the default values
Sound::Sound()
{
	sound = NULL; // the sounds where loading
	channel = NULL; //limited. at least 32. 0 = null
	pos = { 0.0f, 0.0f, 0.0f };
	vel = { 0.0f, 0.0f, 0.0f };
	volume = 1.0f;
}

Sound::Sound(std::string filename, bool isLoop)
	: Sound()
{
	load(filename, isLoop);
}

Sound::Sound(const Sound& other)
	: sound(other.sound),
	channel(NULL),
	volume(other.volume)
{
	pos = { 0.0f, 0.0f, 0.0f };
	vel = { 0.0f, 0.0f, 0.0f };
}



//deconstructs the sound
Sound::~Sound()
{
	sound = nullptr;
	channel = nullptr;
}

// releases the sound
void Sound::release()
{
	sys.result = sound->release(); checkResult(sys.result);
	channel = nullptr;
}

//loads the sound from the file path specificied
bool Sound::load(std::string filename, bool isLoop)
{
	//initializes the sys
	sys.init();

	//creates the sound from the given filename
	sys.result = sys.system->createSound(filename.c_str(), FMOD_3D, 0, &sound); checkResult(sys.result);


	if (sys.result != FMOD_OK)//checks whether the file opened properly or not
	{
		std::cout << "could not open the sound file: " << filename << std::endl;
		checkResult(sys.result);
		return false;
	}

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
	//if (isPlaying()) return;

	// plays the sound
	sys.result = sys.system->playSound(sound, 0, true, &channel); checkResult(sys.result);

	// sets the sound position
	sys.result = channel->set3DAttributes(&pos, &vel); checkResult(sys.result);

	// Unpauses the sound
	sys.result = channel->setPaused(false); checkResult(sys.result);

	// Sets the channel volume
	setVolume(volume);

	float minDist, maxDist;
	FMOD_MODE mode;
	setRolloff(true, 2.0f, 1000.0f);
	channel->get3DMinMaxDistance(&minDist, &maxDist);
	channel->getMode(&mode);
	// Sets how the sound behaves as its position changes
}

void Sound::pause()
{
	if (isPlaying())
	{
		sys.result = channel->setPaused(true); 
		checkResult(sys.result);
	}
}

void Sound::resume()
{
	if (isPlaying())
	{
		sys.result = channel->setPaused(false);
		checkResult(sys.result);
	}
}

void Sound::stop()
{
	if (isPlaying())
	{
		sys.result = channel->stop();
		checkResult(sys.result);
	}
}

//changes how the sound rolloff works
void Sound::setRolloff(bool isLinear, float min, float max)
{
	if (isPlaying())
	{
		sys.result = channel->set3DMinMaxDistance(min, max); checkResult(sys.result);

		if (isLinear)
			sys.result = channel->setMode(FMOD_3D_LINEARROLLOFF);
		else
			sys.result = channel->setMode(FMOD_3D_INVERSEROLLOFF);
		checkResult(sys.result);
	}
}

void Sound::setVolume(float v)
{
	volume = v;
	if (isPlaying())
	{
		sys.result = channel->setVolume(volume); checkResult(sys.result);
	}
}

bool Sound::isPlaying()
{
	if (channel == NULL) return false;

	FMOD::Sound* channelSound;
	sys.result = channel->getCurrentSound(&channelSound);
	if (sound == channelSound)
	{
		bool isPlaying;
		sys.result = channel->isPlaying(&isPlaying); checkResult(sys.result);
		if (isPlaying)
			return true;
	}
	return false;
}