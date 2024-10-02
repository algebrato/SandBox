// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_IINPUTSTREAM__H__
#define __MN_MEDIA_IINPUTSTREAM__H__

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
			class IInputStream : virtual public IStream
			{
			public:
				IInputStream() {};
				virtual ~IInputStream() {};

				virtual StreamPosition GetReadPosition() const = 0;
				virtual std::size_t Read(uint8_t* buffer, std::size_t size) = 0;
			};
		}
	}
}

#endif
