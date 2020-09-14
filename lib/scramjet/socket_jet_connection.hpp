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

#ifndef SCRAMJET__SOCKET_JET_CONNECTION_HPP
#define SCRAMJET__SOCKET_JET_CONNECTION_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>

#include "scramjet/jet_connection.hpp"

namespace scramjet {

class socket_jet_connection final : public jet_connection {
public:
	virtual void connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept override;
	virtual void disconnect(void) noexcept override;
	virtual void receive_message(const message_received_callback_t callback) noexcept override;

	socket_jet_connection(boost::asio::io_context& ioc, const std::string& host, uint16_t port = DEFAULT_SOCKET_JET_PORT) noexcept;
	virtual ~socket_jet_connection() noexcept;

private:
	const std::string host;
	uint16_t port;
	boost::asio::ip::tcp::resolver tcp_resolver;
	boost::asio::ip::tcp::socket tcp_socket;
	std::unique_ptr<boost::asio::generic::stream_protocol::socket> generic_stream_socket;
	boost::asio::streambuf receive_buffer;
	boost::asio::high_resolution_timer deadline;

	static const uint16_t DEFAULT_SOCKET_JET_PORT = UINT16_C(12345);

	void resolve_handler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results) noexcept;
	void resolve_timeout_handler(const boost::system::error_code& ec) noexcept;
	void connect_handler(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& ep) noexcept;
	void connect_timeout_handler(const boost::system::error_code& ec) noexcept;

	void message_length_read(const boost::system::error_code& ec) noexcept;
	void message_read(const boost::system::error_code& ec) noexcept;

	void handle_message(void);

	uint32_t message_length;
};
} // namespace scramjet

#endif
