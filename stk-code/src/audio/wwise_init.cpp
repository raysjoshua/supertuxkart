
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>  
#include <AK/Tools/Common/AkPlatformFuncs.h>  
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AkFilePackageLowLevelIOBlocking.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h> 
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkQueryParameters.h>
#include <AK/SoundEngine/Common/AkDynamicDialogue.h>
#include <AK/SoundEngine/Common/AkDynamicSequence.h>
#include <assert.h>
#include <irrString.h>

#include "wwise_init.hpp"

#ifndef AK_OPTIMIZED
	#include <AK/Comm/AkCommunication.h>

#endif // AK_OPTIMIZED
#include <string>

AkGameObjectID MY_DEFAULT_LISTENER = 0;

#define BANKNAME_INIT L"Init.bnk"
#define BANKNAME_THEME_MUSIC L"theme_music.bnk"
#define BANKNAME_KART L"kart_soundbank.bnk"
#define BANKNAME_VOX L"vox_soundbank.bnk"
#define BANKNAME_UI L"ui_soundbank.bnk"

const AkGameObjectID OST_EMITTER = 1000;
const AkGameObjectID OST_LISTENER = 1001;
const AkGameObjectID MENU_EMITTER = 1002;
const AkGameObjectID MENU_LISTENER = 1003;
const AkGameObjectID GAME_OBJECT_EMITTER = 1004;
const AkGameObjectID GAME_OBJECT_LISTENER = 1005;


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
		assert(!"Could not initialize the Sound Fngine");
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

	eResult = AK::SoundEngine::LoadBank(BANKNAME_VOX, bankID);
	assert(eResult == AK_Success);

	eResult = AK::SoundEngine::LoadBank(BANKNAME_UI, bankID);
	assert(eResult == AK_Success);

	// Register Game Emitter/Listeners

	AK::SoundEngine::RegisterGameObj(MENU_LISTENER, "Menu-Listener");
	AK::SoundEngine::SetListeners(MENU_LISTENER, &MENU_LISTENER, 1);

	AK::SoundEngine::RegisterGameObj(OST_LISTENER, "OST_LISTENER");
	AK::SoundEngine::SetListeners(OST_LISTENER, &OST_LISTENER, 1);

	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_LISTENER, "GAME_OBJECT_LISTENER");
	AK::SoundEngine::SetListeners(GAME_OBJECT_LISTENER, &GAME_OBJECT_LISTENER, 1);


	return true;
}


void WWise::RegisterDefaultListener(AkGameObjectID ID) {
	AK::SoundEngine::SetListeners(ID, &ID, 1);
}

void WWise::UnRegisterDefaultListener(AkGameObjectID ID) {
	//AK::SoundEngine::RemoveDefaultListener(ID);
}


void WWise::RegisterGameEmitterListener(const char* Emitter, AkGameObjectID EmitterID, const char* Listener, AkGameObjectID ListenerID) {
	AK::SoundEngine::RegisterGameObj(EmitterID, Emitter);
	AK::SoundEngine::RegisterGameObj(ListenerID, Listener);
	AK::SoundEngine::SetListeners(EmitterID, &ListenerID, 1);
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
	AK::SoundEngine::PostEvent(Event, MENU_LISTENER);
}

void WWise::PostEventOST(const char* Event, Transport Action) {
	wwise_manager->SetGameSyncSwitch("ost", Event, OST_LISTENER);
	std::string event = Event;
	switch (Action) {
	case Transport::PLAY:
		event = "play_" + event;
		break;
	case Transport::STOP:
		event = "stop_" + event;
		break;
	case Transport::RESUME:
		event = "resume_" + event;
		break;
	case Transport::PAUSE:
		event = "pause_" + event;
		break;
	default:
		event = "play_" + event;
		break;
	}
	AK::SoundEngine::PostEvent(event.c_str(), OST_LISTENER);
}

bool WWise::IsOSTPlaying(const char* Event) {

	AkUInt32 size = 100;
	AkPlayingID argPath[100] = { 0 };
	AKRESULT result = AK::SoundEngine::Query::GetPlayingIDsFromGameObject(OST_LISTENER, size, argPath);


	for (int i = 0; i < size; i++)
	{
		AkPlayingID playingId = argPath[i];
		AkUniqueID temp =  AK::SoundEngine::Query::GetEventIDFromPlayingID(playingId);
		if (playingId > 0) {
			return true;
		}
	}


	return false;
}

bool WWise::IsOSTPlaying() {

	AkUInt32 size = 100;
	AkPlayingID argPath[100] = { 0 };
	AKRESULT result = AK::SoundEngine::Query::GetPlayingIDsFromGameObject(OST_LISTENER, size, argPath);


	for (int i = 0; i < size; i++)
	{
		AkPlayingID playingId = argPath[i];

		if (playingId > 0) {
			return true;
		}
	}


	return false;
}


void WWise::PostNegativeExpletive(const irr::core::stringw Event, AkGameObjectID ID) {
	const char* argPath[2] = {
		"expletive_negative"
	};
	PostDialogue(ID,Event, argPath, 1);
}

void WWise::PostPositiveExpletive(const irr::core::stringw Event, AkGameObjectID ID) {
	const char* argPath[2] = {
		"expletive_positive"
	};
	PostDialogue(ID, Event, argPath, 1);
}

void WWise::PostDialogue(const irr::core::stringw Event, const char* argPath[], const int pathDepth) {
	size_t size = 100;
	char* name = (char*)malloc(size);
	size = std::wcstombs(name, Event.c_str(), size);
	PostDialogue(MENU_LISTENER, name, argPath, pathDepth);
	free(name);
}

void WWise::PostDialogue(const char* Event, const char* argPath[], const int pathDepth) {
	PostDialogue(MENU_LISTENER, Event, argPath, pathDepth);
}

void WWise::PostDialogue(AkGameObjectID ID, const irr::core::stringw Event, const char* argPath[], const int pathDepth) {
	size_t size = 100;
	char* name = (char*)malloc(size);
	size = std::wcstombs(name, Event.c_str(), size);
	PostDialogue(ID, name, argPath, pathDepth);
	free(name);
}

void WWise::PostDialogue(AkGameObjectID ID, const char* Event, const char* argPath[], const int pathDepth) {
	{
		// Open a dynamic sequence using the appropriate game object.

		AkPlayingID sequenceID = AK::SoundEngine::DynamicSequence::Open(ID);

		// Add a single dialogue event to the playlist of the dynamic sequence. 

		{
			// Resolve dialogue event into an audio node ID based on the specified argument path.

			AkUniqueID nodeID =
				AK::SoundEngine::DynamicDialogue::ResolveDialogueEvent(Event, argPath, pathDepth);

			// Add audio node ID to dynamic sequence playlist.

			AK::SoundEngine::DynamicSequence::Playlist* pPlaylist =
				AK::SoundEngine::DynamicSequence::LockPlaylist(sequenceID);

			pPlaylist->Enqueue(nodeID);

			AK::SoundEngine::DynamicSequence::UnlockPlaylist(sequenceID);
		}

		// Play the dynamic sequence.

		AK::SoundEngine::DynamicSequence::Play(sequenceID);

		// Close the dynamic sequence. The dynamic sequence will play until finished and then
		// deallocate itself automatically. 

		AK::SoundEngine::DynamicSequence::Close(sequenceID);
	}
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