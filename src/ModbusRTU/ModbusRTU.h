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

#ifndef __ModbusProt_ModbusRTU_h__
#define __ModbusProt_ModbusRTU_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusRTU
{

	class ModbusRTU
	{
	  public:
		ModbusRTU(void);
		~ModbusRTU(void);

		void slaveAddress(uint8_t slaveAddress);
		uint8_t slaveAddress(void);

		void modbusPDU(ModbusProt::ModbusPDU::SPtr& modbusPDU);
		ModbusProt::ModbusPDU::SPtr& modbusPDU(void);

		bool encode(std::ostream& os) const;
		bool decode(std::istream& is);

	  private:
		uint8_t slaveAddress_ = 0;
		ModbusProt::ModbusPDU::SPtr modbusPDU_ = nullptr;
	};

}

#endif
