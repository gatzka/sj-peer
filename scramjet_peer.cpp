#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <endian.h>

#include <boost/asio.hpp>

boost::asio::io_context io_context;
boost::asio::ip::tcp::socket s(io_context);

boost::asio::streambuf receive_buffer;
std::istream is(&receive_buffer);

static void print_version(void)
{
	uint32_t version_major;
	is >> version_major;
	version_major = le32toh(version_major);

	uint32_t version_minor;
	is >> version_minor;
	version_minor = le32toh(version_minor);

	uint32_t version_patch;
	is >> version_patch;
	version_patch = le32toh(version_patch);

	std::cout << "Protocol Version: " << std::dec << version_major << "." << version_minor << "." << version_patch << std::endl;

	std::cout << "size of streambuf after reading protocol version: " << std::dec <<receive_buffer.size() << std::endl;
}

void read_version_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (!ec) {
		print_version();
	} else {
		std::cerr << "read version info unsucessful, ec: " << ec << std::endl;
	}
}

void read_message_type_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	if (!ec) {
		uint8_t message_type;
		is >> message_type;
		std::cout << "message_type: " << std::hex << (uint32_t)message_type << std::endl;
		std::cout << "size of streambuf after getting mt: " << std::dec <<receive_buffer.size() << std::endl;

		std::size_t bytes_in_buffer = receive_buffer.size();
		if (bytes_in_buffer < 12) {
			std::size_t bytes_to_read = 12 - bytes_in_buffer;
			boost::asio::async_read(s, receive_buffer, boost::asio::transfer_at_least(bytes_to_read), read_version_handler);
		} else {
			std::cout << "enough data" << std::endl;
			print_version();
		}
	} else {
		std::cerr << "message type read unsuccessful, ec: " << ec << std::endl;
	}
}

void connect_handler(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint)
{
	if (!ec) {
		std::cout << "connect  successful!" << std::endl;
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
