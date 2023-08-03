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

#pragma once

#include <AK/IBytes.h>

// -----------------------------------------------------------------------------
// Implementation of the IReadBytes that work on a FILE*
class ReadBytesOnFILE
	: public AK::IReadBytes
{
public:
	ReadBytesOnFILE( FILE * in_pFile );

	virtual bool ReadBytes( void * in_pData, AkInt32 in_cBytes, AkInt32 & out_cRead ) override;

private:
	FILE * m_pFile;
};

// -----------------------------------------------------------------------------
// Implementation of the IWriteBytes that work on a FILE*
class WriteBytesOnFILE
	: public AK::IWriteBytes
{
public:
	WriteBytesOnFILE( FILE * in_pFile );

	virtual bool WriteBytes( const void * in_pData, AkInt32 in_cBytes, AkInt32 & out_cWritten ) override;

private:
	FILE * m_pFile;
};
