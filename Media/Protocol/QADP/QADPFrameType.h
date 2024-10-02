// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMETYPE__H__
#define __MN_MEDIA_QADPFRAMETYPE__H__

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				enum class QADPFrameType
				{
					Unknown = 0,
					Video = 1,
					Audio = 2,
					EndOfStream = 3,
					Fault = 4,
					Text = 5,
					Reset = 6,
					AuxiliaryData = 7,
					KeepAlive = 255,
					Deepath = 256, // for internal use only, not used by IDCP/QADP protocol
				};
			}
		}
	}
}

#endif
