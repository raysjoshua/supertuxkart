
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>  
#include <AK/Tools/Common/AkPlatformFuncs.h>  
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AkFilePackageLowLevelIOBlocking.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h> 
#include <AK/SoundEngine/Common/AkTypes.h>
#include <assert.h>

#include "wwise_init.hpp"

#ifndef AK_OPTIMIZED
	#include <AK/Comm/AkCommunication.h>

#endif // AK_OPTIMIZED
#include <string>

AkGameObjectID MY_DEFAULT_LISTENER = 0;

#define BANKNAME_INIT L"Init.bnk"
#define BANKNAME_THEME_MUSIC L"theme_music.bnk"
#define BANKNAME_KART L"kart_soundbank.bnk"
#define BANKNAME_UI L"ui_soundbank.bnk"

const AkGameObjectID GAME_OBJECT_ID_MENU = 100;


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


    // Loading Sound banks
	g_lowLevelIO.SetBasePath(AKTEXT("./GeneratedSoundBanks/"));
	AK::StreamMgr::SetCurrentLanguage( AKTEXT("English(US)"));
	AkBankID bankID;

	AKRESULT eResult = AK::SoundEngine::LoadBank(BANKNAME_INIT, bankID);
	assert(eResult == AK_Success);
	eResult = AK::SoundEngine::LoadBank(BANKNAME_THEME_MUSIC, bankID);
	assert(eResult == AK_Success);

	eResult = AK::SoundEngine::LoadBank(BANKNAME_KART, bankID);
	assert(eResult == AK_Success);

	eResult = AK::SoundEngine::LoadBank(BANKNAME_UI, bankID);
	assert(eResult == AK_Success);

	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_ID_MENU, "Menu");


	return true;
}


void WWise::RegisterGameListener(AkGameObjectID ID) {
	AK::SoundEngine::SetDefaultListeners(&ID, 1);
}

void WWise::UnRegisterGameListener(AkGameObjectID ID) {
	AK::SoundEngine::RemoveDefaultListener(ID);
}

void WWise::RegisterGameObject(int ID, const char* object) {
	AK::SoundEngine::RegisterGameObj(ID, object);
}

void WWise::UnRegisterGameObject(int ID) {
	AK::SoundEngine::UnregisterGameObj(ID);
}

void WWise::PostEvent(int ID, const char* Event) {
	AK::SoundEngine::PostEvent(Event, ID);
}

void WWise::PostEventUI(const char* Event) {
	AK::SoundEngine::PostEvent(Event, GAME_OBJECT_ID_MENU);
}

void WWise::SetGameObjectPosition(const int ID, const float X, const float Y, const float Z ) {
	AkSoundPosition soundPos;
	soundPos.SetPosition(X, Y, Z);
	soundPos.SetOrientation(1, 0, 0, 0, 1, 0);

	AKRESULT eResult = AK::SoundEngine::SetPosition(ID, soundPos);
	assert(eResult == AK_Success);
}

void WWise::SetGameSyncSwitch(const char* Group,const char* State, const int ID) {
	AKRESULT eResult = AK::SoundEngine::SetSwitch(Group, State, ID);
	assert(eResult == AK_Success);
}

void WWise::SetGameSyncParameter(const char* Group, const AkRtpcValue Value, const int ID) {
	AKRESULT eResult = AK::SoundEngine::SetRTPCValue(Group, Value,ID );
	assert(eResult == AK_Success);
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