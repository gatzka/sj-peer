#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/asio.hpp>

#include "api_version.h"

boost::asio::io_context io_context;
boost::asio::ip::tcp::socket s(io_context);

boost::asio::streambuf receive_buffer;


void read_version_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (ec) {
		std::cerr << "read version info unsucessful, ec: " << ec << std::endl;
		return;
	}

	ApiVersion apiVersion(receive_buffer);
	apiVersion.print();
}

void read_message_type_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
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
			if (bytes_in_buffer < sizeof(ApiVersion)) {
				std::size_t bytes_to_read = sizeof(ApiVersion) - bytes_in_buffer;
				boost::asio::async_read(s, receive_buffer, boost::asio::transfer_at_least(bytes_to_read), read_version_handler);
			} else {
				ApiVersion apiVersion(receive_buffer);
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
	if (!ec) {
		std::cout << "connect successful!" << std::endl;
		boost::asio::async_read(s, receive_buffer, boost::asio::transfer_at_least(1), read_message_type_handler);
	} else {
		std::cerr << "connect unsuccessful, ec: " << ec << std::endl;
	}
}

void resolve_handler(const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::results_type results)
{
	if (!ec) {
		std::cout << "resolve successful!" << std::endl;
		boost::asio::async_connect(s, results, connect_handler);
	} else {
		std::cerr << "resolve unsuccessful, ec: " << ec << std::endl;
	}
}

int main(void)
{

	boost::asio::ip::tcp::resolver resolver(io_context);
	resolver.async_resolve("localhost", "12345", resolve_handler);

	io_context.run();

	return EXIT_SUCCESS;
}
