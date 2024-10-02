// Copyright © 2023 March Networks Corporation. All rights reserved.

#ifndef __MN_MEDIA_QADPFRAMES__H__
#define __MN_MEDIA_QADPFRAMES__H__

#include "QADPAudioFrame.h"
#include "QADPAuxiliaryDataFrame.h"
#include "QADPFrameType.h"
#include "QADPTextFrame.h"
#include "QADPVideoFrame.h"

namespace marchnetworks
{
	namespace media
	{
		namespace protocol
		{
			namespace qadp
			{
				template <QADPFrameType _qadpFrameType>
				struct QADPFrames;

				template <>
				struct QADPFrames<QADPFrameType::Audio>
				{
					using frameType = QADPAudioFrame;
					using headerType = QADPFrameAudioDataHeader;
				};

				template <>
				struct QADPFrames<QADPFrameType::AuxiliaryData>
				{
					using frameType = QADPAuxiliaryDataFrame;
					using headerType = QADPFrameAuxiliaryDataHeader;
				};

				template <>
				struct QADPFrames<QADPFrameType::Text>
				{
					using frameType = QADPTextFrame;
					using headerType = QADPFrameTextDataHeader;
				};

				template <>
				struct QADPFrames<QADPFrameType::Video>
				{
					using frameType = QADPVideoFrame;
					using headerType = QADPFrameVideoDataHeader;
				};
			}
		}
	}
}

#endif
