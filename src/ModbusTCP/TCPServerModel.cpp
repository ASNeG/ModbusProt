/*
   Copyright 2025 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#include <asio.hpp>

#include "ModbusProt/ReadCoilsPDU.h"
#include "ModbusProt/WriteSingleCoilPDU.h"
#include "ModbusProt/WriteMultipleCoilsPDU.h"
#include "ModbusProt/ErrorPDU.h"
#include "ModbusTCP/TCPServerModel.h"
#include "ModbusProt/ReadDiscreteInputsPDU.h"
#include "ModbusProt/ReadInputRegistersPDU.h"
#include "ModbusProt/ReadMultipleHoldingRegistersPDU.h"
#include "ModbusProt/WriteSingleHoldingRegisterPDU.h"
#include "ModbusProt/WriteMultipleHoldingRegistersPDU.h"

namespace ModbusTCP
{


	TCPServerModel::TCPServerModel(
		asio::io_context& ctx
	)
	: TCPServerHandler(ctx)
	{
	}

	TCPServerModel::TCPServerModel(
		void
	)
	: TCPServerHandler()
	{
	}

	TCPServerModel::~TCPServerModel(
		void
	)
	{
	}

	void
	TCPServerModel::addModbusModel(
		ModbusProt::ModbusModel::SPtr modbusModel
	)
	{
		modbusModel_ = modbusModel;
	}

	bool
	TCPServerModel::handleModbusReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		// check  if modbus model exist
		if (modbusModel_ == nullptr) {
			logHandler_->logList(Base::LogLevel::Error, {"modbus model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Handle coil
		switch (req->pduFunction())
		{
			case ModbusProt::PDUFunction::ReadCoils:
				return handleReadCoilsReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::WriteSingleCoil:
				return handleWriteSingleCoilReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::WriteMultipleCoils:
				return handleWriteMultipleCoilsReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::ReadDiscreteInputs:
				return handleReadDiscreteInputsReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::ReadInputRegisters:
				return handleReadInputRegistersReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::ReadMultipleHoldingRegisters:
				return handleReadMultipleHoldingRegistersReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::WriteSingleHoldingRegister:
				return handleWriteSingleHoldingRegisterReq(unitIdentifier, req, res);
			case ModbusProt::PDUFunction::WriteMultipleHoldingRegisters:
				return handleWriteMultipleHoldingRegistersReq(unitIdentifier, req, res);
		}

		// Create error response
		logHandler_->logList(Base::LogLevel::Error, {
			"modbus request error, because pdu function unknown:",
			pduFunctionToString(req->pduFunction())
		});
		res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
		return true;
	}

	bool
	TCPServerModel::handleReadCoilsReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto readCoilReq = std::static_pointer_cast<ModbusProt::ReadCoilsReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle read coils request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::Coils);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::Coils,
			readCoilReq->startingAddress(),
			readCoilReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"starting address", std::to_string(readCoilReq->startingAddress()),
				"with range", std::to_string(readCoilReq->quantityOfInputs()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create modbus response
		auto readCoilRes = std::make_shared<ModbusProt::ReadCoilsResPDU>();

		// Get coil data from memory area
		uint8_t value[MAX_BYTE_LEN];
		memset((char*)&value, 0x00, MAX_BYTE_LEN);
		rc = modbusModel_->getValue(
			ModbusProt::MemoryType::Coils,
			readCoilReq->startingAddress(),
			value,
			readCoilReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing read coils request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		readCoilRes->setCoilStatus(readCoilReq->quantityOfInputs(), value);

		res = readCoilRes;
		return true;
	}

	bool
	TCPServerModel::handleWriteSingleCoilReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto writeSingleCoilReq = std::static_pointer_cast<ModbusProt::WriteSingleCoilReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle write single coil request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::Coils);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::Coils,
			writeSingleCoilReq->address(),
			1
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"address", std::to_string(writeSingleCoilReq->address()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create write single coil response
		auto writeSingleCoilRes = std::make_shared<ModbusProt::WriteSingleCoilResPDU>();

		// Set coil data to memory area
		uint8_t value = 0x00;
		if (writeSingleCoilReq->value() == true) value = 0x01;
		rc = modbusModel_->setValue(
			ModbusProt::MemoryType::Coils,
			writeSingleCoilReq->address(),
			&value,
			1
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing write single coil request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		writeSingleCoilRes->value(writeSingleCoilReq->value());
		writeSingleCoilRes->address(writeSingleCoilReq->address());
		res = writeSingleCoilRes;

		return true;
	}

	bool
	TCPServerModel::handleWriteMultipleCoilsReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto writeMultipleCoilsReq = std::static_pointer_cast<ModbusProt::WriteMultipleCoilsReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle write multiple coils request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::Coils);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::Coils,
			writeMultipleCoilsReq->startingAddress(),
			1
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"starting address", std::to_string(writeMultipleCoilsReq->startingAddress()),
				"with range", std::to_string(writeMultipleCoilsReq->quantityOfOutputs()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create write multiple coils response
		auto writeMultipleCoilsRes = std::make_shared<ModbusProt::WriteMultipleCoilsResPDU>();

		// Set coil data to memory area
		rc = modbusModel_->setValue(
			ModbusProt::MemoryType::Coils,
			writeMultipleCoilsReq->startingAddress(),
			writeMultipleCoilsReq->outputsValue(),
			writeMultipleCoilsReq->quantityOfOutputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing write multiple coils request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		writeMultipleCoilsRes->quantityOfOutputs(writeMultipleCoilsReq->quantityOfOutputs());
		writeMultipleCoilsRes->startingAddress(writeMultipleCoilsReq->startingAddress());
		res = writeMultipleCoilsRes;

		return true;
	}

	bool
	TCPServerModel::handleReadDiscreteInputsReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto readDiscreteInputsReq = std::static_pointer_cast<ModbusProt::ReadDiscreteInputsReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle read discrete inputs request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::Inputs);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::Inputs,
			readDiscreteInputsReq->startingAddress(),
			readDiscreteInputsReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"starting address", std::to_string(readDiscreteInputsReq->startingAddress()),
				"with range", std::to_string(readDiscreteInputsReq->quantityOfInputs()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create modbus response
		auto readDiscreteInputsRes = std::make_shared<ModbusProt::ReadDiscreteInputsResPDU>();

		// Get coil data from memory area
		uint8_t value[MAX_BYTE_LEN];
		memset((char*)&value, 0x00, MAX_BYTE_LEN);
		rc = modbusModel_->getValue(
			ModbusProt::MemoryType::Inputs,
			readDiscreteInputsReq->startingAddress(),
			value,
			readDiscreteInputsReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing read discrete inputs request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		readDiscreteInputsRes->setInputStatus(readDiscreteInputsReq->quantityOfInputs(), value);

		res = readDiscreteInputsRes;
		return true;
	}

	bool
	TCPServerModel::handleReadInputRegistersReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto readInputRegistersReq = std::static_pointer_cast<ModbusProt::ReadInputRegistersReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle read input registers request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::InputRegisters);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::InputRegisters,
			readInputRegistersReq->startingAddress(),
			readInputRegistersReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"starting address", std::to_string(readInputRegistersReq->startingAddress()),
				"with range", std::to_string(readInputRegistersReq->quantityOfInputs()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create modbus response
		auto readInputRegistersRes = std::make_shared<ModbusProt::ReadInputRegistersResPDU>();

		// Get coil data from memory area
		uint8_t value[MAX_BYTE_LEN];
		memset((char*)&value, 0x00, MAX_BYTE_LEN);
		rc = modbusModel_->getValue(
			ModbusProt::MemoryType::InputRegisters,
			readInputRegistersReq->startingAddress(),
			value,
			readInputRegistersReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing read input registers request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		readInputRegistersRes->setInputRegisters(readInputRegistersReq->quantityOfInputs(), (uint16_t*)value);

		res = readInputRegistersRes;
		return true;
	}

	bool
	TCPServerModel::handleReadMultipleHoldingRegistersReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto readMultipleHoldingRegistersReq = std::static_pointer_cast<ModbusProt::ReadMultipleHoldingRegistersReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle read multiple holding registers request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::HoldingRegisters);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::HoldingRegisters,
			readMultipleHoldingRegistersReq->startingAddress(),
			readMultipleHoldingRegistersReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"starting address", std::to_string(readMultipleHoldingRegistersReq->startingAddress()),
				"with range", std::to_string(readMultipleHoldingRegistersReq->quantityOfInputs()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create modbus response
		auto readMultipleHoldingRegistersRes = std::make_shared<ModbusProt::ReadMultipleHoldingRegistersResPDU>();

		// Get coil data from memory area
		uint8_t value[MAX_BYTE_LEN];
		memset((char*)&value, 0x00, MAX_BYTE_LEN);
		rc = modbusModel_->getValue(
			ModbusProt::MemoryType::HoldingRegisters,
			readMultipleHoldingRegistersReq->startingAddress(),
			value,
			readMultipleHoldingRegistersReq->quantityOfInputs()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing read input registers request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		readMultipleHoldingRegistersRes->setHoldingRegisters(readMultipleHoldingRegistersReq->quantityOfInputs(), (uint16_t*)value);

		res = readMultipleHoldingRegistersRes;
		return true;
	}

	bool
	TCPServerModel::handleWriteSingleHoldingRegisterReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto writeSingleHoldingRegisterReq = std::static_pointer_cast<ModbusProt::WriteSingleHoldingRegisterReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle write single holding register request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::HoldingRegisters);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::HoldingRegisters,
			writeSingleHoldingRegisterReq->address(),
			1
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"address", std::to_string(writeSingleHoldingRegisterReq->address()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create write single holding register response
		auto writeSingleHoldingRegisterRes = std::make_shared<ModbusProt::WriteSingleHoldingRegisterResPDU>();

		// Set coil data to memory area
		uint16_t value = writeSingleHoldingRegisterReq->registerValue();
		rc = modbusModel_->setValue(
			ModbusProt::MemoryType::HoldingRegisters,
			writeSingleHoldingRegisterReq->address(),
			(uint8_t*)&value,
			1
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing write single coil request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		writeSingleHoldingRegisterRes->registerValue(writeSingleHoldingRegisterReq->registerValue());
		writeSingleHoldingRegisterRes->address(writeSingleHoldingRegisterReq->address());
		res = writeSingleHoldingRegisterRes;

		return true;
	}

	bool
	TCPServerModel::handleWriteMultipleHoldingRegistersReq(
		uint8_t unitIdentifier,
		ModbusProt::ModbusPDU::SPtr& req,
		ModbusProt::ModbusPDU::SPtr& res
	)
	{
		bool rc = true;
		auto writeMultipleHoldingRegistersReq = std::static_pointer_cast<ModbusProt::WriteMultipleHoldingRegistersReqPDU>(req);
		logHandler_->logList(Base::LogLevel::Debug, {"handle write multiple holding registers request"});

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::HoldingRegisters);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {"memory model not exist"});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_FUNC_UNKNWON);
			return true;
		}

		// Check if address is valid
		rc = modbusModel_->checkAddress(
			ModbusProt::MemoryType::HoldingRegisters,
			writeMultipleHoldingRegistersReq->startingAddress(),
			writeMultipleHoldingRegistersReq->quantityOfRegisters()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"address", std::to_string(writeMultipleHoldingRegistersReq->startingAddress()),
				"not exist"
			});
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create write multiple holding registers response
		auto writeMultipleHoldingRegistersRes = std::make_shared<ModbusProt::WriteMultipleHoldingRegistersResPDU>();

		// Set holding register data to memory area
		uint16_t value[MAX_BYTE_LEN/2];
		writeMultipleHoldingRegistersReq->getRegistersValue(writeMultipleHoldingRegistersReq->quantityOfRegisters(), value);
		rc = modbusModel_->setValue(
			ModbusProt::MemoryType::HoldingRegisters,
			writeMultipleHoldingRegistersReq->startingAddress(),
			(uint8_t*)&value,
			writeMultipleHoldingRegistersReq->quantityOfRegisters()
		);
		if (!rc) {
			logHandler_->logList(Base::LogLevel::Error, {
				"processing write single coil request error"
			});
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		writeMultipleHoldingRegistersRes->quantityOfRegisters(writeMultipleHoldingRegistersReq->quantityOfRegisters());
		writeMultipleHoldingRegistersRes->startingAddress(writeMultipleHoldingRegistersReq->startingAddress());
		res = writeMultipleHoldingRegistersRes;

		return true;
	}

}
