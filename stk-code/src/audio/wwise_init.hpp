#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>  
#include <AK/Tools/Common/AkPlatformFuncs.h>  
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AkFilePackageLowLevelIOBlocking.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h> 
#include <assert.h>
#include <string>
#include <irrString.h>

class WWise {
public:
	enum Transport { PLAY, STOP, RESUME, PAUSE };
	bool InitSoundEngine();
	void RegisterGameObject(int ID, const char* object);
	void UnRegisterGameObject(int ID);
	void SetGameObjectPosition(const int ID, const float X, const float Y, const float Z);
	void RegisterDefaultListener(AkGameObjectID ID);
	void UnRegisterDefaultListener(AkGameObjectID ID);
	void RegisterGameEmitterListener(const char* Emitter, AkGameObjectID EmitterID, const char* Listener, AkGameObjectID ListenerID);
	void PostEvent(int ID, const char* Event);
	void PostEventUI(const char* Event);

	void PostEventOST(const char* Event, Transport Action);
	bool IsOSTPlaying();
	bool IsOSTPlaying(const char* Event);

	void PostNegativeExpletive(const irr::core::stringw Event, AkGameObjectID ID);
	void PostPositiveExpletive(const irr::core::stringw Event, AkGameObjectID ID);
	void PostDialogue(const irr::core::stringw Event, const char* argPath[], const int pathDepth);
	void PostDialogue(const char* Event, const char* argPath[], const int pathDepth);
	void PostDialogue(AkGameObjectID ID, const irr::core::stringw Event, const char* argPath[], const int pathDepth);
	void PostDialogue(AkGameObjectID ID, const char* Event, const char* argPath[], const int pathDepth);
	void SetGameSyncSwitch(const char* Group, const char* State, const int ID);
	void SetGameSyncParameter(const char* Group, const AkRtpcValue Value, const int ID);
	void ProcessAudio();
	void TermSoundEngine();
};
extern WWise* wwise_manager;