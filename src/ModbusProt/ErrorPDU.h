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

#ifndef __ModbusProt_ErrorPDU_h__
#define __ModbusProt_ErrorPDU_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class ErrorPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<ErrorPDU>;
		using ExceptionCode = enum {
			EC_FUNC_UNKNWON = 1,
			EC_ADDRESS_UNKNWON,
			EC_DATA_INVALID,
			EC_PROCESSING_ERROR,
			EC_PROCESSING_TAKES_A_LONG_TIME,
			EC_REPEAT_REQUEST,
			EC_PROG_UNKNOWN,
			EC_PARITY_ERROR
		};

		ErrorPDU(PDUFunction pduFunction);
		virtual ~ErrorPDU(void);

		uint8_t exceptionCode(void);
		void exceptionCode(uint8_t exceptionCode);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint8_t exceptionCode_ = 0x00;
	};

}

#endif
