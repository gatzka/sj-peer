#include <scramjet/jet_peer.hpp>
#include <scramjet/socket_jet_connection.hpp>

int main()
{
    scramjet::socket_jet_connection connection;
    scramjet::jet_peer peer(connection);
	return 0;
}
