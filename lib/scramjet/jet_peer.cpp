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
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>

#include "scramjet/jet_connection.hpp"
#include "scramjet/jet_peer.hpp"
#include "scramjet/message_type.hpp"
#include "scramjet/protocol_version.hpp"


namespace scramjet {

const static protocol_version supported_version(1, 0, 0);

uint16_t jet_peer::s_nextRequestId = 0;

jet_peer::jet_peer(std::unique_ptr<jet_connection> c) noexcept
	: m_connection(std::move(c))
	, m_connected_callback(nullptr)
{
}

jet_peer::~jet_peer() noexcept
{
}

void jet_peer::connect(const connected_callback_t& connect_callback, std::chrono::milliseconds timeout) noexcept
{
	m_connected_callback = connect_callback;

	using namespace std::placeholders;
	m_connection->connect(std::bind(&jet_peer::connected, this, _1), timeout);
}

void jet_peer::connected(scramjet::error_code ec)
{
	if (m_connected_callback != nullptr) {
		m_connected_callback(ec);
	}

	if (ec != scramjet::error_code::SCRAMJET_OK) {
		std::cerr << "Connection not established!" << std::endl;
		return;
	}

	using namespace std::placeholders;
	m_connection->receive_message(std::bind(&jet_peer::version_received, this, _1, _2, _3));
}

void jet_peer::disconnect(void) noexcept
{
    m_connection->disconnect();
}

void jet_peer::addState(jet_peer& peer, const std::string& key, data value, response_callback_code callback)
{
	peer.sendRequest(scramjet::FUNCTION_ADD_STATE, key, value, callback);
}

static bool is_correct_protocol_version(const uint8_t* buffer, size_t buffer_length)
{
	uint8_t message_type;
	if (buffer_length != sizeof(message_type) + protocol_version::get_version_size()) {
        return false;
    }

    message_type = *buffer;
    buffer++;
    if (message_type != scramjet::message_type::MESSAGE_API_VERSION) {
        return false;
    }
    protocol_version v(buffer);
    v.print();
    return v.is_compatible(supported_version);
}

void jet_peer::version_received(enum error_code ec, const uint8_t* message, size_t message_length)
{
	if (ec != scramjet::error_code::SCRAMJET_OK) {
			disconnect();
			return;
	}

	std::cout << "Got message of length: " << message_length << std::endl;

	if (!is_correct_protocol_version(message, message_length)) {
			std::cerr << "protocol API version not supported!" << std::endl;
			return;
	}
	
	using namespace std::placeholders;
	m_connection->receive_message(std::bind(&jet_peer::message_received, this, _1, _2, _3));
}

void jet_peer::message_received(enum error_code ec, const uint8_t* message, size_t message_length)
{
    if (ec != scramjet::error_code::SCRAMJET_OK) {
        disconnect();
        return;
    }

	(void)message;
		std::cout << "Got message of length: " << message_length << std::endl;
}


/// @param callback What do we expect to get from a request? Should it be just a result code? Are there reuests that return data?
void jet_peer::sendRequest(function_type function, const std::string& key, data value, response_callback_code callback)
{
	/// a request consists of
	/// -message length (uint32_t
	/// -message type (request) (uint8_t)
	/// -request id (created inside)
	/// -jet function
	/// -jet function payload
	message_type messageType = MESSAGE_REQUEST;
	uint16_t requestId = s_nextRequestId;
	int32_t jetFunction = 
	++s_nextRequestId;
	
	

}
} // namespace scramjet
