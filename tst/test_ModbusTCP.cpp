
#include <cpunit>
#include <iostream>
#include <vector>

#include "ModbusTCP/ModbusTCPBase.h"
#include "ModbusTCP/ModbusTCPClient.h"
#include "ModbusTCP/ModbusTCPServer.h"

#include "Condition.h"

namespace TestModbusTCP
{
	using namespace cpunit;
	using namespace ModbusTCP;
	using namespace Test;

	const std::string serverIP = "127.0.0.1";
	const std::string serverPort = "1234";

	Condition condition_;
	uint32_t numberStates_ = 0;
	std::vector<ModbusTCPClientState> stateVec_;
	void connectionStateCallback(ModbusTCPClientState clientState) {
		std::cout << "STATE: ";
		std::cout << "...." << static_cast<typename std::underlying_type<ModbusTCPClientState>::type>(clientState) << std::endl;

		if (numberStates_ == 0) return;
		stateVec_.push_back(clientState);
		numberStates_--;
		if (numberStates_== 0) {
			std::cout << "SEND SIGNAL..." << std::endl;
			condition_.signal();
		}

	}

    CPUNIT_TEST(TestModbusTCP, getEndpoint)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create tcp base object
    	ModbusTCPBase tcpBase;
    	CPUNIT_ASSERT(tcpBase.getEndpoint(serverIP, serverPort, serverEndpoint) == true);
	}

    CPUNIT_TEST(TestModbusTCP, client_not_con_not_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	ModbusTCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort, serverEndpoint) == true);

    	// Client connect to not existing server
    	stateVec_.clear();
    	numberStates_ = 3;
    	condition_.init();
    	client.connect(serverEndpoint, connectionStateCallback, 0);
    	CPUNIT_ASSERT(condition_.wait(1000) == true);
    	CPUNIT_ASSERT(stateVec_[0] == ModbusTCPClientState::Connecting);
    	CPUNIT_ASSERT(stateVec_[1] == ModbusTCPClientState::Close);
    	CPUNIT_ASSERT(stateVec_[2] == ModbusTCPClientState::Error);
    }

    CPUNIT_TEST(TestModbusTCP, client_not_con_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	ModbusTCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort, serverEndpoint) == true);

    	// Client connect to not existing server
    	stateVec_.clear();
    	numberStates_ = 6;
    	condition_.init();
    	client.connect(serverEndpoint, connectionStateCallback, 200);
    	CPUNIT_ASSERT(condition_.wait(3000) == true);
    	client.disconnect();

    	CPUNIT_ASSERT(stateVec_[0] == ModbusTCPClientState::Connecting);
    	CPUNIT_ASSERT(stateVec_[1] == ModbusTCPClientState::Close);
    	CPUNIT_ASSERT(stateVec_[2] == ModbusTCPClientState::Connecting);
    	CPUNIT_ASSERT(stateVec_[3] == ModbusTCPClientState::Close);
    	CPUNIT_ASSERT(stateVec_[4] == ModbusTCPClientState::Connecting);
    	CPUNIT_ASSERT(stateVec_[5] == ModbusTCPClientState::Close);
    }

    CPUNIT_TEST(TestModbusTCP, server_open_close)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create server object
    	ModbusTCPServer server;
    	CPUNIT_ASSERT(server.getEndpoint(serverIP, serverPort, serverEndpoint) == true);

    	// Open server acceptor
    	CPUNIT_ASSERT(server.open(serverEndpoint) == true);

    	// Close server acceptor
    	server.close();
    }

}
