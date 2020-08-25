//#include <cstdint>
//#include <cstdlib>
//#include <cstring>
//#include <iostream>

//#include <boost/asio.hpp>
//#include <boost/endian/conversion.hpp>

class api_version {
public:
	api_version(boost::asio::streambuf &receive_buffer);
	void print() const;

private:
	uint32_t m_major;
	uint32_t m_minor;
	uint32_t m_patch;
};

