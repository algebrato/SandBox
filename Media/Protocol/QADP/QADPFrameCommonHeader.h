// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMECOMMONHEADER__H__
#define __MN_MEDIA_QADPFRAMECOMMONHEADER__H__

#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				static constexpr uint32_t DefaultMediaPacketFramingID = 0x12345679;

#pragma pack(push, 1)
				struct QADPFrameCommonHeader
				{
					uint32_t mediaPacketFramingID;
					int32_t mediaPacketFramingType;
					int32_t frameSequenceNumber;
					int32_t metadataSize;
					int32_t dataSize;
					uint32_t realTimestampHighOrderBits;
					uint32_t realTimestampLowOrderBits;
					uint32_t synchronizationTimestampHighOrderBits;
					uint32_t synchronizationTimestampLowOrderBits;
					uint32_t flags;
				};
#pragma pack(pop)
			}
		}
	}
}

#endif
