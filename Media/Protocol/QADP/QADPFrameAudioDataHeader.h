// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMEAUDIODATAHEADER__H__
#define __MN_MEDIA_QADPFRAMEAUDIODATAHEADER__H__

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
				struct QADPFrameAudioDataHeader
				{
					int32_t requestID;
					char numberOfChannels;
					char numberOfBitsPerSample;
					int16_t playbackSpeedPercentage;
					int32_t samplingRate;
					char codec[8];
					char encoderID[8];
				};
#pragma pack(pop)
			}
		}
	}
}

#endif
