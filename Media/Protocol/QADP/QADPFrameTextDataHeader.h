// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMETEXTDATAHEADER__H__
#define __MN_MEDIA_QADPFRAMETEXTDATAHEADER__H__

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
				struct QADPFrameTextDataHeader
				{
					int32_t sourceIdSize;
					int32_t encodingSize;
					int32_t textSampleTitleLength;
					int32_t textSamplePrefixLength;
					int32_t textSampleLength;
				};
#pragma pack(pop)
			}
		}
	}
}

#endif
