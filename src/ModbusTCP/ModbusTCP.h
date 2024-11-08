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

#ifndef __ModbusTCP_ModbusTCP_h__
#define __ModbusTCP_ModbusTCP_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusTCP
{

	class ModbusTCP
	{
	  public:
		ModbusTCP(void);
		~ModbusTCP(void);

		void transactionIdentifier(uint16_t transactionIdentifier);
		uint16_t transactionIdentifier(void);
		void protocolIdentifier(uint16_t protocolIdentifier);
		uint16_t protocolIdentifier(void);
		uint16_t length(void);
		void unitIdentifier(uint8_t unitIdentifier);
		uint8_t unitIdentifier(void);

		void modbusPDU(ModbusProt::ModbusPDU::SPtr& modbusPDU);
		ModbusProt::ModbusPDU::SPtr& modbusPDU(void);

		bool encode(std::ostream& os) const;
		bool decode(std::istream& is);

	  private:
		uint16_t transactionIdentifier_ = 0;
		uint16_t protocolIdentifier_= 0;
		uint16_t length_ = 0;
		uint8_t unitIdentifier_ = 0;
		ModbusProt::ModbusPDU::SPtr modbusPDU_ = nullptr;
	};

}

#endif
