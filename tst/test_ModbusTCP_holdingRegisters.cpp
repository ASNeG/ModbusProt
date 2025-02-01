
#include <cpunit>
#include <iostream>
#include <vector>

#include "ModbusTCP/TCPClient.h"
#include "ModbusTCP/TCPServer.h"
#include "ModbusTCP/TCPServerModel.h"
#include "ModbusProt/ModbusModel.h"
#include "ModbusProt/ReadMultipleHoldingRegistersPDU.h"
#include "ModbusProt/WriteSingleHoldingRegisterPDU.h"
#include "ModbusProt/WriteMultipleHoldingRegistersPDU.h"
#include "Condition.h"

namespace TestModbusTCP_HoldingRegisters
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
		auto tcpServerModel = std::make_shared<TCPServerModel>(ctx);
		tcpServerModel->stateCallback(serverConnectionStateCallback);
		tcpServerModel->addModbusModel(modbusModel);
		tcpServerHandler_ = tcpServerModel;
		return tcpServerHandler_;
	}

    CPUNIT_TEST(TestModbusTCP_HoldingRegisters, write_single_holding_register)
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

    	// Client sends write single holding register request to server
    	for (uint16_t address = 1; address <= 100; address++) {
    		responseCondition_.init();

    		// Set single holding register
    		auto writeSingleHoldingRegisterReq = std::make_shared<ModbusProt::WriteSingleHoldingRegisterReqPDU>();
    		writeSingleHoldingRegisterReq->address(address);
    		writeSingleHoldingRegisterReq->registerValue(address);
    		ModbusProt::ModbusPDU::SPtr req = writeSingleHoldingRegisterReq;
    		client.send(0, req, responseCallback);
    		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    		// Receive and check write single holding register response
    		CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    		CPUNIT_ASSERT(res_ != nullptr);
    		CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    		CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::WriteSingleHoldingRegister);

    		// Check contents of write single holding register response
    		auto writeSingleHoldingRegisterRes = std::static_pointer_cast<ModbusProt::WriteSingleHoldingRegisterResPDU>(res_);
    		CPUNIT_ASSERT(writeSingleHoldingRegisterRes->address() == address);
    		CPUNIT_ASSERT(writeSingleHoldingRegisterRes->registerValue() == address);
    	}

    	// Read holding register request (single value)
    	for (uint16_t address = 1; address < 100; address++) {
    		responseCondition_.init();

    		// Create and send read coil request
    		auto readMultipleHoldingRegistersReq = std::make_shared<ModbusProt::ReadMultipleHoldingRegistersReqPDU>();
    		readMultipleHoldingRegistersReq->startingAddress(address);
    		readMultipleHoldingRegistersReq->quantityOfInputs(1);
    		ModbusProt::ModbusPDU::SPtr req = readMultipleHoldingRegistersReq;
    		client.send(0, req, responseCallback);
    		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    		// Receive and check write single coil response
    		CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    		CPUNIT_ASSERT(res_ != nullptr);
    		CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    		CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadMultipleHoldingRegisters);

    		// Check contents of read multiple holding registers response
    		auto readMultipleHoldingRegistersRes = std::static_pointer_cast<ModbusProt::ReadMultipleHoldingRegistersResPDU>(res_);
    		CPUNIT_ASSERT(readMultipleHoldingRegistersRes->byteCount() == 2);
    		uint16_t val;
    		CPUNIT_ASSERT(readMultipleHoldingRegistersRes->getHoldingRegisters(1, &val) == true);
    		CPUNIT_ASSERT(val == address);
    	}

    	// Read holding register request (all bits)
    	responseCondition_.init();

    	// Create and send read multiple holding registers request
    	auto readMultipleHoldingRegistersReq = std::make_shared<ModbusProt::ReadMultipleHoldingRegistersReqPDU>();
    	readMultipleHoldingRegistersReq->startingAddress(1);
    	readMultipleHoldingRegistersReq->quantityOfInputs(100);
    	ModbusProt::ModbusPDU::SPtr req = readMultipleHoldingRegistersReq;
    	client.send(0, req, responseCallback);
    	CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    	// Receive and check write single coil response
    	CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    	CPUNIT_ASSERT(res_ != nullptr);
    	CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    	CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadMultipleHoldingRegisters);

    	// Check contents of read multiple holding registers response
    	auto readMultipleHoldingRegistersRes = std::static_pointer_cast<ModbusProt::ReadMultipleHoldingRegistersResPDU>(res_);
    	CPUNIT_ASSERT(readMultipleHoldingRegistersRes->byteCount() == 200);
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		uint16_t val;
    		CPUNIT_ASSERT(readMultipleHoldingRegistersRes->getHoldingRegisters(idx, val) == true);
    	}

    	uint16_t value[MAX_BYTE_LEN/2];
    	memset((char*)&value, 0x00, MAX_BYTE_LEN/2);
    	CPUNIT_ASSERT(readMultipleHoldingRegistersRes->getHoldingRegisters(99, value) == true);
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		CPUNIT_ASSERT(value[idx] = idx+1);
    	}
    }

    CPUNIT_TEST(TestModbusTCP_HoldingRegisters, write_multiple_holding_registers)
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

		// Create and send write multiple holding registers request
		auto writeMultipleHoldingRegistersReq = std::make_shared<ModbusProt::WriteMultipleHoldingRegistersReqPDU>();
		writeMultipleHoldingRegistersReq->startingAddress(1);
		writeMultipleHoldingRegistersReq->quantityOfRegisters(100);
		for (uint16_t address = 1; address <= 100; address++) {
			writeMultipleHoldingRegistersReq->setRegistersValue(address-1, address);
		}
		ModbusProt::ModbusPDU::SPtr req0 = writeMultipleHoldingRegistersReq;
		client.send(0, req0, responseCallback);
		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

		// Receive and check write single coil response
		CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
		CPUNIT_ASSERT(res_ != nullptr);
		CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
		CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::WriteMultipleHoldingRegisters);

		// Check contents of write single coil response
		auto writeMultipleHoldingRegistersRes = std::static_pointer_cast<ModbusProt::WriteMultipleHoldingRegistersResPDU>(res_);
		CPUNIT_ASSERT(writeMultipleHoldingRegistersRes->startingAddress() == 1);
		CPUNIT_ASSERT(writeMultipleHoldingRegistersRes->quantityOfRegisters() == 100);

    	// Read holding register request (single bit)
    	for (uint16_t address = 1; address < 100; address++) {
    		responseCondition_.init();

    		// Create and send read multiple holding register request
    		auto readMultipleHoldingRegistersReq = std::make_shared<ModbusProt::ReadMultipleHoldingRegistersReqPDU>();
    		readMultipleHoldingRegistersReq->startingAddress(address);
    		readMultipleHoldingRegistersReq->quantityOfInputs(1);
    		ModbusProt::ModbusPDU::SPtr req = readMultipleHoldingRegistersReq;
    		client.send(0, req, responseCallback);
    		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    		// Receive and check response
    		CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    		CPUNIT_ASSERT(res_ != nullptr);
    		CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    		CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadMultipleHoldingRegisters);

    		// Check contents of response
    		auto readMultipleHoldingRegistersRes = std::static_pointer_cast<ModbusProt::ReadMultipleHoldingRegistersResPDU>(res_);
    		CPUNIT_ASSERT(readMultipleHoldingRegistersRes->byteCount() == 2);
    		uint16_t val;
    		CPUNIT_ASSERT(readMultipleHoldingRegistersRes->getHoldingRegisters(0, val) == true);
    		CPUNIT_ASSERT(val == address);
    	}

    	// Read holding registers request (all bits)
    	responseCondition_.init();

    	// Create and send read coil request
    	auto readMultipleHoldingRegistersReq = std::make_shared<ModbusProt::ReadMultipleHoldingRegistersReqPDU>();
    	readMultipleHoldingRegistersReq->startingAddress(1);
    	readMultipleHoldingRegistersReq->quantityOfInputs(100);
    	ModbusProt::ModbusPDU::SPtr req = readMultipleHoldingRegistersReq;
    	client.send(0, req, responseCallback);
    	CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    	// Receive and check write single coil response
    	CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    	CPUNIT_ASSERT(res_ != nullptr);
    	CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    	CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadMultipleHoldingRegisters);

    	// Check contents of write single coil response
    	auto readMultipleHoldingRegistersRes = std::static_pointer_cast<ModbusProt::ReadMultipleHoldingRegistersResPDU>(res_);
    	CPUNIT_ASSERT(readMultipleHoldingRegistersRes->byteCount() == 200);
    	for (uint16_t idx = 0; idx < 100; idx++) {
    		uint16_t val;
    		CPUNIT_ASSERT(readMultipleHoldingRegistersRes->getHoldingRegisters(idx, val) == true);
    	}

    	uint16_t value[MAX_BYTE_LEN/2];
    	memset((char*)&value, 0x00, MAX_BYTE_LEN);
    	CPUNIT_ASSERT(readMultipleHoldingRegistersRes->getHoldingRegisters(99, value) == true);
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		CPUNIT_ASSERT(value[idx] == idx+1);
    	}
    }
}
