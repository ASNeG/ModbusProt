
#include <cpunit>
#include <iostream>
#include <vector>

#include "ModbusTCP/TCPClient.h"
#include "ModbusTCP/TCPServer.h"
#include "ModbusTCP/TCPServerModel.h"
#include "ModbusProt/ReadInputRegistersPDU.h"
#include "Condition.h"

namespace TestModbusTCP_InputRegisters
{
	using namespace cpunit;
	using namespace ModbusTCP;
	using namespace Test;

	const std::string serverIP = "127.0.0.1";
	const std::string serverPort1 = "5502";

	// Handle client response
	Condition responseCondition_;
	ModbusProt::ModbusError modbusError_ = ModbusProt::ModbusError::Ok;
	ModbusProt::ModbusPDU::SPtr req_ = nullptr;
	ModbusProt::ModbusPDU::SPtr res_ = nullptr;
	void responseCallback(ModbusProt::ModbusError modbusError, ModbusProt::ModbusPDU::SPtr& req, ModbusProt::ModbusPDU::SPtr& res)
	{
		std::cout << "RECV RESPONSE: ";
		std::cout << "...." << static_cast<typename std::underlying_type<ModbusProt::ModbusError>::type>(modbusError) << std::endl;

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

	TCPServerHandler::SPtr tcpServerHandler_ = nullptr;
	TCPServerHandler::SPtr acceptCallbackOK(asio::io_context& ctx, asio::ip::tcp::socket& client)
	{
		auto modbusModel = std::make_shared<ModbusProt::ModbusModel>();
		modbusModel->registerMemoryAreaDefaults();
		uint16_t values[100];
		for (uint8_t idx = 0; idx < 100; idx++) {
			values[idx] = idx;
		}
		std::cout << std::endl;
		modbusModel->setValue(ModbusProt::MemoryType::InputRegisters, 0, (uint8_t*)values, 100);
		auto tcpServerModel = std::make_shared<TCPServerModel>(ctx);
		tcpServerModel->stateCallback(serverConnectionStateCallback);
		tcpServerModel->addModbusModel(modbusModel);
		tcpServerHandler_ = tcpServerModel;
		return tcpServerHandler_;
	}

    CPUNIT_TEST(TestModbusTCP_InputRegisters, read_input_registers)
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

    	// Read discrete inputs request (all bits)
    	responseCondition_.init();

    	// Create and send read discrete inputs request
    	auto readInputRegistersReq = std::make_shared<ModbusProt::ReadInputRegistersReqPDU>();
    	readInputRegistersReq->startingAddress(1);
    	readInputRegistersReq->quantityOfInputs(100);
    	ModbusProt::ModbusPDU::SPtr req = readInputRegistersReq;
    	client.send(0, req, responseCallback);
    	CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    	// Receive and check read discrete inputs response
    	CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    	CPUNIT_ASSERT(res_ != nullptr);
    	CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    	CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadInputRegisters);

    	// Check contents of read discrete inputs response
    	auto readInputRegistersRes = std::static_pointer_cast<ModbusProt::ReadInputRegistersResPDU>(res_);
    	CPUNIT_ASSERT(readInputRegistersRes->byteCount() == 200);
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		uint16_t val;
    		CPUNIT_ASSERT(readInputRegistersRes->getInputRegisters(idx, val) == true);
    	}

    	uint16_t value[100];
    	memset((char*)&value, 0x00, 200);
    	CPUNIT_ASSERT(readInputRegistersRes->getInputRegisters(100, value) == true);
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		CPUNIT_ASSERT(value[idx] == idx+1);
    	}
    }

}
