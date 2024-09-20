
#include <cpunit>
#include <iostream>

#include "ModbusTCP/ModbusTCPClient.h"
#include "ModbusTCP/ModbusTCPServer.h"

namespace TestModbusTCP
{
	using namespace cpunit;
	using namespace ModbusTCP;

	void stateCallback1(ModbusTCPClientState state)
	{
		;
	}

	const std::string serverIP = "127.0.0.1";
	const std::string serverPort = "1234";

	void connectionStateCallback(ModbusTCPClientState clientState) {
		std::cout << "STATE: ";
		std::cout << "...." << static_cast<typename std::underlying_type<ModbusTCPClientState>::type>(clientState) << std::endl;
	}

    CPUNIT_TEST(TestModbusTCP, client_not_con_no_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	ModbusTCPClient client;
    	assert_equals("getEndpoint fails", client.getEndpoint(serverIP, serverPort, serverEndpoint), true);

    	// Client connect to not existing server
    	client.connect(serverEndpoint, connectionStateCallback, 0);
    	sleep(10);

    }

}
