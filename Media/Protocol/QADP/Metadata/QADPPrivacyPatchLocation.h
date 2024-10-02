// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPPRIVACYPATCHLOCATION__H__
#define __MN_MEDIA_QADPPRIVACYPATCHLOCATION__H__

#include <cstdint>

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				struct QADPPrivacyPatchLocation
				{
					int16_t left;
					int16_t top;
					int16_t width;
					int16_t height;
				};
			}
		}
	}
}

#endif
