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
#include "ModbusProt/ErrorPDU.h"
#include "ModbusTCP/TCPServerModel.h"

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
		}

		// Create error response
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

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::Coils);
		if (!rc) {
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
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create modbus response
		auto readCoilRes = std::make_shared<ModbusProt::ReadCoilsResPDU>();

		// Get coil data from memory area
		uint8_t value[MAX_BYTE_LEN];
		rc = modbusModel_->getValue(
			ModbusProt::MemoryType::Coils,
			readCoilReq->startingAddress(),
			value,
			readCoilReq->quantityOfInputs()
		);
		if (!rc) {
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		readCoilRes->setCoilStatus(readCoilReq->quantityOfInputs(), value);

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

		// Check if function exist
		rc = modbusModel_->checkType(ModbusProt::MemoryType::Coils);
		if (!rc) {
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
			res = createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_ADDRESS_UNKNWON);
			return true;
		}

		// Create modbus response
		auto writeSingleCoilRes = std::make_shared<ModbusProt::WriteSingleCoilResPDU>();

		// Get coil data from memory area
		uint8_t value;
		rc = modbusModel_->getValue(
			ModbusProt::MemoryType::Coils,
			writeSingleCoilReq->address(),
			&value,
			1
		);
		if (!rc) {
			res = TCPServerHandler::createErrorPDU(req->pduFunction(), ModbusProt::ErrorPDU::ExceptionCode::EC_PROCESSING_ERROR);
			return true;
		}
		if ((value & 0x80) == 0x80) writeSingleCoilRes->value(true);
		else writeSingleCoilRes->value(false);
		writeSingleCoilRes->address(writeSingleCoilRes->address());

		return true;
	}

}
