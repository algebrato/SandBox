// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPAudioFrame.h"

#include "QADPFrameCommonHeader.h"

using marchnetworks::media::protocol::qadp::QADPAudioFrame;
using marchnetworks::media::protocol::qadp::QADPFrame;
using marchnetworks::media::protocol::qadp::QADPFrameAudioDataHeader;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;

QADPAudioFrame::QADPAudioFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<QADPFrameAudioDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAudioDataHeader>&& hostByteOrderDataHeader,
	std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data) :
	QADPFrame(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader), std::move(metadata), std::move(data)),
	m_networkByteOrderDataHeader(std::move(networkByteOrderDataHeader)),
	m_hostByteOrderDataHeader(std::move(hostByteOrderDataHeader))
{
}

QADPAudioFrame::~QADPAudioFrame()
{
}

std::unique_ptr<QADPAudioFrame> QADPAudioFrame::Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<QADPFrameAudioDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameAudioDataHeader>&& hostByteOrderDataHeader,
	std::unique_ptr<uint8_t[]> metadata, std::unique_ptr<uint8_t[]> data)
{
	if (!networkByteOrderCommonHeader || !hostByteOrderCommonHeader)
		throw std::runtime_error("media packet framing header cannot be null");

	if (!networkByteOrderDataHeader || !hostByteOrderDataHeader)
		throw std::runtime_error("audio data header cannot be null");

	if (metadata && hostByteOrderCommonHeader->metadataSize <= 0)
		throw std::runtime_error("unexpected metadata with no size specified");

	if (!metadata && hostByteOrderCommonHeader->metadataSize > 0)
		throw std::runtime_error("unexpected null metadata with size greater than zero");

	if (data && hostByteOrderCommonHeader->dataSize <= 0)
		throw std::runtime_error("unexpected data with no size specified");

	if (!data && hostByteOrderCommonHeader->dataSize > 0)
		throw std::runtime_error("unexpected null data with size greater than zero");

	return std::unique_ptr<QADPAudioFrame>(new QADPAudioFrame(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader),
		std::move(networkByteOrderDataHeader), std::move(hostByteOrderDataHeader),
		std::move(metadata), std::move(data)));
}

std::string QADPAudioFrame::GetCodecName() const
{
	return m_hostByteOrderDataHeader->codec;
}

decltype(QADPFrameAudioDataHeader::numberOfChannels) QADPAudioFrame::GetNumberOfChannels() const
{
	return m_hostByteOrderDataHeader->numberOfChannels;
}

decltype(QADPFrameAudioDataHeader::numberOfBitsPerSample) QADPAudioFrame::GetNumberOfBitsPerSample() const
{
	return m_hostByteOrderDataHeader->numberOfBitsPerSample;
}

decltype(QADPFrameAudioDataHeader::samplingRate) QADPAudioFrame::GetSamplingRate() const
{
	return m_hostByteOrderDataHeader->samplingRate;
}
