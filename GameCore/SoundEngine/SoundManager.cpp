#include "SoundManager.h"

bool SoundManager::m_isShutdown = false;

SoundManager::SoundManager()
{
	/*if(instance == NULL)
	{
		instance = new SoundManager();
	}*/
	
}

SoundManager::~SoundManager()
{
}

void SoundManager::Init()
{
	if(instance == NULL)
	{
		instance = new SoundManager();
	}

	ALFWInit();

	ALFWprintf("PlayStatic Test Application\n");

	if (!ALFWInitOpenAL())
	{
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
		return;
	}

	// Generate an AL Buffer
	
}

void SoundManager::PlaySound(const char *filename)
{
	alGenBuffers( 1, &uiBuffer );
		// Load Wave file into OpenAL Buffer
	if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(filename), uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(filename));
	}

	// Generate a Source to playback the Buffer
    alGenSources( 1, &uiSource );

	// Attach Source to Buffer
	alSourcei( uiSource, AL_BUFFER, uiBuffer );

	// Play Source
    alSourcePlay( uiSource );
	ALFWprintf("Playing Source ");
}

void SoundManager::StopSound()
{
	alSourceStop(uiSource);
}

void SoundManager::PauseSound()
{
	alSourcePause(uiSource);
}

void SoundManager::ResumeSound()
{
	alSourcePlay(uiSource);
}

void SoundManager::Shutdown()
{
	ALFWShutdownOpenAL();
	
	ALFWShutdown();
	m_isShutdown = true;
}

bool SoundManager::IsShutdown()
{
	return m_isShutdown;
}
void SoundManager::Update()
{
	ALFWprintf(".");
	alGetSourcei( uiSource, AL_SOURCE_STATE, &iState);
	if(iState == AL_STOPPED)
	{
		ALFWprintf("\n");
		alDeleteSources(1, &uiSource);
		alDeleteBuffers(1, &uiBuffer);
		alSourceStop(uiSource);
		Shutdown();
	}
}
