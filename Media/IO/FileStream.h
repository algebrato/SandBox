// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_FILESTREAM__H__
#define __MN_MEDIA_FILESTREAM__H__

#include "IInputStream.h"
#include "IOutputStream.h"
#include "StreamTypes.h"

#include <string>

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			class FileStream : public IInputStream, public IOutputStream
			{
			public:
				FileStream(const std::string& fullpath, bool isBinaryFormat = false, bool hasFileToBeCreated = false);
				virtual ~FileStream();

				void Open() override;
				void Close() override;

				std::string GetFullpath() const;
				std::size_t GetLength() const;

				bool IsEndOfStream() const override;
				bool IsOpened() const override;

				StreamPosition GetReadPosition() const override;
				StreamPosition GetWritePosition() const override;

				std::size_t Read(uint8_t* buffer, std::size_t size) override;
				std::size_t Write(const uint8_t* bytes, std::size_t size) override;

			protected:
				StreamPosition SeekFile(StreamPosition position);

			private:
				int m_fd;
				int m_flags;
				std::string m_fullpath;
			};
		}
	}
}

#endif
