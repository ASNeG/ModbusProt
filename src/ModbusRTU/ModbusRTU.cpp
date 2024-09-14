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
#include "ModbusRTU/ModbusRTU.h"

using namespace ModbusProt;

namespace ModbusRTU
{


	ModbusRTU::ModbusRTU(void)
	{
	}

	ModbusRTU::~ModbusRTU(void)
	{
	}

	void
	ModbusRTU::slaveAddress(uint8_t slaveAddress)
	{
		slaveAddress_ = slaveAddress;
	}

	uint8_t
	ModbusRTU::slaveAddress(void)
	{
		return slaveAddress_;
	}

	void
	ModbusRTU::modbusPDU(ModbusPDU::SPtr& modbusPDU)
	{
		modbusPDU_ = modbusPDU;
	}

	ModbusPDU::SPtr&
	ModbusRTU::modbusPDU(void)
	{
		return modbusPDU_;
	}

	bool
	ModbusRTU::encode(std::ostream& os) const
	{
		// check parameter
		if (modbusPDU_ == nullptr) return false;

		// Write RTU header
		try {
			os.write((char*)&slaveAddress_, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		// Encode modbus PDU data
		if (!modbusPDU_->encode(os)) return false;

		return true;
	}

	bool
	ModbusRTU::decode(std::istream& is)
	{
		// Read RTU header
		try {
			is.read((char*)&slaveAddress_, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		// Read modbus function code
		uint8_t funcCode;
		try {
			is.read((char*)&funcCode, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		// Decode function code of modbus PDU
		ModbusPDU modbusPDU(PDUFunction::None, PDUType::None);
		if (!modbusPDU.decode(is)) return false;

		// Create modbus PDU class
		modbusPDU_ = ModbusPDUFactory::createModbusPDU(modbusPDU.pduFunction(), modbusPDU.pduType());
		if (modbusPDU_ == nullptr) return false;

		// Decode modbus PDU data
		if (!modbusPDU_->decode(is)) return false;

		return true;
	}

}
