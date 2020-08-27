/*
 * SPDX-License-Identifier: MIT
 *
 * The MIT License (MIT)
 *
 * Copyright (c) <2020> Matthias Loy, Stephan Gatzka
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

#include <boost/asio.hpp>

#include "protocol_version.h"

boost::asio::io_context io_context;


boost::asio::ip::tcp::socket s(io_context);

boost::asio::streambuf receive_buffer;

void read_version_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (ec) {
		std::cerr << "read version info unsucessful, ec: " << ec << std::endl;
		return;
	}

	protocol_version apiVersion(receive_buffer);
	apiVersion.print();
}

void read_message_type_handler(boost::asio::ip::tcp::socket& socket, const boost::system::error_code& ec)
{
	if (ec) {
		std::cerr << "message type read unsuccessful, ec: " << ec << std::endl;
		return;
	}
	uint8_t message_type;
	std::memcpy(&message_type, boost::asio::buffer_cast<const void*>(receive_buffer.data()), sizeof(message_type));
	receive_buffer.consume(sizeof(message_type));
	std::cout << "message_type: " << std::hex << (uint32_t)message_type << std::endl;

	switch (message_type) {
	/// \todo will there be an interface header from scramjet containing constants regarding the protocol
	case 1:
		{
			std::size_t bytes_in_buffer = receive_buffer.size();
			if (bytes_in_buffer < sizeof(protocol_version)) {
				std::size_t bytes_to_read = sizeof(protocol_version) - bytes_in_buffer;
				boost::asio::async_read(socket, receive_buffer, boost::asio::transfer_at_least(bytes_to_read), read_version_handler);
			} else {
				protocol_version apiVersion(receive_buffer);
				apiVersion.print();
			}
		}
		break;
	default:
		// since we don't know how much to read to get behind the unknown type, we are lost here and should disconnect!
		std::cerr << "unknown message type!" << std::endl;
		break;
	}
}

void connect_handler(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint)
{
	if (ec) {
		std::cerr << "connect unsuccessful, ec: " << ec << std::endl;
		return;
	}
	std::cout << "connect successful!" << std::endl;
	
	//boost::asio::generic::stream_protocol::socket generic_stream_socket(std::move(s));
	boost::asio::async_read(s, receive_buffer, boost::asio::transfer_at_least(1), std::bind(read_message_type_handler, std::ref(s), std::placeholders::_1));

}

void resolve_handler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results)
{
	if (ec) {
		std::cerr << "resolve unsuccessful, ec: " << ec << std::endl;
		return;
	}
	std::cout << "resolve successful!" << std::endl;
	boost::asio::async_connect(s, results, connect_handler);
}

int main(void)
{

	boost::asio::ip::tcp::resolver resolver(io_context);
	resolver.async_resolve("localhost", "12345", resolve_handler);

	io_context.run();

	return EXIT_SUCCESS;
}
