// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPMETADATAPACKETTYPE__H__
#define __MN_MEDIA_QADPMETADATAPACKETTYPE__H__

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				enum class QADPMetadataPacketType
				{
					ImageAspectRatio = 0x000000A,
					ImageEnhancements,
					ImageTransforms,
					StreamChannelStatus,
					LensConfiguration,
					SmallMosaicPrivacyPatches,
					MosaicPrivacyPatches = 0x000000F0,
					BlackPrivacyPatches = 0x00000F00,
					DriveSerialNumber = 0x000000A0,
					Seproban = 0x000000B0,
					AudioConfiguration = 0x1000000A
				};
			}
		}
	}
}

#endif
