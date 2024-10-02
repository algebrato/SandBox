// Copyright © 2024 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_ISEEKABLE__H__
#define __MN_MEDIA_ISEEKABLE__H__

#include "StreamTypes.h"

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			template <typename TPosition>
			class ISeekable
			{
			public:
				ISeekable() {};
				virtual ~ISeekable() {};

				virtual TPosition Seek(TPosition position, StreamSeekDirection seekDirection = StreamSeekDirection::begin) = 0;
			};
		}
	}
}

#endif
