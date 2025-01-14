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

#ifndef __ModbusTCP_ModbusTCPQueueElement_h__
#define __ModbusTCP_ModbusTCPQueueElement_h__

#include <functional>

#include "Base/QueueElement.h"
#include "ModbusProt/ModbusCallback.h"

namespace ModbusTCP
{

	class ModbusTCPQueueElement
	: public Base::QueueElement
	{
	  public:
		using SPtr = std::shared_ptr<ModbusTCPQueueElement>;

		ModbusTCPQueueElement(void);
		~ModbusTCPQueueElement(void);

		uint8_t unitIdentifier_ = 0x00;
		ModbusProt::ModbusPDU::SPtr req_;
		ModbusProt::ModbusPDU::SPtr res_;
		ModbusProt::ResponseCallback responseCallback_;
	};

}

#endif
