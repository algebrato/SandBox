// Copyright © 2023 March Networks Corporation. All rights reserved.

#include "QADPTextFrame.h"

#include "QADPFrameCommonHeader.h"

using marchnetworks::media::protocol::qadp::QADPFrame;
using marchnetworks::media::protocol::qadp::QADPFrameCommonHeader;
using marchnetworks::media::protocol::qadp::QADPFrameTextDataHeader;
using marchnetworks::media::protocol::qadp::QADPTextFrame;

QADPTextFrame::QADPTextFrame(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<QADPFrameTextDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameTextDataHeader>&& hostByteOrderDataHeader,
	std::unique_ptr<char[]> textSourceId, std::unique_ptr<uint8_t[]> data) :
	QADPFrame(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader), nullptr, std::move(data)),
	m_networkByteOrderDataHeader(std::move(networkByteOrderDataHeader)),
	m_hostByteOrderDataHeader(std::move(hostByteOrderDataHeader)),
	m_textSourceId(std::move(textSourceId))
{
}

QADPTextFrame::~QADPTextFrame()
{
}

std::unique_ptr<QADPTextFrame> QADPTextFrame::Make(std::unique_ptr<QADPFrameCommonHeader>&& networkByteOrderCommonHeader, std::unique_ptr<QADPFrameCommonHeader>&& hostByteOrderCommonHeader,
	std::unique_ptr<QADPFrameTextDataHeader>&& networkByteOrderDataHeader, std::unique_ptr<QADPFrameTextDataHeader>&& hostByteOrderDataHeader,
	std::unique_ptr<char[]> textSourceId, std::unique_ptr<uint8_t[]> data)
{
	if (!networkByteOrderCommonHeader || !hostByteOrderCommonHeader)
		throw std::runtime_error("media packet framing header cannot be null");

	if (!networkByteOrderDataHeader || !hostByteOrderDataHeader)
		throw std::runtime_error("text data header cannot be null");

	if (textSourceId && hostByteOrderDataHeader->sourceIdSize <= 0)
		throw std::runtime_error("unexpected source id with no size specified");

	if (!textSourceId && hostByteOrderDataHeader->sourceIdSize > 0)
		throw std::runtime_error("unexpected null source id with size greater than zero");

	if (data && hostByteOrderCommonHeader->dataSize <= 0)
		throw std::runtime_error("unexpected data with no size specified");

	if (!data && hostByteOrderCommonHeader->dataSize > 0)
		throw std::runtime_error("unexpected null data with size greater than zero");

	return std::unique_ptr<QADPTextFrame>(new QADPTextFrame(std::move(networkByteOrderCommonHeader), std::move(hostByteOrderCommonHeader),
		std::move(networkByteOrderDataHeader), std::move(hostByteOrderDataHeader),
		std::move(textSourceId), std::move(data)));
}

std::string QADPTextFrame::GetEncoding() const
{
	return std::string(reinterpret_cast<const char*>(GetData()), m_hostByteOrderDataHeader->encodingSize);
}

std::string QADPTextFrame::GetPrefix() const
{
	return std::string(reinterpret_cast<const char*>(GetData()) + m_hostByteOrderDataHeader->encodingSize + m_hostByteOrderDataHeader->textSampleTitleLength, m_hostByteOrderDataHeader->textSamplePrefixLength);
}

std::string QADPTextFrame::GetSourceId() const
{
	return std::string(reinterpret_cast<const char*>(m_textSourceId.get()), m_hostByteOrderDataHeader->sourceIdSize);
}

std::string QADPTextFrame::GetText() const
{
	return std::string(reinterpret_cast<const char*>(GetData()) + m_hostByteOrderDataHeader->encodingSize + m_hostByteOrderDataHeader->textSampleTitleLength + m_hostByteOrderDataHeader->textSamplePrefixLength, m_hostByteOrderDataHeader->textSampleLength);
}

std::string QADPTextFrame::GetTitle() const
{
	return std::string(reinterpret_cast<const char*>(GetData()) + m_hostByteOrderDataHeader->encodingSize, m_hostByteOrderDataHeader->textSampleTitleLength);
}
