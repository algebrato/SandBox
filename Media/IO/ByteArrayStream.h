// Copyright © 2024 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_BYTEARRAYSTREAM__H__
#define __MN_MEDIA_BYTEARRAYSTREAM__H__

#include "IInputStream.h"
#include "StreamTypes.h"

#include <memory>
#include <string>

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			class ByteArrayStream : public IInputStream
			{
			public:
				ByteArrayStream(std::unique_ptr<uint8_t[]>&& byteArray, std::size_t size);
				virtual ~ByteArrayStream();

				void Open() override;
				void Close() override;

				bool IsEndOfStream() const override;
				bool IsOpened() const override;

				StreamPosition GetReadPosition() const override;

				std::size_t Read(uint8_t* buffer, std::size_t size) override;

			private:
				std::unique_ptr<uint8_t[]> m_byteArray;
				std::size_t m_size;
				StreamPosition m_position;
			};
		}
	}
}

#endif
