// Copyright © 2024 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_IOBJECTINPUTSTREAM__H__
#define __MN_MEDIA_IOBJECTINPUTSTREAM__H__

#include "IStream.h"

#include <memory>

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			template <typename T>
			class IObjectInputStream : virtual public IStream
			{
			public:
				IObjectInputStream() {};
				virtual ~IObjectInputStream() {};

				virtual std::shared_ptr<T> Read() = 0;
			};
		}
	}
}

#endif
