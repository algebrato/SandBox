// Copyright © 2024 March Networks Corporation. All rights reserved.

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "ByteArrayStream.h"

using marchnetworks::media::io::ByteArrayStream;
using marchnetworks::media::io::StreamPosition;

ByteArrayStream::ByteArrayStream(std::unique_ptr<uint8_t[]>&& byteArray, std::size_t size) :
	m_byteArray(std::move(byteArray)),
	m_size(size),
	m_position(0)
{
}

ByteArrayStream::~ByteArrayStream()
{
}

void ByteArrayStream::Open()
{
}

void ByteArrayStream::Close()
{
}

bool ByteArrayStream::IsEndOfStream() const
{
	return m_position == EndOfStream;
}

bool ByteArrayStream::IsOpened() const
{
	return true;
}

StreamPosition ByteArrayStream::GetReadPosition() const
{
	return m_position;
}

std::size_t ByteArrayStream::Read(uint8_t* buffer, std::size_t size)
{
	std::size_t totalBytesRead = 0;

	if (!IsEndOfStream())
	{
		auto maxReadBytes = m_size - m_position;
		auto bytesToRead = size < maxReadBytes ? size : maxReadBytes;

		memcpy(buffer, m_byteArray.get() + m_position, bytesToRead);

		m_position += bytesToRead;
		if (m_position >= m_size)
			m_position = EndOfStream;

		totalBytesRead = bytesToRead;
	}

	return totalBytesRead;
}
