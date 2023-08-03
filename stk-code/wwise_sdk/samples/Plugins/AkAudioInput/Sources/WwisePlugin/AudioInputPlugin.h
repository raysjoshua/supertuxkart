/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Version: v2021.1.5  Build: 7749
  Copyright (c) 2006-2021 Audiokinetic Inc.
*******************************************************************************/
//////////////////////////////////////////////////////////////////////
//
// AudioInputPlugin.h
//
// Audio Input Wwise plugin implementation.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/Wwise/Plugin.h>
#include <AK/SoundEngine/Common/AkCommonDefs.h>

#include "SoundInput.h"

extern const char* const szAudioInputGain;

class AudioInput
	: public AK::Wwise::Plugin::AudioPlugin
{
public:
	AudioInput()
	{
		m_Input.InputOn();
	}
	~AudioInput()
	{
		m_Input.InputOff();
	}

	virtual bool GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const override;

	static void GetFormatCallbackFunc(
		AkPlayingID		in_playingID,   ///< Playing ID (same that was returned from the PostEvent call or from the PlayAudioInput call.
		AkAudioFormat&  io_AudioFormat  ///< Already filled format, modify it if required.
		);

	static void ExecuteCallbackFunc(
		AkPlayingID		in_playingID,  ///< Playing ID (same that was returned from the PostEvent call or from the PlayAudioInput call.
		AkAudioBuffer*	io_pBufferOut  ///< Buffer to fill
		);

private:
	SoundInput m_Input;
};

AK_DECLARE_PLUGIN_CONTAINER(AkAudioInput);
