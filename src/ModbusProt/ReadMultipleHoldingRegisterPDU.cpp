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
#include "ModbusProt/ReadMultipleHoldingRegistersPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class ReadMultipleHoldingRegistersReq
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadMultipleHoldingRegistersReqPDU::ReadMultipleHoldingRegistersReqPDU(void)
	: ModbusPDU(PDUFunction::ReadMultipleHoldingRegisters, PDUType::Request)
	{
	}

	ReadMultipleHoldingRegistersReqPDU::~ReadMultipleHoldingRegistersReqPDU(void)
	{
	}

	void
	ReadMultipleHoldingRegistersReqPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	ReadMultipleHoldingRegistersReqPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	ReadMultipleHoldingRegistersReqPDU::quantityOfInputs(uint16_t quantityOfInputs)
	{
		quantityOfInputs_ = quantityOfInputs;
	}

	uint16_t
	ReadMultipleHoldingRegistersReqPDU::quantityOfInputs(void)
	{
		return quantityOfInputs_;
	}

	bool
	ReadMultipleHoldingRegistersReqPDU::encode(std::ostream& os) const
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
	ReadMultipleHoldingRegistersReqPDU::decode(std::istream& is)
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
	// Class ReadMultipleHoldingRegistersRes
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadMultipleHoldingRegistersResPDU::ReadMultipleHoldingRegistersResPDU(void)
	: ModbusPDU(PDUFunction::ReadMultipleHoldingRegisters, PDUType::Response)
	{
		reset();
	}

	ReadMultipleHoldingRegistersResPDU::~ReadMultipleHoldingRegistersResPDU(void)
	{
	}

	void
	ReadMultipleHoldingRegistersResPDU::reset(void)
	{
		byteCount_ =  0;
		memset((char*)holdingRegisters, 0x00, MAX_BYTE_LEN);
	}

	uint8_t
	ReadMultipleHoldingRegistersResPDU::byteCount(void)
	{
		return byteCount_;
	}

	bool
	ReadMultipleHoldingRegistersResPDU::setHoldingRegisters(uint16_t count, uint16_t* value)
	{
		// Check max index
		if (count == 0 || count*2 > MAX_BYTE_LEN) return false;

		byteCount_ = count * 2;
		memcpy(holdingRegisters, value, byteCount_);
		return true;
	}

	bool
	ReadMultipleHoldingRegistersResPDU::setHoldingRegisters(uint16_t  idx, uint16_t value)
	{
		// Check index
		if (idx*2 >= MAX_BYTE_LEN) return false;

		// Set data
		holdingRegisters[idx] = value;
		if (byteCount_ < ((idx+1)*2)) byteCount_ = ((idx+1)*2);

		return true;
	}

	bool
	ReadMultipleHoldingRegistersResPDU::getHoldingRegisters(uint16_t count, uint16_t* value)
	{
		// Check max index
		if (count == 0) return false;
		if (count*2 > MAX_BYTE_LEN) return false;
		if (count*2 > byteCount_) return false;

		uint8_t byteCount = count*2;
		memcpy(value, holdingRegisters, byteCount);

		return true;
	}

	bool
	ReadMultipleHoldingRegistersResPDU::getHoldingRegisters(uint16_t idx, uint16_t& value)
	{
		// Check index
		if (idx*2 > MAX_BYTE_LEN) return false;
		if (idx*2 > byteCount_) return false;

		// Get data
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		value = holdingRegisters[idx];

		return true;
	}

	bool
	ReadMultipleHoldingRegistersResPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			os.write((char*)&byteCount_, 1);
			os.write((char*)&holdingRegisters, byteCount_);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	ReadMultipleHoldingRegistersResPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&byteCount_, 1);
			if (byteCount_ > MAX_BYTE_LEN) return false;
			is.read((char*)&holdingRegisters, byteCount_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
