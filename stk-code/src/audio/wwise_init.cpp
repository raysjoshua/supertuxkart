
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>  
#include <AK/Tools/Common/AkPlatformFuncs.h>  
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AkFilePackageLowLevelIOBlocking.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h> 
#include <assert.h>

#include "wwise_init.hpp"

#ifndef AK_OPTIMIZED
	#include <AK/Comm/AkCommunication.h>

#endif // AK_OPTIMIZED


CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

WWise* wwise_manager = NULL;

bool  WWise::InitSoundEngine() {
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success) {
		assert(!"Could not create the memory manager.");
		return false;
	}

	//
	// Create and initialize an instance of the default streaming manager. Note
	// that you can override the default streaming manager with your own. 
	//

	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings here.


	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}

	// Create Sound Engine

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success) 
	{
		assert(!"Could not initialize the Sound Engine");
		return false;
	}

	// Initialize Music Engine
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success) {
		assert(!"Could not initialize the Music Engine");
		return false;
	}

	// Initialize Spatial Audio
	AkSpatialAudioInitSettings settings;
	if (AK::SpatialAudio::Init( settings ) != AK_Success) {
		assert(!"Could not initialize the Spatial Audio");
		return false;
	}

	#ifndef AK_OPTIMIZED
		// Initilize Communications
		AkCommSettings commSettings;
		AK::Comm::GetDefaultInitSettings(commSettings);
		if (AK::Comm::Init(commSettings) != AK_Success) {
			assert(!"Could not initialize communication");
			return false;
		}
	#endif // AK_OPTIMIZED


	return true;
}

void WWise::ProcessAudio() {
	AK::SoundEngine::RenderAudio();
}

void WWise::TermSoundEngine() {
	#ifndef AK_OPTIMIZED
		//
		// Terminate Communication Services. Should be done first
		//
		AK::Comm::Term();
	#endif // AK_OPTIMIZED

	AK::MusicEngine::Term();
	AK::SoundEngine::Term();

	// Terminate the streaming device and streaming manager
	// CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
	// that lives in the Stream Manager, and unregisters itself as the File Location Resolver.

	g_lowLevelIO.Term();

	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager
	AK::MemoryMgr::Term();
}