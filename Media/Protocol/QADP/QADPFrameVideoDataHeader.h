// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMEVIDEODATAHEADER__H__
#define __MN_MEDIA_QADPFRAMEVIDEODATAHEADER__H__

#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
#pragma pack(push, 1)
				struct QADPFrameVideoDataHeader
				{
					int32_t requestID;
					int32_t width; // unused
					int32_t height; // unused
					char codec[16];
				};
#pragma pack(pop)
			}
		}
	}
}

#endif
