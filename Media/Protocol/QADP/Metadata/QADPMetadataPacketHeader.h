// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPMETADATAPACKETHEADER__H__
#define __MN_MEDIA_QADPMETADATAPACKETHEADER__H__

#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				struct QADPMetadataPacketHeader
				{
					int32_t tag;
					int32_t type;
					uint32_t length;
				};
			}
		}
	}
}

#endif
