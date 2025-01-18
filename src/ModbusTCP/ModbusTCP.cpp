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

#include <sstream>
#include <ios>

#include "ModbusProt/ByteOrder.h"
#include "ModbusProt/ModbusPDUFactory.h"
#include "ModbusTCP/ModbusTCP.h"

using namespace ModbusProt;

namespace ModbusTCP
{


	ModbusTCP::ModbusTCP(void)
	{
	}

	ModbusTCP::~ModbusTCP(void)
	{
	}

	void
	ModbusTCP::transactionIdentifier(uint16_t transactionIdentifier)
	{
		transactionIdentifier_ = transactionIdentifier;
	}

	uint16_t
	ModbusTCP::transactionIdentifier(void)
	{
		return transactionIdentifier_;
	}

	void
	ModbusTCP::protocolIdentifier(uint16_t protocolIdentifier)
	{
		protocolIdentifier_ = protocolIdentifier;
	}

	uint16_t
	ModbusTCP::protocolIdentifier(void)
	{
		return protocolIdentifier_;
	}

	uint16_t
	ModbusTCP::length(void)
	{
		return length_;
	}

	void
	ModbusTCP::unitIdentifier(uint8_t unitIdentifier)
	{
		unitIdentifier_ = unitIdentifier;
	}

	uint8_t
	ModbusTCP::unitIdentifier(void)
	{
		return unitIdentifier_;
	}

	void
	ModbusTCP::modbusPDU(ModbusPDU::SPtr& modbusPDU)
	{
		modbusPDU_ = modbusPDU;
	}

	ModbusPDU::SPtr&
	ModbusTCP::modbusPDU(void)
	{
		return modbusPDU_;
	}

	void
	ModbusTCP::pduType(ModbusProt::PDUType pduType)
	{
		pduType_ = pduType;
	}

	bool
	ModbusTCP::encode(std::ostream& os) const
	{
		// check parameter
		if (modbusPDU_ == nullptr) {
			return false;
		}

		// Get length of PDU
		std::stringstream ss;
		if (!modbusPDU_->encode(ss)) {
			return false;
		}
		ss.seekg(0, std::ios::end);
		*(const_cast<uint16_t*>(&length_)) = ss.tellg();
		ss.seekg(0, std::ios::beg);

		// Write MBAP header
		try {
			uint16_t transactionIdentifier = ByteOrder::toBig(transactionIdentifier_);
			os.write((char*)&transactionIdentifier, 2);

			uint16_t protocolIdentifier = ByteOrder::toBig(protocolIdentifier_);
			os.write((char*)&protocolIdentifier, 2);

			uint16_t length = ByteOrder::toBig(length_+1);
			os.write((char*)&length, 2);

			os.write((char*)&unitIdentifier_, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		// Encode modbus PDU data
		if (!modbusPDU_->encode(os)) {
			return false;
		}

		return true;
	}

	bool
	ModbusTCP::decode(std::istream& is)
	{
		// Read MBAP header
		try {
			uint16_t transactionIdentifier;
			is.read((char*)&transactionIdentifier, 2);
			transactionIdentifier_ = ByteOrder::fromBig(transactionIdentifier);

			uint16_t protocolIdentifier;
			is.read((char*)&protocolIdentifier, 2);
			protocolIdentifier_ = ByteOrder::fromBig(protocolIdentifier);

			uint16_t length;
			is.read((char*)&length, 2);
			length_ = ByteOrder::fromBig(length) - 1;

			is.read((char*)&unitIdentifier_, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		// Decode function code of modbus PDU
		std::cout << "xxxxxxxxxxxxxxxxxx" << std::endl;
		ModbusPDU modbusPDU(PDUFunction::None, pduType_);
		if (!modbusPDU.decode(is)) return false;
		std::cout << "xxxxxxxxxxxxxxxxxx" << std::endl;

		// Create modbus PDU class
		modbusPDU_ = ModbusPDUFactory::createModbusPDU(modbusPDU.pduFunction(), modbusPDU.pduType());
		if (modbusPDU_ == nullptr) return false;

		// Decode modbus PDU data
		if (!modbusPDU_->decode(is)) return false;

		return true;
	}

}
