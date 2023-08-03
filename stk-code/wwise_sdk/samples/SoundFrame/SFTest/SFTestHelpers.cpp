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

#include "stdafx.h"

#include "SFTestHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------

ReadBytesOnFILE::ReadBytesOnFILE( FILE * in_pFile )
	: m_pFile( in_pFile )
{
	_ASSERT( m_pFile );
}

bool ReadBytesOnFILE::ReadBytes( void * in_pData, AkInt32 in_cBytes, AkInt32 & out_cRead )
{
	out_cRead = (AkInt32) fread( in_pData, 1, in_cBytes, m_pFile );

	return in_cBytes == out_cRead;
}

// -----------------------------------------------------------------------------

WriteBytesOnFILE::WriteBytesOnFILE( FILE * in_pFile )
	: m_pFile( in_pFile )
{
	_ASSERT( m_pFile );
}

bool WriteBytesOnFILE::WriteBytes( const void * in_pData, AkInt32 in_cBytes, AkInt32 & out_cWritten )
{
	out_cWritten = (AkInt32) fwrite( in_pData, 1, in_cBytes, m_pFile );

	return in_cBytes == out_cWritten;
}
