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

#ifndef __ModbusProt_ModbusModelInterface_h__
#define __ModbusProt_ModbusModelInterface_h__

#include <memory>

namespace ModbusProt
{

	enum class MemoryType {
		Coils,
		Inputs,
		InputRegisters,
		HoldingRegisters
	};

	class ModbusModelInterface {
	  public:
		using SPtr = std::shared_ptr<ModbusModelInterface>;

		ModbusModelInterface(void) {}
		virtual ~ModbusModelInterface(void) {}

		virtual bool checkType(MemoryType memoryType) = 0;
		virtual bool checkAddress(MemoryType memoryType, uint16_t startAddress, uint16_t numValues) = 0;
		virtual bool setValue(MemoryType memoryType, uint16_t startAddress, uint8_t* values, uint16_t numValues) = 0;
		virtual bool getValue(MemoryType memoryType, uint16_t startAddress, uint8_t* values, uint16_t numValues) = 0;
	};
}

#endif
