
#include <cpunit>
#include <iostream>
#include <vector>

#include "ModbusTCP/TCPClient.h"
#include "ModbusTCP/TCPServer.h"
#include "ModbusProt/ReadCoilsPDU.h"
#include "Condition.h"

namespace TestModbusTCP
{
	using namespace cpunit;
	using namespace ModbusTCP;
	using namespace Test;

	const std::string serverIP = "127.0.0.1";
	const std::string serverPort1 = "1234";


	// Handle client response
	Condition responseCondition_;
	ModbusProt::ModbusError modbusError_ = ModbusProt::ModbusError::Ok;
	ModbusProt::ModbusPDU::SPtr req_ = nullptr;
	ModbusProt::ModbusPDU::SPtr res_ = nullptr;
	void responseCallback(ModbusProt::ModbusError modbusError, ModbusProt::ModbusPDU::SPtr& req, ModbusProt::ModbusPDU::SPtr& res)
	{
		std::cout << "RECV RESPONSE: ";
		std::cout << "...." << static_cast<typename std::underlying_type<TCPClientState>::type>(modbusError) << std::endl;

		modbusError_ = modbusError;
		req_ = req;
		res_ = res;
		responseCondition_.signal();
	}

	// Handle client connection state
	Condition clientCondition_;
	uint32_t clientNumberStates_ = 0;
	std::vector<TCPClientState> clientStateVec_;
	void clientConnectionStateCallback(TCPClientState clientState) {
		std::cout << "CLIENT STATE: ";
		std::cout << "...." << static_cast<typename std::underlying_type<TCPClientState>::type>(clientState) << std::endl;

		if (clientNumberStates_ == 0) return;
		clientStateVec_.push_back(clientState);
		clientNumberStates_--;
		if (clientNumberStates_== 0) {
			std::cout << "CLIENT SEND SIGNAL..." << std::endl;
			clientCondition_.signal();
		}
	}

	// Handle server connection state
	Condition serverCondition_;
	uint32_t serverNumberStates_ = 0;
	std::vector<TCPServerState> serverStateVec_;
	void serverConnectionStateCallback(TCPServerState serverState) {
		std::cout << "SERVER STATE: ";
		std::cout << "...." << static_cast<typename std::underlying_type<TCPServerState>::type>(serverState) << std::endl;

		if (serverNumberStates_ == 0) return;
		serverStateVec_.push_back(serverState);
		serverNumberStates_--;
		if (serverNumberStates_== 0) {
			std::cout << "SERVER SEND SIGNAL..." << std::endl;
			serverCondition_.signal();
		}
	}

	TCPServerHandler::SPtr acceptCallbackError(asio::io_context& ctx, asio::ip::tcp::socket& client)
	{
		client.close();
		return nullptr;
	}

	TCPServerHandler::SPtr tcpServerHandler_ = nullptr;
	TCPServerHandler::SPtr acceptCallbackOK(asio::io_context& ctx, asio::ip::tcp::socket& client)
	{
		tcpServerHandler_ = std::make_shared<TCPServerHandler>(ctx);
		tcpServerHandler_->stateCallback(serverConnectionStateCallback);
		return tcpServerHandler_;
	}

