#pragma once
#include"Framework.h"
#include "../Common.h"

/**
@class          SoundManager
@brief  		offer a VERY simple interface to play background music & sound effect
*/
class SoundManager : public Singleton<SoundManager>
{
public:
    SoundManager();
    ~SoundManager(); 

	static void Init();

    /**
    @brief Release the shared Engine object
    @warning It must be called before the application exit, or a memroy leak will be casued.
    */
	static void Shutdown();

    /**
     @brief execute every frame
     @param none
     */
    void Update();
	
	void PlaySound(const char* filename);

	void StopSound();

	void PauseSound();

	void ResumeSound();
	
	bool IsShutdown();
private:
		ALuint      uiBuffer;
		ALuint      uiSource;  
		ALint       iState;
		static bool		m_isShutdown;


};