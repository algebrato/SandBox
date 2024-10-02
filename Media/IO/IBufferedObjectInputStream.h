// Copyright © 2024 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_IBUFFEREDOBJECTINPUTSTREAM__H__
#define __MN_MEDIA_IBUFFEREDOBJECTINPUTSTREAM__H__

#include "IObjectInputStream.h"
#include "ISeekable.h"

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			template <typename T, typename... TPosition>
			class IBufferedObjectInputStream : public IObjectInputStream<T>, public ISeekable<TPosition>...
			{
			public:
				IBufferedObjectInputStream() {};
				virtual ~IBufferedObjectInputStream() {};
			};
		}
	}
}

#endif
