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

#include "ModbusProt/ByteOrder.h"
#include "ModbusProt/ReadCoilsPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class ReadCoilsReq
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadCoilsReqPDU::ReadCoilsReqPDU(void)
	: ModbusPDU(PDUFunction::ReadCoils, PDUType::Request)
	{
	}

	ReadCoilsReqPDU::~ReadCoilsReqPDU(void)
	{
	}

	void
	ReadCoilsReqPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	ReadCoilsReqPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	ReadCoilsReqPDU::quantityOfInputs(uint16_t quantityOfInputs)
	{
		quantityOfInputs_ = quantityOfInputs;
	}

	uint16_t
	ReadCoilsReqPDU::quantityOfInputs(void)
	{
		return quantityOfInputs_;
	}

	bool
	ReadCoilsReqPDU::encode(std::ostream& os) const
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
	ReadCoilsReqPDU::decode(std::istream& is)
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
	// Class ReadCoilsRes
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ReadCoilsRes::ReadCoilsRes(void)
	: ModbusPDU(PDUFunction::ReadCoils, PDUType::Response)
	{
		reset();
	}

	ReadCoilsRes::~ReadCoilsRes(void)
	{
	}

	void
	ReadCoilsRes::reset(void)
	{
		byteCount_ =  0;
		memset((char*)coilStatus_, 0x00, MAX_BYTE_LEN);
	}

	uint8_t
	ReadCoilsRes::byteCount(void)
	{
		return byteCount_;
	}

	bool
	ReadCoilsRes::setCoilStatus(uint16_t  idx, bool value)
	{
		// Check index
		if (idx > MAX_BYTE_LEN * 8) return false;

		// Set coil status
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		if (value == true) {
			coilStatus_[byteIdx] = coilStatus_[byteIdx] | 0x01 << posIdx;
		}
		else {
			coilStatus_[byteIdx] = coilStatus_[byteIdx] & ~(0x01 << posIdx);
		}
		if (byteCount_ < byteIdx) byteCount_ = byteIdx;

		return true;
	}

	bool
	ReadCoilsRes::getCoilStatus(uint16_t idx, bool& value)
	{
		// Check index
		if (idx >= MAX_BYTE_LEN * 8) return false;
		if (idx >= byteCount_ * 8) return false;

		// Get coil status
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		uint8_t byte = 0x01 << byteIdx;
		value = ((coilStatus_[byteIdx] & byte) == byte);

		return true;
	}

	bool
	ReadCoilsRes::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			os.write((char*)&byteCount_, 1);
			os.write((char*)&coilStatus_, byteCount_);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	ReadCoilsRes::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&byteCount_, 1);
			if (byteCount_ > MAX_BYTE_LEN) return false;
			is.read((char*)&coilStatus_, byteCount_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
