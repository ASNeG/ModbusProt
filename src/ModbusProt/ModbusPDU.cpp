/*
   Copyright 2024-2025 Kai Huebl (kai@huebl-sgh.de)

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

#include <string.h>

#include "ModbusProt/ModbusPDU.h"
#include "ModbusProt/ByteOrder.h"

namespace ModbusProt
{

	ModbusPDU::FuncCodeMap funcCodeMap_ = {
		{ PDUFunction::ReadCoils, 0x01 },
		{ PDUFunction::ReadDiscreteInputs, 0x02 },
		{ PDUFunction::ReadMultipleHoldingRegisters, 0x03},
		{ PDUFunction::ReadInputRegisters, 0x04 },
		{ PDUFunction::WriteSingleCoil, 0x05 },
		{ PDUFunction::WriteSingleHoldingRegister, 0x06 },
		{ PDUFunction::WriteMultipleCoils, 0x0F },
		{ PDUFunction::WriteMultipleHoldingRegisters, 0x10 }
	};

	std::string
	pduFunctionToString(PDUFunction pduFunction)
	{
		switch (pduFunction)
		{
			case PDUFunction::ReadDiscreteInputs: return "ReadDiscreteInputs";
			case PDUFunction::ReadCoils: return "ReadCoils";
			case PDUFunction::WriteSingleCoil: return "WriteSingleCoil";
			case PDUFunction::WriteMultipleCoils: return "WriteMultipleCoils";
			case PDUFunction::ReadInputRegisters: return "ReadInputRegisters";
			case PDUFunction::ReadMultipleHoldingRegisters: return "ReadMultipleHoldingRegisters";
			case PDUFunction::WriteSingleHoldingRegister: return "WriteSingleHoldingRegister";
			case PDUFunction::WriteMultipleHoldingRegisters: return "WriteMultipleHoldingRegisters";
			case PDUFunction::ReadWriteMultipleRegisters: return "ReadWriteMultipleRegisters";
			case PDUFunction::MaskWriteRegister: return "MaskWriteRegister";
			case PDUFunction::ReadFifoQueue: return "ReadFifoQueue";
			case PDUFunction::ReadFileRecord: return "ReadFileRecord";
			case PDUFunction::WriteFileRecord: return "WriteFileRecord";
			case PDUFunction::ReadAxceptionStatus: return "ReadAxceptionStatus";
			case PDUFunction::Diagnostic: return "Diagnostic";
			case PDUFunction::GetComEventCounter: return "GetComEventCounter";
			case PDUFunction::GetComEventLog: return "ReadAxceptionStatus";
			case PDUFunction::ReportSlaveID: return "ReportSlaveID";
			case PDUFunction::ReadDeviceIdentification: return "ReadDeviceIdentification";
			case PDUFunction::EncapsulatedInterfaceTransport: return "EncapsulatedInterfaceTransport";
		}
		return "Unknown";
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class ModbusPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ModbusPDU::ModbusPDU(PDUFunction pduFunction, PDUType pduType)
	: pduFunction_(pduFunction)
	, pduType_(pduType)
	{
	}

	ModbusPDU::~ModbusPDU(void)
	{
	}

	bool
	ModbusPDU::encode(std::ostream& os) const
	{
		// Get function code
		auto it = ModbusProt::funcCodeMap_.find(pduFunction_);
		if (it == ModbusProt::funcCodeMap_.end()) return false;
		uint8_t pduFunction = it->second;
		if (pduType_ == PDUType::Error) {
			pduFunction += 0x80;
		}

		// Write function code to output stream
		try {
			os.write((char*)&pduFunction, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}
		return true;
	}

	bool
	ModbusPDU::decode(std::istream& is)
	{
		pduFunction_ = PDUFunction::None;

		// Read function code from input stream
		uint8_t funcCode =  0;
		try {
			is.read((char*)&funcCode, 1);
		}
		catch (std::istream::failure e) {
			return false;
		}
		if ((funcCode & 0x80) == 0x80) {
			funcCode -= 0x80;
			pduType_ = PDUType::Error;
		}

		// Find function code
		for (auto it: ModbusProt::funcCodeMap_) {
			if (it.second == funcCode) {
				pduFunction_ = it.first;
				return true;
			}
		}
		return false;
	}

	PDUFunction
	ModbusPDU::pduFunction(void)
	{
		return pduFunction_;
	}

	PDUType
	ModbusPDU::pduType(void)
	{
		return pduType_;
	}

	void
	ModbusPDU::pduType(PDUType pduType)
	{
		pduType_ = pduType;
	}

	std::string
	ModbusPDU::pduToString(void)
	{
		switch (pduType_)
		{
			case PDUType::None: return "None";
			case PDUType::Request: return "Request";
			case PDUType::Response: return "Response";
			case PDUType::Error: return "Error";
			default: return "Unknown";
		}
	}

}
