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

class WWise {
public:
	bool InitSoundEngine();
	void PostEvent(char* Event);
	void SetGameSyncState(char* Group, char* State);
	void SetGameSyncParameter(char* Group, AkRtpcValue Value);
	void ProcessAudio();
	void TermSoundEngine();
};
extern WWise* wwise_manager;