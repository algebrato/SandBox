// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_IOUTPUTSTREAM__H__
#define __MN_MEDIA_IOUTPUTSTREAM__H__

#include "IStream.h"
#include "StreamTypes.h"

#include <cstddef>
#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			class IOutputStream : virtual public IStream
			{
			public:
				IOutputStream() {};
				virtual ~IOutputStream() {};

				virtual StreamPosition GetWritePosition() const = 0;
				virtual std::size_t Write(const uint8_t* bytes, std::size_t size) = 0;
			};
		}
	}
}

#endif
