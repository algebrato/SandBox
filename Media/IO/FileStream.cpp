// Copyright © 2023 - 2024 March Networks Corporation. All rights reserved.

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "FileStream.h"

#include <fcntl.h>
#include <fstream>
#include <functional>
#include <limits>
#ifdef _WIN32
#include <basetsd.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef S_IRUSR
#define S_IRUSR _S_IREAD
#endif
#ifndef S_IWUSR
#define S_IWUSR _S_IWRITE
#endif
#ifndef S_IRGRP
#define S_IRGRP 0x0020
#endif
#ifndef S_IROTH
#define S_IROTH 0x0004
#endif

#define openfd ::_open
#define closefd ::_close
#define readfd ::_read
#define writefd ::_write
#define seekfd ::_lseek

typedef off_t loff_t;
#else
#include <unistd.h>

#define openfd ::open
#define closefd ::close
#define readfd ::read
#define writefd ::write
#define seekfd ::lseek

#define O_BINARY 0x0000
#endif // !_WIN32

using marchnetworks::media::io::FileStream;
using marchnetworks::media::io::StreamPosition;

FileStream::FileStream(const std::string& fullpath, bool isBinaryFormat, bool hasFileToBeCreated) :
	m_fd(-1),
	m_flags((hasFileToBeCreated ? O_CREAT | O_TRUNC | O_RDWR : O_RDWR | O_APPEND) | (isBinaryFormat ? O_BINARY : 0x0000)),
	m_fullpath(fullpath)
{
}

FileStream::~FileStream()
{
	Close();
}

void FileStream::Open()
{
	if (!IsOpened())
	{
		m_fd = openfd(m_fullpath.c_str(), m_flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (!IsOpened())
			throw std::fstream::failure(std::string("cannot open file \"").append(m_fullpath).append("\"").c_str());
	}
}

void FileStream::Close()
{
	if (IsOpened())
	{
		closefd(m_fd);
		m_fd = -1;
	}
}

std::string FileStream::GetFullpath() const
{
	return m_fullpath;
}

std::size_t FileStream::GetLength() const
{
	std::size_t length = 0;

	if (IsOpened())
	{
		auto position = seekfd(m_fd, 0, SEEK_CUR);
		length = seekfd(m_fd, 0, SEEK_END);

		seekfd(m_fd, position, SEEK_SET);
	}

	return length;
}

bool FileStream::IsEndOfStream() const
{
	auto isEndOfStream = true;

	if (IsOpened())
	{
		auto position = seekfd(m_fd, 0, SEEK_CUR);
		auto endOfStream = seekfd(m_fd, 0, SEEK_END);
		isEndOfStream = position == endOfStream;

		seekfd(m_fd, position, SEEK_SET);
	}

	return isEndOfStream;
}

bool FileStream::IsOpened() const
{
	return m_fd > 0;
}

StreamPosition FileStream::GetReadPosition() const
{
	return IsOpened() ? seekfd(m_fd, 0, SEEK_CUR) : StreamPositionError;
}

StreamPosition FileStream::GetWritePosition() const
{
	return IsOpened() ? seekfd(m_fd, 0, SEEK_CUR) : StreamPositionError;
}

std::size_t FileStream::Read(uint8_t* buffer, std::size_t size)
{
	std::size_t totalBytesRead = 0;

	if (IsOpened())
	{
		using readfd_result_type = decltype(std::function{ readfd })::result_type;
		auto bytesToRead = size <= (std::numeric_limits<readfd_result_type>::max)() ? static_cast<readfd_result_type>(size) : (std::numeric_limits<readfd_result_type>::max)();

		while (bytesToRead > 0)
		{
			auto bytesRead = readfd(m_fd, buffer + totalBytesRead, bytesToRead);

			if (bytesRead < 0)
				throw std::fstream::failure(std::string("cannot read from file \"").append(m_fullpath).append("\"").c_str());

			if (bytesRead == 0)
				bytesToRead = 0;
			else
			{
				totalBytesRead += bytesRead;
				bytesToRead -= bytesRead;
			}
		}
	}

	return totalBytesRead;
}

std::size_t FileStream::Write(const uint8_t* bytes, std::size_t size)
{
	std::size_t totalWrittenBytes = 0;

	if (IsOpened())
	{
		using writefd_result_type = decltype(std::function{ writefd })::result_type;
		auto bytesToWrite = size <= (std::numeric_limits<writefd_result_type>::max)() ? static_cast<writefd_result_type>(size) : (std::numeric_limits<writefd_result_type>::max)();

		while (bytesToWrite > 0)
		{
			auto writtenBytes = writefd(m_fd, bytes + totalWrittenBytes, bytesToWrite);

			if (writtenBytes < 0)
				throw std::fstream::failure(std::string("cannot write to file \"").append(m_fullpath).append("\"").c_str());

			if (writtenBytes == 0)
				bytesToWrite = 0;
			else
			{
				totalWrittenBytes += writtenBytes;
				bytesToWrite -= writtenBytes;
			}
		}
	}

	return totalWrittenBytes;
}

StreamPosition FileStream::SeekFile(StreamPosition position)
{
	auto streamPosition = position;

	if (IsOpened())
	{
		if (position > static_cast<std::size_t>((std::numeric_limits<loff_t>::max)()))
		{
			if (seekfd(m_fd, 0, SEEK_SET) < 0)
				throw std::fstream::failure(std::string("cannot set position within file \"").append(m_fullpath).append("\"").c_str());

			do
			{
				if (seekfd(m_fd, (std::numeric_limits<loff_t>::max)(), SEEK_CUR) < 0)
					throw std::fstream::failure(std::string("cannot set position within file \"").append(m_fullpath).append("\"").c_str());

				position -= static_cast<std::size_t>((std::numeric_limits<loff_t>::max)());

			} while (position > static_cast<std::size_t>((std::numeric_limits<loff_t>::max)()));

			if (seekfd(m_fd, static_cast<loff_t>(position), SEEK_CUR) < 0)
				throw std::fstream::failure(std::string("cannot set position within file \"").append(m_fullpath).append("\"").c_str());
		}
		else
		{
			if (seekfd(m_fd, static_cast<loff_t>(position), SEEK_SET) < 0)
				throw std::fstream::failure(std::string("cannot set position within file \"").append(m_fullpath).append("\"").c_str());
		}
	}

	return streamPosition;
}
