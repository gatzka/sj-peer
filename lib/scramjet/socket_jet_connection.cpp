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

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <iostream> //TODO

#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>

#include "scramjet/jet_connection.hpp"
#include "scramjet/socket_jet_connection.hpp"

namespace scramjet {
socket_jet_connection::socket_jet_connection(boost::asio::io_context& ioc, const std::string& h, uint16_t p) noexcept
        : m_host(h)
        , m_port(p)
        , m_tcp_resolver(ioc)
        , m_tcp_socket(ioc)
        , m_deadline(ioc)
{
}

socket_jet_connection::~socket_jet_connection() noexcept
{
}

void socket_jet_connection::connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept
{
	using namespace std::placeholders;
	m_connected_callback = connect_callback;
	m_connect_timeout = timeout;

	m_tcp_resolver.async_resolve(m_host, std::to_string(m_port),
	                             std::bind(&socket_jet_connection::resolve_handler,
	                                       this,
	                                       _1,
	                                       _2));
	m_deadline.expires_from_now(timeout);
	m_deadline.async_wait(std::bind(&socket_jet_connection::resolve_timeout_handler, this, _1));
}

void socket_jet_connection::disconnect(void) noexcept
{
	m_generic_stream_socket.get()->cancel();
	m_generic_stream_socket.get()->close();
}

void socket_jet_connection::resolve_handler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results) noexcept
{
	m_deadline.cancel();
	if (ec) {
		if (ec == boost::asio::error::operation_aborted) {
			m_connected_callback(SCRAMJET_OPERATION_ABORTED);
			return;
		}

		m_connected_callback(SCRAMJET_HOST_NOT_FOUND);
		return;
	}

	using namespace std::placeholders;
	boost::asio::async_connect(m_tcp_socket, results, std::bind(&socket_jet_connection::connect_handler, this, _1, _2));
	m_deadline.expires_from_now(m_connect_timeout);
	m_deadline.async_wait(std::bind(&socket_jet_connection::connect_timeout_handler, this, _1));
}

void socket_jet_connection::resolve_timeout_handler(const boost::system::error_code& ec) noexcept
{
	if (ec && (ec == boost::asio::error::operation_aborted)) {
		return;
	}

	m_tcp_resolver.cancel();
}

void socket_jet_connection::connect_handler(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& ep) noexcept
{
	(void)ep;

	m_deadline.cancel();
	if (ec) {
		if (ec == boost::asio::error::operation_aborted) {
			m_connected_callback(SCRAMJET_OPERATION_ABORTED);
			return;
		}

		m_connected_callback(SCRAMJET_CONNECTION_REFUSED);
		return;
	}

	m_generic_stream_socket = std::unique_ptr<boost::asio::generic::stream_protocol::socket>(
	    new boost::asio::generic::stream_protocol::socket(
	        std::move(m_tcp_socket)));
	m_connected_callback(SCRAMJET_OK);
}

void socket_jet_connection::connect_timeout_handler(const boost::system::error_code& ec) noexcept
{
	if (ec && (ec == boost::asio::error::operation_aborted)) {
		return;
	}

	m_tcp_socket.cancel();
	m_tcp_socket.close();
}

void socket_jet_connection::receive_message(const message_received_callback_t callback) noexcept
{
	m_message_received_callback = callback;

	boost::asio::async_read(*m_generic_stream_socket.get(),
	                        m_receive_buffer,
	                        boost::asio::transfer_at_least(4),
	                        std::bind(&socket_jet_connection::message_length_read,
	                                  this,
	                                  std::placeholders::_1));
}

void socket_jet_connection::message_length_read(const boost::system::error_code& ec) noexcept
{
	if (ec) {
		m_connected_callback(SCRAMJET_WRONG_MESSAGE_FORMAT);
		return;
	}

	std::memcpy(&m_message_length, boost::asio::buffer_cast<const void*>(m_receive_buffer.data()), sizeof(m_message_length));
	m_receive_buffer.consume(sizeof(m_message_length));
	boost::endian::little_to_native_inplace(m_message_length);

	std::size_t bytes_in_buffer = m_receive_buffer.size();
	if (bytes_in_buffer < (size_t)m_message_length) {
		std::size_t bytes_to_read = m_message_length - bytes_in_buffer;
		boost::asio::async_read(*m_generic_stream_socket.get(),
		                        m_receive_buffer,
		                        boost::asio::transfer_at_least(bytes_to_read),
		                        std::bind(&socket_jet_connection::message_read,
		                                  this,
		                                  std::placeholders::_1));
	} else {
		handle_message();
	}
}

void socket_jet_connection::message_read(const boost::system::error_code& ec) noexcept
{
	if (ec) {
		disconnect();
		return;
	}

	handle_message();
}

void socket_jet_connection::handle_message(void)
{
	m_message_received_callback(SCRAMJET_OK, boost::asio::buffer_cast<const uint8_t*>(m_receive_buffer.data()), (size_t)m_message_length);
	m_receive_buffer.consume(m_message_length);
}

} // namespace scramjet
