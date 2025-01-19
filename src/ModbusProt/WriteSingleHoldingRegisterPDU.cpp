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
#include <string.h>

#include "ModbusProt/ByteOrder.h"
#include "ModbusProt/WriteSingleHoldingRegisterPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteSingleHoldingRegisterReqPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteSingleHoldingRegisterReqPDU::WriteSingleHoldingRegisterReqPDU(void)
	: ModbusPDU(PDUFunction::WriteSingleHoldingRegister, PDUType::Request)
	{
	}

	WriteSingleHoldingRegisterReqPDU::~WriteSingleHoldingRegisterReqPDU(void)
	{
	}

	void
	WriteSingleHoldingRegisterReqPDU::address(uint16_t address)
	{
		address_ = address;
	}

	uint16_t
	WriteSingleHoldingRegisterReqPDU::address(void)
	{
		return address_;
	}

	void
	WriteSingleHoldingRegisterReqPDU::registerValue(uint16_t registerValue)
	{
		registerValue_ = registerValue;
	}

	uint16_t
	WriteSingleHoldingRegisterReqPDU::registerValue(void)
	{
		return registerValue_;
	}

	bool
	WriteSingleHoldingRegisterReqPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			uint16_t address = ByteOrder::toBig(address_);
			os.write((char*)&address, 2);
			uint16_t registerValue = ByteOrder::toBig(registerValue_);
			os.write((char*)&registerValue, 2);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteSingleHoldingRegisterReqPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&address_, 2);
			address_ = ByteOrder::fromBig(address_);
			is.read((char*)&registerValue_, 2);
			registerValue_ = ByteOrder::fromBig(registerValue_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteSingleHoldingRegisterResPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteSingleHoldingRegisterResPDU::WriteSingleHoldingRegisterResPDU(void)
	: ModbusPDU(PDUFunction::WriteSingleHoldingRegister, PDUType::Response)
	{
	}

	WriteSingleHoldingRegisterResPDU::~WriteSingleHoldingRegisterResPDU(void)
	{
	}

	void
	WriteSingleHoldingRegisterResPDU::address(uint16_t address)
	{
		address_ = address;
	}

	uint16_t
	WriteSingleHoldingRegisterResPDU::address(void)
	{
		return address_;
	}

	void
	WriteSingleHoldingRegisterResPDU::registerValue(uint16_t registerValue)
	{
		registerValue_ = registerValue;
	}

	uint16_t
	WriteSingleHoldingRegisterResPDU::registerValue(void)
	{
		return registerValue_;
	}

	bool
	WriteSingleHoldingRegisterResPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			uint16_t address = ByteOrder::toBig(address_);
			os.write((char*)&address, 2);
			uint16_t registerValue = ByteOrder::toBig(registerValue_);
			os.write((char*)&registerValue, 2);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteSingleHoldingRegisterResPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&address_, 2);
			address_ = ByteOrder::fromBig(address_);
			is.read((char*)&registerValue_, 2);
			registerValue_ = ByteOrder::fromBig(registerValue_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
