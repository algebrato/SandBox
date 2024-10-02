// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPPRIVACYPATCHTYPE__H__
#define __MN_MEDIA_QADPPRIVACYPATCHTYPE__H__

#include "QADPMetadataPacketType.h"

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				enum class QADPPrivacyPatchType
				{
					SmallMosaic = static_cast<int>(QADPMetadataPacketType::SmallMosaicPrivacyPatches),
					Mosaic = static_cast<int>(QADPMetadataPacketType::MosaicPrivacyPatches),
					Black = static_cast<int>(QADPMetadataPacketType::BlackPrivacyPatches)
				};
			}
		}
	}
}

#endif
