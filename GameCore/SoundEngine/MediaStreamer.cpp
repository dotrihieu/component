/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* 
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
* 
* http://www.apache.org/licenses/LICENSE-2.0 
* 
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and limitations under the License.
*/

#include "pch.h"
#include "MediaStreamer.h"

#include <assert.h>

using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((uint32)(byte)(ch0) | ((uint32)(byte)(ch1) << 8) |       \
	((uint32)(byte)(ch2) << 16) | ((uint32)(byte)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

MediaStreamer::MediaStreamer() :
	m_offset(0)
{
	ZeroMemory(&m_waveFormat, sizeof(m_waveFormat));
	m_location = Package::Current->InstalledLocation;
	m_locationPath = Platform::String::Concat(m_location->Path, "\\");
}

MediaStreamer::~MediaStreamer()
{
	delete [] m_data;
}
Platform::Array<byte>^ MediaStreamer::ReadData(
	_In_ Platform::String^ filename
	)
{
	CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {0};
	extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	extendedParams.lpSecurityAttributes = nullptr;
	extendedParams.hTemplateFile = nullptr;

	HANDLE file(
		CreateFile2(
		filename->Data(),
		GENERIC_READ,
		FILE_SHARE_READ,
		OPEN_EXISTING,
		&extendedParams
		)
		);

	assert(file != INVALID_HANDLE_VALUE);

	FILE_STANDARD_INFO fileInfo = {0};
	assert(GetFileInformationByHandleEx(
		file,
		FileStandardInfo,
		&fileInfo,
		sizeof(fileInfo)
		));

	assert(fileInfo.EndOfFile.HighPart == 0);

	Platform::Array<byte>^ fileData = ref new Platform::Array<byte>(fileInfo.EndOfFile.LowPart);

	assert(ReadFile(
		file,
		fileData->Data,
		fileData->Length,
		nullptr,
		nullptr
		) );

	return fileData;
}

void MediaStreamer::Initialize(__in const WCHAR* url)
{
	Platform::Array<byte>^ data = ReadData(ref new Platform::String(url));
	UINT32 length = data->Length;
	const byte * dataPtr = data->Data;
	UINT32 offset = 0;

	DWORD riffDataSize = 0;

	auto ReadChunk = [&length, &offset, &dataPtr, &riffDataSize](DWORD fourcc, DWORD& outChunkSize, DWORD& outChunkPos) -> HRESULT
	{
		while (true)
		{
			if (offset + sizeof(DWORD) * 2 >= length)
			{
				return E_FAIL;
			}

			// Read two DWORDs.
			DWORD chunkType = *reinterpret_cast<const DWORD *>(&dataPtr[offset]);
			DWORD chunkSize = *reinterpret_cast<const DWORD *>(&dataPtr[offset + sizeof(DWORD)]);
			offset += sizeof(DWORD) * 2;

			if (chunkType == MAKEFOURCC('R', 'I', 'F', 'F'))
			{
				riffDataSize = chunkSize;
				chunkSize = sizeof(DWORD);
				outChunkSize = sizeof(DWORD);
				outChunkPos = offset;
			}
			else
			{
				outChunkSize = chunkSize;
				outChunkPos = offset;
			}

			offset += chunkSize;

			if (chunkType == fourcc)
			{
				return S_OK;
			}
		}
	};

	// Locate riff chunk, check the file type.
	DWORD chunkSize = 0;
	DWORD chunkPos = 0;

	assert(ReadChunk(MAKEFOURCC('R', 'I', 'F', 'F'), chunkSize, chunkPos) == S_OK);
	assert(*reinterpret_cast<const DWORD *>(&dataPtr[chunkPos]) == MAKEFOURCC('W', 'A', 'V', 'E'));

	// Locate 'fmt ' chunk, copy to WAVEFORMATEXTENSIBLE.
	assert(ReadChunk(MAKEFOURCC('f', 'm', 't', ' '), chunkSize, chunkPos) == S_OK);
	assert(chunkSize <= sizeof(m_waveFormat));
	CopyMemory(&m_waveFormat, &dataPtr[chunkPos], chunkSize);

	// Locate the 'data' chunk and copy its contents to a buffer.
	assert(ReadChunk(MAKEFOURCC('d', 'a', 't', 'a'), chunkSize, chunkPos) == S_OK);
	m_data = new byte[chunkSize];
	m_data_size = chunkSize;
	
	ZeroMemory(m_data, m_data_size*sizeof(byte));
	CopyMemory(m_data, &dataPtr[chunkPos], chunkSize);

	m_offset = 0;
}

void MediaStreamer::ReadAll(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength)
{
	UINT32 toCopy = GetMaxStreamLengthInBytes() - m_offset;
	if (toCopy > maxBufferSize) toCopy = maxBufferSize;

	CopyMemory(buffer, m_data, toCopy);
	*bufferLength = toCopy;

	m_offset += toCopy;
	if (m_offset > GetMaxStreamLengthInBytes()) m_offset = GetMaxStreamLengthInBytes();
}

void MediaStreamer::Restart()
{
	m_offset = 0;
}