// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPSTREAMCHANNELBUSYSTATUS__H__
#define __MN_MEDIA_QADPSTREAMCHANNELBUSYSTATUS__H__

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				enum class QADPStreamChannelBusyStatus
				{
					Streaming = 0,
					NotStreamingIntentionally,
					NotStreamingForConcomitantSeprobanAlarm
				};
			}
		}
	}
}

#endif
