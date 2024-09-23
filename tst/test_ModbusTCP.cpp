
#include <cpunit>
#include <iostream>
#include <vector>

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
			condition_.signal();
		}

	}

#if 0
    CPUNIT_TEST(TestModbusTCP, client_not_con_not_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	ModbusTCPClient client;
    	assert_equals(client.getEndpoint(serverIP, serverPort, serverEndpoint), true);

    	// Client connect to not existing server
    	stateVec_.clear();
    	numberStates_ = 3;
    	condition_.init();
    	client.connect(serverEndpoint, connectionStateCallback, 0);
    	assert_equals(condition_.wait(1000), true);
    	assert_equals(stateVec_[0] == ModbusTCPClientState::Connecting, true);
    	assert_equals(stateVec_[1] == ModbusTCPClientState::Close, true);
    	assert_equals(stateVec_[2] == ModbusTCPClientState::Error, true);
    }
#endif

    CPUNIT_TEST(TestModbusTCP, client_not_con_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	ModbusTCPClient client;
    	assert_equals(client.getEndpoint(serverIP, serverPort, serverEndpoint), true);

    	// Client connect to not existing server
    	stateVec_.clear();
    	numberStates_ = 4;
    	condition_.init();
    	std::cout << "AAAAAA1" << std::endl;
    	client.connect(serverEndpoint, connectionStateCallback, 5000);
    	std::cout << "AAAAAA2" << std::endl;
    	assert_equals(condition_.wait(4000), false);
    	std::cout << "AAAAAA3" << std::endl;
    	client.disconnect();

    	assert_equals(false, true);
    	assert_equals(stateVec_[0] == ModbusTCPClientState::Connecting, true);
    	assert_equals(stateVec_[1] == ModbusTCPClientState::Close, true);
    	assert_equals(stateVec_[2] == ModbusTCPClientState::Connecting,  true);
    	assert_equals(stateVec_[3] == ModbusTCPClientState::Close, true);

    }

}
