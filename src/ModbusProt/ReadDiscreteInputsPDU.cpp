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
#include "ModbusProt/ReadDiscreteInputsPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class ReadDiscreteInputsReq
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadDiscreteInputsReqPDU::ReadDiscreteInputsReqPDU(void)
	: ModbusPDU(PDUFunction::ReadDiscreteInputs, PDUType::Request)
	{
	}

	ReadDiscreteInputsReqPDU::~ReadDiscreteInputsReqPDU(void)
	{
	}

	void
	ReadDiscreteInputsReqPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	ReadDiscreteInputsReqPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	ReadDiscreteInputsReqPDU::quantityOfInputs(uint16_t quantityOfInputs)
	{
		quantityOfInputs_ = quantityOfInputs;
	}

	uint16_t
	ReadDiscreteInputsReqPDU::quantityOfInputs(void)
	{
		return quantityOfInputs_;
	}

	bool
	ReadDiscreteInputsReqPDU::encode(std::ostream& os) const
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
	ReadDiscreteInputsReqPDU::decode(std::istream& is)
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
	// Class ReadDiscreteInputsRes
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadDiscreteInputsResPDU::ReadDiscreteInputsResPDU(void)
	: ModbusPDU(PDUFunction::ReadDiscreteInputs, PDUType::Response)
	{
		reset();
	}

	ReadDiscreteInputsResPDU::~ReadDiscreteInputsResPDU(void)
	{
	}

	void
	ReadDiscreteInputsResPDU::reset(void)
	{
		byteCount_ =  0;
		memset((char*)inputStatus_, 0x00, MAX_BYTE_LEN);
	}

	uint8_t
	ReadDiscreteInputsResPDU::byteCount(void)
	{
		return byteCount_;
	}

	bool
	ReadDiscreteInputsResPDU::setInputStatus(uint16_t count, uint8_t* value)
	{
		// Check max index
		if (count == 0 || count > MAX_BYTE_LEN * 8) return false;

		byteCount_ = ((count-1)/8) + 1;
		memcpy(inputStatus_, value, byteCount_);
		return true;
	}

	bool
	ReadDiscreteInputsResPDU::setInputStatus(uint16_t  idx, bool value)
	{
		// Check index
		if (idx >= MAX_BYTE_LEN * 8) return false;

		// Set coil status
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		if (value == true) {
			inputStatus_[byteIdx] = inputStatus_[byteIdx] | 1 << posIdx;
		}
		else {
			inputStatus_[byteIdx] = inputStatus_[byteIdx] & ~(1 << posIdx);
		}
		if (byteCount_ < (byteIdx+1)) byteCount_ = byteIdx+1;

		return true;
	}

	bool
	ReadDiscreteInputsResPDU::getInputStatus(uint16_t count, uint8_t* value)
	{
		// Check max index
		if (count == 0) return false;
		if (count > MAX_BYTE_LEN * 8) return false;
		if (count >= byteCount_ * 8) return false;

		uint8_t byteCount = ((count-1)/8) + 1;
		memcpy(value, inputStatus_, byteCount);

		return true;
	}

	bool
	ReadDiscreteInputsResPDU::getInputStatus(uint16_t idx, bool& value)
	{
		// Check index
		if (idx >= MAX_BYTE_LEN * 8) return false;
		if (idx >= byteCount_ * 8) return false;

		// Get coil status
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		uint8_t byte = 1 << posIdx;
		value = ((inputStatus_[byteIdx] & byte) == byte);

		return true;
	}

	bool
	ReadDiscreteInputsResPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			os.write((char*)&byteCount_, 1);
			os.write((char*)&inputStatus_, byteCount_);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	ReadDiscreteInputsResPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&byteCount_, 1);
			if (byteCount_ > MAX_BYTE_LEN) return false;
			is.read((char*)&inputStatus_, byteCount_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
