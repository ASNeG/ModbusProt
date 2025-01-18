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
#include "WriteMultipleCoilsPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteMultipleCoilsReqPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteMultipleCoilsReqPDU::WriteMultipleCoilsReqPDU(void)
	: ModbusPDU(PDUFunction::WriteMultipleCoils, PDUType::Request)
	{
	}

	WriteMultipleCoilsReqPDU::~WriteMultipleCoilsReqPDU(void)
	{
	}

	void
	WriteMultipleCoilsReqPDU::reset(void)
	{
		byteCount_ =  0;
		memset((char*)outputsValue_, 0x00, MAX_BYTE_LEN);
	}

	uint16_t
	WriteMultipleCoilsReqPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	WriteMultipleCoilsReqPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	WriteMultipleCoilsReqPDU::quantityOfOutputs(void)
	{
		return quantityOfOutputs_;
	}

	void
	WriteMultipleCoilsReqPDU::quantityOfOutputs(uint16_t quantityOfOutputs)
	{
		quantityOfOutputs_ = quantityOfOutputs;
	}

	bool
	WriteMultipleCoilsReqPDU::setOutputsValue(uint16_t count, uint8_t* value)
	{
		// Check max index
		if (count == 0 || count > MAX_BYTE_LEN * 8) return false;

		byteCount_ = ((count-1)/8) + 1;
		memcpy(outputsValue_, value, byteCount_);
		std::cout << "YYYYYYYY" << (uint32_t)byteCount_ << std::endl;
		return true;
	}

	bool
	WriteMultipleCoilsReqPDU::setOutputsValue(uint16_t idx, bool value)
	{
		// Check index
		if (idx >= MAX_BYTE_LEN * 8) return false;

		// Set coil status
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		if (value == true) {
			outputsValue_[byteIdx] = outputsValue_[byteIdx] | 1 << posIdx;
		}
		else {
			outputsValue_[byteIdx] = outputsValue_[byteIdx] & ~(1 << posIdx);
		}
		uint8_t byteCount = ((idx-1)/8) + 1;
		if (byteCount_ < byteCount) byteCount_ = byteCount;
		return true;
	}

	bool
	WriteMultipleCoilsReqPDU::getOutputsValue(uint16_t count, uint8_t* value)
	{
		// Check max index
		if (count == 0) return false;
		if (count > MAX_BYTE_LEN * 8) return false;
		if (count >= byteCount_ * 8) return false;

		uint8_t byteCount = ((count-1)/8) + 1;
		memcpy(value, outputsValue_, byteCount);

		return true;
	}

	bool
	WriteMultipleCoilsReqPDU::getOutputsValue(uint16_t idx, bool& value)
	{
		// Check index
		if (idx >= MAX_BYTE_LEN * 8) return false;
		if (idx >= byteCount_ * 8) return false;

		// Get coil status
		uint8_t byteIdx = idx / 8;
		uint8_t posIdx = idx % 8;

		uint8_t byte = 1 << byteIdx;
		value = ((outputsValue_[byteIdx] & byte) == byte);

		return true;
	}

	uint8_t*
	WriteMultipleCoilsReqPDU::outputsValue(void)
	{
		return outputsValue_;
	}

	bool
	WriteMultipleCoilsReqPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		std::cout << "*** " << (uint32_t)startingAddress_ << std::endl;
		std::cout << "*** " << (uint32_t)quantityOfOutputs_ << std::endl;
		std::cout << "*** " << (uint32_t)byteCount_ << std::endl;


		// Write data to output stream
		try {
			uint16_t startingAddress = ByteOrder::toBig(startingAddress_);
			os.write((char*)&startingAddress, 2);
			uint16_t quantityOfOutputs = ByteOrder::toBig(quantityOfOutputs_);
			os.write((char*)&quantityOfOutputs, 2);

			os.write((char*)&byteCount_, 1);
			os.write((char*)&outputsValue_, byteCount_);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteMultipleCoilsReqPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&startingAddress_, 2);
			startingAddress_ = ByteOrder::fromBig(startingAddress_);
			is.read((char*)&quantityOfOutputs_, 2);
			quantityOfOutputs_ = ByteOrder::fromBig(quantityOfOutputs_);

			is.read((char*)&byteCount_, 1);
			if (byteCount_ > MAX_BYTE_LEN) return false;
			is.read((char*)&outputsValue_, byteCount_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteMultipleCoilsResPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteMultipleCoilsResPDU::WriteMultipleCoilsResPDU(void)
	: ModbusPDU(PDUFunction::WriteMultipleCoils, PDUType::Response)
	{
	}

	WriteMultipleCoilsResPDU::~WriteMultipleCoilsResPDU(void)
	{
	}

	uint16_t
	WriteMultipleCoilsResPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	WriteMultipleCoilsResPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	WriteMultipleCoilsResPDU::quantityOfOutputs(void)
	{
		return quantityOfOutputs_;
	}

	void
	WriteMultipleCoilsResPDU::quantityOfOutputs(uint16_t quantityOfOutputs)
	{
		quantityOfOutputs_ = quantityOfOutputs;
	}

	bool
	WriteMultipleCoilsResPDU::encode(std::ostream& os) const
	{
		uint16_t startingAddress = ByteOrder::toBig(startingAddress_);
		os.write((char*)&startingAddress, 2);

		uint16_t quantityOfOutputs = ByteOrder::toBig(quantityOfOutputs);
		os.write((char*)&quantityOfOutputs, 2);

		return true;
	}

	bool
	WriteMultipleCoilsResPDU::decode(std::istream& is)
	{
		is.read((char*)&startingAddress_, 2);
		startingAddress_ = ByteOrder::fromBig(startingAddress_);

		is.read((char*)&quantityOfOutputs_, 2);
		quantityOfOutputs_ = ByteOrder::fromBig(quantityOfOutputs_);

		return true;
	}

}
