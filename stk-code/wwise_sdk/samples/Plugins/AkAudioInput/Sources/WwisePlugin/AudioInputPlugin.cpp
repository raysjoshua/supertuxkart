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
// AudioInputPlugin.cpp
//
// Audio Input Wwise plugin implementation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioInputPlugin.h"

// Audio input plugin property names
const char* const szAudioInputGain = "AudioInputGain";

// Store current plugin settings into banks when asked to.
bool AudioInput::GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const
{
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szAudioInputGain ) );

	return true;
}

AK_DEFINE_PLUGIN_CONTAINER(AkAudioInput);
AK_EXPORT_PLUGIN_CONTAINER(AkAudioInput);
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(AkAudioInput, AudioInput, AkAudioInputSource);

DEFINE_PLUGIN_REGISTER_HOOK;
DEFINEDUMMYASSERTHOOK;
