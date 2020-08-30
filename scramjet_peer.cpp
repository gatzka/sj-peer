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
#include <boost/beast.hpp>
#include <boost/endian/conversion.hpp>

#include "protocol_version.h"

static const std::string default_tcp_port = "12345";
static const std::string default_local_endpoint = "unix_domain_socket_file";
static const std::string default_websocket_port = "80";
static const std::string default_websocket_path = "/";

boost::asio::io_context io_context;

/// We have to create it later.
std::unique_ptr<boost::asio::generic::stream_protocol::socket> generic_stream_socket;

boost::asio::streambuf receive_buffer;

void handle_message(boost::asio::streambuf& receive_buffer)
{
	uint8_t message_type;
	std::memcpy(&message_type, boost::asio::buffer_cast<const void*>(receive_buffer.data()), sizeof(message_type));
	receive_buffer.consume(sizeof(message_type));
	std::cout << "message_type: " << std::hex << (uint32_t)message_type << std::endl;

	switch (message_type) {
	/// \todo will there be an interface header from scramjet containing constants regarding the protocol
	case 1: {
		protocol_version protocol_version(receive_buffer);
		protocol_version.print();
	} break;
	default:
		// since we don't know how much to read to get behind the unknown type, we are lost here and should disconnect!
		std::cerr << "unknown message type!" << std::endl;
		break;
	}
}

void read_message(const boost::system::error_code& ec)
{
	if (ec) {
		std::cerr << "message read unsuccessful, ec: " << ec.message() << std::endl;
		return;
	}

	handle_message(receive_buffer);
}

void read_message_length(const boost::system::error_code& ec)
{
	if (ec) {
		std::cerr << "message length read unsuccessful, ec: " << ec.message() << std::endl;
		return;
	}

	uint32_t message_length;
	std::memcpy(&message_length, boost::asio::buffer_cast<const void*>(receive_buffer.data()), sizeof(message_length));
	receive_buffer.consume(sizeof(message_length));
	boost::endian::little_to_native_inplace(message_length);
	std::cout << "message_length: " << std::dec << message_length << std::endl;

	std::size_t bytes_in_buffer = receive_buffer.size();
	if (bytes_in_buffer < (size_t)message_length) {
		std::size_t bytes_to_read = sizeof(protocol_version) - bytes_in_buffer;
		boost::asio::async_read(*generic_stream_socket.get(), receive_buffer, boost::asio::transfer_at_least(bytes_to_read), std::bind(read_message, std::placeholders::_1));
	} else {
		handle_message(receive_buffer);
	}
}

int main(void)
{
	/// relevant for tcp hand websocket
	static const std::string host = "localhost";

	// tcp specific handling happens within the following lambdas
	boost::asio::ip::tcp::resolver tcp_resolver(io_context);

	boost::asio::ip::tcp::socket tcp_socket(io_context);
	auto tcp_resolve_handler = [&tcp_socket](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results) {
		if (ec) {
			std::cerr << "tcp resolve unsuccessful, ec: " << ec << std::endl;
			return;
		}
		std::cout << "tcp resolve successful!" << std::endl;

		auto tcp_connect_handler = [&tcp_socket](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint) {
			if (ec) {
				std::cerr << "tcp connect unsuccessful, ec: " << ec.message() << std::endl;
				return;
			}
			std::cout << "tcp connect successful!" << std::endl;

			generic_stream_socket = std::unique_ptr<boost::asio::generic::stream_protocol::socket>(new boost::asio::generic::stream_protocol::socket(std::move(tcp_socket)));
			boost::asio::async_read(*generic_stream_socket.get(), receive_buffer, boost::asio::transfer_at_least(4), std::bind(read_message_length, std::placeholders::_1));
		};
		boost::asio::async_connect(tcp_socket, results, tcp_connect_handler);
	};
	tcp_resolver.async_resolve(host, default_tcp_port, tcp_resolve_handler);

	// unix domain socket specific handling happens within the following lambdas
	boost::asio::local::stream_protocol::socket local_socket(io_context);
	boost::asio::local::stream_protocol::endpoint ep(default_local_endpoint);

	auto local_connect_handler = [&local_socket](const boost::system::error_code& ec) {
		if (ec) {
			std::cerr << "local connect unsuccessful, ec: " << ec.message() << std::endl;
			return;
		}
		std::cout << "local connect successful!" << std::endl;

		generic_stream_socket = std::unique_ptr<boost::asio::generic::stream_protocol::socket>(new boost::asio::generic::stream_protocol::socket(std::move(local_socket)));
		boost::asio::async_read(*generic_stream_socket.get(), receive_buffer, boost::asio::transfer_at_least(4), std::bind(read_message_length, std::placeholders::_1));
	};
	local_socket.async_connect(ep, local_connect_handler);

	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket(io_context);

	auto websocket_resolve_handler = [&websocket](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results) {
		if (ec) {
			std::cerr << "websocket resolve unsuccessful, ec: " << ec << std::endl;
			return;
		}
		std::cout << "websocket resolve successful!" << std::endl;

		auto websocket_connect_handler = [&websocket](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint) {
			if (ec) {
				std::cerr << "websocket connect unsuccessful, ec: " << ec.message() << std::endl;
				return;
			}
			std::cout << "websocket connect successful!" << std::endl;

			auto websocket_handshake_handler = [&websocket](const boost::system::error_code& ec) {
				if (ec) {
					std::cerr << "websocket handshake unsuccessful, ec: " << ec << std::endl;
					return;
				}
				generic_stream_socket = std::unique_ptr<boost::asio::generic::stream_protocol::socket>(new boost::asio::generic::stream_protocol::socket(std::move(websocket.next_layer())));
				boost::asio::async_read(*generic_stream_socket.get(), receive_buffer, boost::asio::transfer_at_least(4), std::bind(read_message_length, std::placeholders::_1));
			};
			websocket.async_handshake(host, default_websocket_path, websocket_handshake_handler);
		};
		boost::asio::async_connect(websocket.next_layer(), results, websocket_connect_handler);
	};
	tcp_resolver.async_resolve(host, default_websocket_port, websocket_resolve_handler);

	io_context.run();

	return EXIT_SUCCESS;
}
