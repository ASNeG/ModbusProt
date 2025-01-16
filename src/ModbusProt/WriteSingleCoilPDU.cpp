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
#include "ModbusProt/WriteSingleCoilPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteSingleCoilReqPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteSingleCoilReqPDU::WriteSingleCoilReqPDU(void)
	: ModbusPDU(PDUFunction::WriteSingleCoil, PDUType::Request)
	{
	}

	WriteSingleCoilReqPDU::~WriteSingleCoilReqPDU(void)
	{
	}

	void
	WriteSingleCoilReqPDU::address(uint16_t address)
	{
		address_ = address;
	}

	uint16_t
	WriteSingleCoilReqPDU::address(void)
	{
		return address_;
	}

	void
	WriteSingleCoilReqPDU::value(bool value)
	{
		value_ = value;
	}

	bool
	WriteSingleCoilReqPDU::value(void)
	{
		return value_;
	}

	bool
	WriteSingleCoilReqPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			uint16_t address = ByteOrder::toBig(address_);
			os.write((char*)&address, 2);

			uint8_t byte = 0x00;
			if (value_) byte = 0xFF;
			os.write((char*)&byte, 1);
			byte = 0x00;
			os.write((char*)&byte, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteSingleCoilReqPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&address_, 2);
			address_ = ByteOrder::fromBig(address_);

			uint8_t byte = 0;
			is.read((char*)&byte, 1);
			if (byte == 0xFF) value_ = true;
			else value_ = false;
			is.read((char*)&byte, 1);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteSingleCoilResPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteSingleCoilResPDU::WriteSingleCoilResPDU(void)
	: ModbusPDU(PDUFunction::WriteSingleCoil, PDUType::Response)
	{
	}

	WriteSingleCoilResPDU::~WriteSingleCoilResPDU(void)
	{
	}

	void
	WriteSingleCoilResPDU::address(uint16_t address)
	{
		address_ = address;
	}

	uint16_t
	WriteSingleCoilResPDU::address(void)
	{
		return address_;
	}

	void
	WriteSingleCoilResPDU::value(bool value)
	{
		value_ = value;
	}

	bool
	WriteSingleCoilResPDU::value(void)
	{
		return value_;
	}

	bool
	WriteSingleCoilResPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			uint16_t address = ByteOrder::toBig(address_);
			os.write((char*)&address, 2);

			uint8_t byte = 0x00;
			if (value_) byte = 0xFF;
			os.write((char*)&byte, 1);
			byte = 0x00;
			os.write((char*)&byte, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteSingleCoilResPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&address_, 2);
			address_ = ByteOrder::fromBig(address_);

			uint8_t byte = 0;
			is.read((char*)&byte, 1);
			if (byte == 0xFF) value_ = true;
			else value_ = false;
			is.read((char*)&byte, 1);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
