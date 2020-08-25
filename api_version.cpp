#include <iostream>

#include <boost/asio/streambuf.hpp>
#include <boost/endian/conversion.hpp>

#include "api_version.h"

ApiVersion::ApiVersion(boost::asio::streambuf &receive_buffer)
{
	std::memcpy(&m_major, boost::asio::buffer_cast<const void*>(receive_buffer.data()), sizeof(m_major));
	receive_buffer.consume(sizeof(m_major));
	boost::endian::little_to_native_inplace(m_major);

	std::memcpy(&m_minor, boost::asio::buffer_cast<const void*>(receive_buffer.data()), sizeof(m_minor));
	receive_buffer.consume(sizeof(m_minor));
	boost::endian::little_to_native_inplace(m_minor);

	std::memcpy(&m_patch, boost::asio::buffer_cast<const void*>(receive_buffer.data()), sizeof(m_patch));
	receive_buffer.consume(sizeof(m_patch));
	boost::endian::little_to_native_inplace(m_patch);
}

void ApiVersion::print()
{
	std::cout << "Protocol Version: " << std::dec << m_major << "." << m_minor << "." << m_patch << std::endl;
}