    CPUNIT_TEST(TestModbusTCP, getEndpoint)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create tcp base object
    	TCPBase tcpBase;
    	CPUNIT_ASSERT(tcpBase.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);
	}

    CPUNIT_TEST(TestModbusTCP, client_not_con_not_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	TCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Client connect to not existing server
    	clientStateVec_.clear();
    	clientNumberStates_ = 2;
    	clientCondition_.init();
    	client.connect(serverEndpoint, clientConnectionStateCallback);
    	CPUNIT_ASSERT(clientCondition_.wait(1000) == true);
    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::Close);
    }

    CPUNIT_TEST(TestModbusTCP, client_not_con_retry)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create client object
    	TCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Client connect to not existing server
    	clientStateVec_.clear();
    	clientNumberStates_ = 6;
    	clientCondition_.init();
    	client.reconnectTimeout(200);
    	client.connect(serverEndpoint, clientConnectionStateCallback);
    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);

    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::WaitForReconnect);
    	CPUNIT_ASSERT(clientStateVec_[2] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[3] == TCPClientState::WaitForReconnect);
    	CPUNIT_ASSERT(clientStateVec_[4] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[5] == TCPClientState::WaitForReconnect);
    	clientStateVec_.clear();

    	clientNumberStates_ = 2;
    	clientCondition_.init();
    	client.disconnect();
    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Close);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::Down);
    }

    CPUNIT_TEST(TestModbusTCP, server_open_close)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create server object
    	TCPServer server;
    	CPUNIT_ASSERT(server.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Open server acceptor
    	CPUNIT_ASSERT(server.open(serverEndpoint, acceptCallbackError) == true);

    	sleep(1);

    	// Close server acceptor
    	server.close();
    }

    CPUNIT_TEST(TestModbusTCP, client_connect_server_error)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create server object
    	TCPServer server;
    	CPUNIT_ASSERT(server.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Open server acceptor
    	CPUNIT_ASSERT(server.open(serverEndpoint, acceptCallbackError) == true);

    	// Create client object
    	TCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Client connect to server - server don't accept connection
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 3;
    	client.connect(serverEndpoint, clientConnectionStateCallback);
    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);

    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::Connected);
    	CPUNIT_ASSERT(clientStateVec_[2] == TCPClientState::Down);

    	// Close server acceptor
    	server.close();
    }

    CPUNIT_TEST(TestModbusTCP, handler_disconnect)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create server object
    	TCPServer server;
    	CPUNIT_ASSERT(server.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Open server acceptor
    	CPUNIT_ASSERT(server.open(serverEndpoint, acceptCallbackOK) == true);

    	// Create client object
    	TCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Client connect to server
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 2;
    	client.connect(serverEndpoint, clientConnectionStateCallback);
    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);

    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::Connected);

    	// close handler
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 1;

    	serverCondition_.init();
    	serverStateVec_.clear();
    	serverNumberStates_ = 1;

    	tcpServerHandler_->disconnect();
    	CPUNIT_ASSERT(serverCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(serverStateVec_[0] == TCPServerState::Down);

    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Down);
    }

    CPUNIT_TEST(TestModbusTCP, client_disconnect)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create server object
    	TCPServer server;
    	CPUNIT_ASSERT(server.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Open server acceptor
    	CPUNIT_ASSERT(server.open(serverEndpoint, acceptCallbackOK) == true);

    	// Create client object
    	TCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Client connect to server
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 2;
    	client.connect(serverEndpoint, clientConnectionStateCallback);
    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);

    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::Connected);

    	// close handler
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 1;

    	serverCondition_.init();
    	serverStateVec_.clear();
    	serverNumberStates_ = 1;

    	client.disconnect();
    	CPUNIT_ASSERT(serverCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(serverStateVec_[0] == TCPServerState::Down);

    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Down);
    }

    CPUNIT_TEST(TestModbusTCP, send_recv)
	{
    	asio::ip::tcp::endpoint serverEndpoint;

    	// Create server object
    	TCPServer server;
    	CPUNIT_ASSERT(server.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Open server acceptor
    	CPUNIT_ASSERT(server.open(serverEndpoint, acceptCallbackOK) == true);

    	// Create client object
    	TCPClient client;
    	CPUNIT_ASSERT(client.getEndpoint(serverIP, serverPort1, serverEndpoint) == true);

    	// Client connect to server
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 2;
    	client.connect(serverEndpoint, clientConnectionStateCallback);
    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);

    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Connecting);
    	CPUNIT_ASSERT(clientStateVec_[1] == TCPClientState::Connected);

    	// Client sends read coil request to server and receives error response
    	responseCondition_.init();
    	ModbusProt::ReadCoilsReqPDU::SPtr readCoilsReq = std::make_shared<ModbusProt::ReadCoilsReqPDU>();
    	readCoilsReq->startingAddress(0x10);
    	readCoilsReq->quantityOfInputs(0x10);
    	ModbusProt::ModbusPDU::SPtr req = readCoilsReq;
		client.send(0x12, req, responseCallback);

		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    	// close handler
    	clientCondition_.init();
    	clientStateVec_.clear();
    	clientNumberStates_ = 1;

    	serverCondition_.init();
    	serverStateVec_.clear();
    	serverNumberStates_ = 1;

    	client.disconnect();
    	CPUNIT_ASSERT(serverCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(serverStateVec_[0] == TCPServerState::Down);

    	CPUNIT_ASSERT(clientCondition_.wait(3000) == true);
    	CPUNIT_ASSERT(clientStateVec_[0] == TCPClientState::Down);
    }

}
