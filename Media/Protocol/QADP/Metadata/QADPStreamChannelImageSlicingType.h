// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPSTREAMCHANNELIMAGESLICINGTYPE__H__
#define __MN_MEDIA_QADPSTREAMCHANNELIMAGESLICINGTYPE__H__

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				enum class QADPStreamChannelImageSlicingType
				{
					NoSlicing = 0,
					Interlaced = 2,
					Sliced_2x2 = 4,
					Sliced_3x3 = 9,
					Sliced_4x4 = 16
				};
			}
		}
	}
}

#endif
