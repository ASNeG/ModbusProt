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
#include "ModbusProt/ReadInputRegistersPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class ReadInputRegistersReq
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadInputRegistersReqPDU::ReadInputRegistersReqPDU(void)
	: ModbusPDU(PDUFunction::ReadInputRegisters, PDUType::Request)
	{
	}

	ReadInputRegistersReqPDU::~ReadInputRegistersReqPDU(void)
	{
	}

	void
	ReadInputRegistersReqPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	ReadInputRegistersReqPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	ReadInputRegistersReqPDU::quantityOfInputs(uint16_t quantityOfInputs)
	{
		quantityOfInputs_ = quantityOfInputs;
	}

	uint16_t
	ReadInputRegistersReqPDU::quantityOfInputs(void)
	{
		return quantityOfInputs_;
	}

	bool
	ReadInputRegistersReqPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			uint16_t startingAddress = ByteOrder::toBig(startingAddress_);
			os.write((char*)&startingAddress, 2);

			uint16_t quantityOfInputs = ByteOrder::toBig(quantityOfInputs_);
			os.write((char*)&quantityOfInputs, 2);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	ReadInputRegistersReqPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&startingAddress_, 2);
			startingAddress_ = ByteOrder::fromBig(startingAddress_);

			is.read((char*)&quantityOfInputs_, 2);
			quantityOfInputs_ = ByteOrder::fromBig(quantityOfInputs_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class ReadInputRegistersRes
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadInputRegistersResPDU::ReadInputRegistersResPDU(void)
	: ModbusPDU(PDUFunction::ReadInputRegisters, PDUType::Response)
	{
		reset();
	}

	ReadInputRegistersResPDU::~ReadInputRegistersResPDU(void)
	{
	}

	void
	ReadInputRegistersResPDU::reset(void)
	{
		byteCount_ =  0;
		memset((char*)inputRegisters_, 0x00, MAX_BYTE_LEN);
	}

	uint8_t
	ReadInputRegistersResPDU::byteCount(void)
	{
		return byteCount_;
	}

	bool
	ReadInputRegistersResPDU::setInputRegisters(uint16_t count, uint16_t* value)
	{
		// Check max index
		if (count == 0 || count*2 > MAX_BYTE_LEN) return false;

		byteCount_ = count * 2;
		memcpy(inputRegisters_, value, byteCount_);
		return true;
	}

	bool
	ReadInputRegistersResPDU::setInputRegisters(uint16_t  idx, uint16_t value)
	{
		// Check index
		if (idx*2 >= MAX_BYTE_LEN) return false;

		// Set data
		inputRegisters_[idx] = value;
		if (byteCount_ < ((idx+1)*2)) byteCount_ = ((idx+1)*2);

		return true;
	}

	bool
	ReadInputRegistersResPDU::getInputRegisters(uint16_t count, uint16_t* value)
	{
		// Check max index
		if (count == 0) return false;
		if (count*2 > MAX_BYTE_LEN) return false;
		if (count*2 >= byteCount_) return false;

		uint8_t byteCount = count*2;
		memcpy(value, inputRegisters_, byteCount);

		return true;
	}

	bool
	ReadInputRegistersResPDU::getInputRegisters(uint16_t idx, uint16_t& value)
	{
		// Check index
		if (idx*2 >= MAX_BYTE_LEN) return false;
		if (idx*2 >= byteCount_) return false;

		// Get data
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		value = inputRegisters_[idx];

		return true;
	}

	bool
	ReadInputRegistersResPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			os.write((char*)&byteCount_, 1);
			os.write((char*)&inputRegisters_, byteCount_);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	ReadInputRegistersResPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&byteCount_, 1);
			if (byteCount_ > MAX_BYTE_LEN) return false;
			is.read((char*)&inputRegisters_, byteCount_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
