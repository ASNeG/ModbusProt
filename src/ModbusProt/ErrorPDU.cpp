/*
   Copyright 2024-2025 Kai Huebl (kai@huebl-sgh.de)

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

#include "ModbusProt/ErrorPDU.h"
#include "ModbusProt/ByteOrder.h"

namespace ModbusProt
{

	ErrorRes::ErrorRes(PDUFunction pduFunction)
	: ModbusPDU(pduFunction, PDUType::Error)
	{
	}

	ErrorRes::~ErrorRes(void)
	{
	}

	void
	ErrorRes::exceptionCode(uint8_t exceptionCode)
	{
		exceptionCode_ = exceptionCode;
	}

	uint8_t
	ErrorRes::exceptionCode(void)
	{
		return exceptionCode_;
	}

	bool
	ErrorRes::encode(std::ostream& os) const
	{
		// Write PDU header to output stream
		if (ModbusPDU::encode(os) == false) {
			return false;
		}

		// Write data to output stream
		try {
			os.write((char*)&exceptionCode_, 1);
		}
		catch (std::ostream::failure e) {
			return false;
		}

		return true;
	}

	bool
	ErrorRes::decode(std::istream& is)
	{
		// Read data  from intput stream
		try {
			is.read((char*)&exceptionCode_, 1);
		}
		catch (std::istream::failure e) {
			return false;
		}

		return true;
	}

}
