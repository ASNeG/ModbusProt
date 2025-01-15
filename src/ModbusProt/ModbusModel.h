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

#ifndef __ModbusProt_ModbusModel_h__
#define __ModbusProt_ModbusModel_h__

#include <memory>

namespace ModbusProt
{

	enum class MemoryType {
		Coils,
		Inputs,
		InputRegisters,
		HoldingRegisters
	};

	class MemoryArea
	{
	  public:
		using SPtr = std::shared_ptr<MemoryArea>;

		MemoryArea(
			MemoryType memoryType,
			uint16_t startAddress,
			uint16_t numValues
		);
		~MemoryArea(void);

		bool checkAddress(uint16_t address, uint16_t numValues);
		bool setValue(uint16_t address, uint8_t* value, uint16_t numValues);
		bool getValue(uint16_t address, uint8_t* value, uint16_t numValues);

	  private:
		MemoryType memoryType_ = MemoryType::Coils;
		uint16_t startAddress_ = 0;
		uint32_t numValues_ = 0;
		uint8_t valueSizeBits_ = 0;

		char* memData_ = nullptr;
		uint16_t memLen_ = 0;

		bool setValue(uint16_t address, bool value);
		bool setValue(uint16_t address, uint16_t value);
		bool getValue(uint16_t address, bool& value);
		bool getValue(uint16_t address, uint16_t& value);
	};

	class ModbusModel {
	  public:
		using SPtr = std::shared_ptr<ModbusModel>;

		ModbusModel(void);
		virtual ~ModbusModel(void);

		bool registerMemoryAreaDefaults(void);
		bool registerMemoryArea(
			MemoryType memoryType,
			uint16_t startAddress,
			uint16_t numValues
		);
		bool checkType(MemoryType memoryType);
		bool checkAddress(MemoryType memoryType, uint16_t startAddress, uint16_t numValues);
		bool setValue(MemoryType memoryType, uint16_t startAddress, uint8_t* values, uint16_t numValues);
		bool getValue(MemoryType memoryType, uint16_t startAddress, uint8_t* values, uint16_t numValues);

	  private:
		MemoryArea::SPtr memoryAreaCoils_ = nullptr;
		MemoryArea::SPtr memoryAreaInputs_ = nullptr;
		MemoryArea::SPtr memoryAreaInputRegisters_ = nullptr;
		MemoryArea::SPtr memoryAreaHoldingRegisters_ = nullptr;
	};
}

#endif
