// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_STREAMTYPES__H__
#define __MN_MEDIA_STREAMTYPES__H__

#include <cstddef>
#include <limits>

namespace marchnetworks
{
	namespace media
	{
		namespace io
		{
			typedef long long StreamPosition;
			typedef long long StreamOffset;

			constexpr StreamOffset StreamOffsetError = (std::numeric_limits<StreamOffset>::min)();
			constexpr StreamPosition StreamPositionError = (std::numeric_limits<StreamPosition>::min)();
			constexpr StreamPosition EndOfStream = (std::numeric_limits<StreamPosition>::max)();

			enum class StreamSeekDirection
			{
				begin,
				current,
				end
			};
		}
	}
}

#endif
