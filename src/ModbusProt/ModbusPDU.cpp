/*
   Copyright 2024 Kai Huebl (kai@huebl-sgh.de)

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
		{ PDUFunction::ReadCoils, 0x01 }
	};


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
		auto it = funcCodeMap_.find(pduFunction_);
		if (it == funcCodeMap_.end()) return false;
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
		for (auto it: funcCodeMap_) {
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

}
