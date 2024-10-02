// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMEFLAGS__H__
#define __MN_MEDIA_QADPFRAMEFLAGS__H__

#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				struct QADPFrameFlags
				{
					QADPFrameFlags() = delete;

					static constexpr uint32_t MEDIA_KEY_FRAME = 1;
				};
			}
		}
	}
}

#endif
