
#include <cpunit>
#include <iostream>
#include <vector>

#include "ModbusTCP/TCPClient.h"
#include "ModbusTCP/TCPServer.h"
#include "ModbusTCP/TCPServerModel.h"
#include "ModbusProt/ReadCoilsPDU.h"
#include "ModbusProt/WriteSingleCoilPDU.h"
#include "Condition.h"

namespace TestModbusTCP_Coil
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

    CPUNIT_TEST(TestModbusTCP_Coil, write_single_coil)
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

    	// Client sends write single coil request to server
    	for (uint16_t address = 1; address < 100; address++) {
    		responseCondition_.init();

    		// Create and send write single coil request
    		auto writeSingleCoilReq = std::make_shared<ModbusProt::WriteSingleCoilReqPDU>();
    		writeSingleCoilReq->address(address);
    		writeSingleCoilReq->value(address % 3 == 0 ? true : false);
    		ModbusProt::ModbusPDU::SPtr req = writeSingleCoilReq;
    		client.send(0, req, responseCallback);
    		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    		// Receive and check write single coil response
    		CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    		CPUNIT_ASSERT(res_ != nullptr);
    		CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    		CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::WriteSingleCoil);

    		// Check contents of write single coil response
    		auto writeSingleCoilRes = std::static_pointer_cast<ModbusProt::WriteSingleCoilResPDU>(res_);
    		CPUNIT_ASSERT(writeSingleCoilRes->address() == address);
    		CPUNIT_ASSERT(writeSingleCoilRes->value() == (address % 3 == 0 ? true : false));
    	}

    	// Read coil request (single bit)
    	for (uint16_t address = 1; address < 100; address++) {
    		responseCondition_.init();

    		// Create and send read coil request
    		auto readCoilsReq = std::make_shared<ModbusProt::ReadCoilsReqPDU>();
    		readCoilsReq->startingAddress(address);
    		readCoilsReq->quantityOfInputs(1);
    		ModbusProt::ModbusPDU::SPtr req = readCoilsReq;
    		client.send(0, req, responseCallback);
    		CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    		// Receive and check write single coil response
    		CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    		CPUNIT_ASSERT(res_ != nullptr);
    		CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    		CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadCoils);

    		// Check contents of write single coil response
    		auto readCoilsRes = std::static_pointer_cast<ModbusProt::ReadCoilsResPDU>(res_);
    		CPUNIT_ASSERT(readCoilsRes->byteCount() == 1);
    		uint8_t b; CPUNIT_ASSERT(readCoilsRes->getCoilStatus(1, &b) == true);
    		CPUNIT_ASSERT(b == (address % 3 == 0 ? 0x01 : 0x00));
    	}

    	// Read coil request (all bits)
    	responseCondition_.init();

    	// Create and send read coil request
    	auto readCoilsReq = std::make_shared<ModbusProt::ReadCoilsReqPDU>();
    	readCoilsReq->startingAddress(1);
    	readCoilsReq->quantityOfInputs(100);
    	ModbusProt::ModbusPDU::SPtr req = readCoilsReq;
    	client.send(0, req, responseCallback);
    	CPUNIT_ASSERT(responseCondition_.wait(3000) == true);

    	// Receive and check write single coil response
    	CPUNIT_ASSERT(modbusError_ == ModbusProt::ModbusError::Ok);
    	CPUNIT_ASSERT(res_ != nullptr);
    	CPUNIT_ASSERT(res_->pduType() == ModbusProt::PDUType::Response);
    	CPUNIT_ASSERT(req_->pduFunction() == ModbusProt::PDUFunction::ReadCoils);

    	// Check contents of write single coil response
    	auto readCoilsRes = std::static_pointer_cast<ModbusProt::ReadCoilsResPDU>(res_);
    	CPUNIT_ASSERT(readCoilsRes->byteCount() == ((99/8)+1));
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		bool b; CPUNIT_ASSERT(readCoilsRes->getCoilStatus(idx, b) == true);
    	}

    	uint8_t value[MAX_BYTE_LEN];
    	memset((char*)&value, 0x00, MAX_BYTE_LEN);
    	CPUNIT_ASSERT(readCoilsRes->getCoilStatus(99, value) == true);
    	for (uint16_t idx = 0; idx < 99; idx++) {
    		uint8_t offset = idx / 8;
    		uint8_t rest = idx % 8;
    		bool b = (value[offset] & (1<<rest)) != 0;
    		CPUNIT_ASSERT(b == ((idx+1) % 3 == 0 ? true : false));
    	}
    }

}
