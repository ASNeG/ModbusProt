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
#include "WriteMultipleHoldingRegistersPDU.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteMultipleHoldingRegistersReqPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteMultipleHoldingRegistersReqPDU::WriteMultipleHoldingRegistersReqPDU(void)
	: ModbusPDU(PDUFunction::WriteMultipleHoldingRegisters, PDUType::Request)
	{
	}

	WriteMultipleHoldingRegistersReqPDU::~WriteMultipleHoldingRegistersReqPDU(void)
	{
	}

	void
	WriteMultipleHoldingRegistersReqPDU::reset(void)
	{
		byteCount_ =  0;
		memset((char*)registersValue_, 0x00, MAX_BYTE_LEN);
	}

	uint16_t
	WriteMultipleHoldingRegistersReqPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	WriteMultipleHoldingRegistersReqPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	WriteMultipleHoldingRegistersReqPDU::quantityOfRegisters(void)
	{
		return quantityOfRegisters_;
	}

	void
	WriteMultipleHoldingRegistersReqPDU::quantityOfRegisters(uint16_t quantityOfRegisters)
	{
		quantityOfRegisters_ = quantityOfRegisters;
	}

	bool
	WriteMultipleHoldingRegistersReqPDU::setRegistersValue(uint16_t count, uint16_t* value)
	{
		// Check max index
		if (count == 0 || count*2 > MAX_BYTE_LEN) return false;

		memcpy(registersValue_, (char*)value, count*2);
		if (byteCount_ < (count+1)*2) byteCount_ = (count+1)*2;
		return true;
	}

	bool
	WriteMultipleHoldingRegistersReqPDU::setRegistersValue(uint16_t idx, uint16_t value)
	{
		// Check index
		if (idx*2 >= MAX_BYTE_LEN) return false;

		// Set register value
		registersValue_[idx] = value;
		if (byteCount_ < (idx+1)*2) byteCount_ = (idx+1)*2;
		return true;
	}

	bool
	WriteMultipleHoldingRegistersReqPDU::getRegistersValue(uint16_t count, uint16_t* value)
	{
		// Check max index
		if (count == 0) return false;
		if (count*2 > MAX_BYTE_LEN) return false;
		if (count*2 > byteCount_) return false;

		memcpy(value, registersValue_, count*2);

		return true;
	}

	bool
	WriteMultipleHoldingRegistersReqPDU::getRegistersValue(uint16_t idx, uint16_t& value)
	{
		// Check index
		if (idx*2 > MAX_BYTE_LEN) return false;
		if (idx*2 > byteCount_) return false;

		// Get coil status
		value = registersValue_[idx];

		return true;
	}

	bool
	WriteMultipleHoldingRegistersReqPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			uint16_t startingAddress = ByteOrder::toBig(startingAddress_);
			os.write((char*)&startingAddress, 2);
			uint16_t quantityOfRegisters = ByteOrder::toBig(quantityOfRegisters_);
			os.write((char*)&quantityOfRegisters, 2);

			os.write((char*)&byteCount_, 1);

			uint16_t sendBuffer[MAX_BYTE_LEN];
			for (uint32_t idx = 0; idx < byteCount_/2; idx++) {
				sendBuffer[idx] = ByteOrder::toBig(registersValue_[idx]);
			}
			os.write((char*)sendBuffer, byteCount_);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteMultipleHoldingRegistersReqPDU::decode(std::istream& is)
	{
		// Read data from input stream
		try {
			is.read((char*)&startingAddress_, 2);
			startingAddress_ = ByteOrder::fromBig(startingAddress_);
			is.read((char*)&quantityOfRegisters_, 2);
			quantityOfRegisters_ = ByteOrder::fromBig(quantityOfRegisters_);

			is.read((char*)&byteCount_, 1);
			if (byteCount_ > (MAX_BYTE_LEN/2)) return false;

			uint16_t recvBuffer[MAX_BYTE_LEN/2];
			is.read((char*)recvBuffer, (uint32_t)byteCount_);
			for (uint32_t idx = 0; idx < byteCount_/2; idx++) {
				registersValue_[idx] = ByteOrder::fromBig(recvBuffer[idx]);
			}

		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// Class WriteMultipleHoldingRegistersResPDU
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	WriteMultipleHoldingRegistersResPDU::WriteMultipleHoldingRegistersResPDU(void)
	: ModbusPDU(PDUFunction::WriteMultipleHoldingRegisters, PDUType::Response)
	{
	}

	WriteMultipleHoldingRegistersResPDU::~WriteMultipleHoldingRegistersResPDU(void)
	{
	}

	uint16_t
	WriteMultipleHoldingRegistersResPDU::startingAddress(void)
	{
		return startingAddress_;
	}

	void
	WriteMultipleHoldingRegistersResPDU::startingAddress(uint16_t startingAddress)
	{
		startingAddress_ = startingAddress;
	}

	uint16_t
	WriteMultipleHoldingRegistersResPDU::quantityOfRegisters(void)
	{
		return quantityOfRegisters_;
	}

	void
	WriteMultipleHoldingRegistersResPDU::quantityOfRegisters(uint16_t quantityOfRegisters)
	{
		quantityOfRegisters_ = quantityOfRegisters;
	}

	bool
	WriteMultipleHoldingRegistersResPDU::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		try {
			uint16_t startingAddress = ByteOrder::toBig(startingAddress_);
			os.write((char*)&startingAddress, 2);

			uint16_t quantityOfRegisters = ByteOrder::toBig(quantityOfRegisters_);
			os.write((char*)&quantityOfRegisters, 2);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	WriteMultipleHoldingRegistersResPDU::decode(std::istream& is)
	{
		try {
			is.read((char*)&startingAddress_, 2);
			startingAddress_ = ByteOrder::fromBig(startingAddress_);

			is.read((char*)&quantityOfRegisters_, 2);
			quantityOfRegisters_ = ByteOrder::fromBig(quantityOfRegisters_);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
