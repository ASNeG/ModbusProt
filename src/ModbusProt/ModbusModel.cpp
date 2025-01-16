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
#include <iostream>

#include "ModbusProt/ModbusModel.h"

namespace ModbusProt
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// MemoryArea
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	MemoryArea::MemoryArea(
		MemoryType memoryType,
		uint16_t startAddress,
		uint16_t numValues
	)
	{
		memoryType_ = memoryType;
		startAddress_ = startAddress;
		numValues_ = numValues;

		if (memoryType_ == MemoryType::Coils || memoryType == MemoryType::Inputs) {
			valueSizeBits_ = 1;
		}
		else {
			valueSizeBits_ = 16;
		}

		memLen_ = ((numValues_ * valueSizeBits_) + 7) / 8;
		memData_ = (char*)malloc(memLen_);
		memset(memData_, 0x00, memLen_);
	}

	MemoryArea::~MemoryArea(void)
	{
		if (memData_ != nullptr) {
			delete memData_;
		}

		memData_ = nullptr;
		memLen_ = 0;
	}

	bool
	MemoryArea::checkAddress(uint16_t address, uint16_t numValues)
	{
		if (address < startAddress_  || address + numValues >= startAddress_ + numValues_) return false;
		return true;
	}

	bool
	MemoryArea::setValue(uint16_t address, uint8_t* value, uint16_t numValues)
	{
		if (address < startAddress_  || address + numValues >= startAddress_ + numValues_) return false;

		if (valueSizeBits_ == 1) {
			for (uint16_t idx = 0; idx < numValues; idx++) {
				uint16_t offset = idx / 8;
				uint8_t rest = idx % 8;

				bool boolVal = (value[offset] & (1 << rest)) != 0;
				setValue(address+idx, boolVal);
			}
		}
		else {
			uint16_t* val = (uint16_t*)value;
			for (uint16_t idx = 0; idx < numValues; idx++) {
				setValue(address+idx, val[idx]);
			}
		}

		return true;
	}

	bool
	MemoryArea::getValue(uint16_t address, uint8_t* value, uint16_t numValues)
	{
		if (address < startAddress_  || address + numValues >= startAddress_ + numValues_) return false;

		if (valueSizeBits_ == 1) {
			for (uint16_t idx = 0; idx < numValues; idx++) {
				uint16_t offset = idx / 8;
				uint8_t rest = idx % 8;

				bool boolVal = 0;
				getValue(address+idx, boolVal);

				if (boolVal == true) {
					value[offset] |= (1 << rest);
				}
				else {
					value[offset] &= (~(1 << rest));
				}
			}
		}
		else {
			uint16_t* val = (uint16_t*)value;
			for (uint16_t idx = 0; idx < numValues; idx++) {
				getValue(address+idx, val[idx]);
			}
		}

		return true;
	}

	bool
	MemoryArea::setValue(uint16_t address, bool value)
	{
		uint16_t offset = (address - startAddress_) / 8;
		uint8_t rest = (address - startAddress_) % 8;

		if (value) {
			memData_[offset] = memData_[offset] | (1 << rest);
		}
		else {
			memData_[offset] = memData_[offset] & ~(1 << rest);
		}

		return true;
	}

	bool
	MemoryArea::setValue(uint16_t address, uint16_t value)
	{
		uint16_t offset = (address - startAddress_);
		((uint16_t*)memData_)[offset] = value;
		return true;
	}

	bool
	MemoryArea::getValue(uint16_t address, bool& value)
	{
		uint16_t offset = (address - startAddress_) / 8;
		uint8_t rest = (address - startAddress_) % 8;

		if ((memData_[offset] & (1 << rest)) != 0) {
			value = 1;
		}
		else {
			value = 0;
		}

		return true;
	}

	bool
	MemoryArea::getValue(uint16_t address, uint16_t& value)
	{
		uint16_t offset = (address - startAddress_);
		value = ((uint16_t*)memData_)[offset];
		return true;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ModbusModel
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ModbusModel::ModbusModel(void)
	{
	}

	ModbusModel::~ModbusModel(void)
	{
	}

	bool
	ModbusModel::registerMemoryAreaDefaults(void)
	{
		if (!registerMemoryArea(MemoryType::Coils, 1, 9999)) {
			return false;
		}
		if (!registerMemoryArea(MemoryType::Inputs, 10001, 19999)) {
			return false;
		}
		if (!registerMemoryArea(MemoryType::InputRegisters, 30001, 39999)) {
			return false;
		}
		if (!registerMemoryArea(MemoryType::HoldingRegisters, 40001, 49999)) {
			return false;
		}

		return true;
	}

	bool
	ModbusModel::registerMemoryArea(
		MemoryType memoryType,
		uint16_t startAddress,
		uint16_t numValues
	)
	{
		// Create memory area
		auto memoryArea = std::make_shared<MemoryArea>(memoryType, startAddress, numValues);

		// Register memory area
		switch (memoryType) {
			case MemoryType::Coils:
			{
				memoryAreaCoils_ = memoryArea;
				break;
			}
			case MemoryType::Inputs:
			{
				memoryAreaInputs_ = memoryArea;
				break;
			}
			case MemoryType::InputRegisters:
			{
				memoryAreaInputRegisters_ = memoryArea;
				break;
			}
			case MemoryType::HoldingRegisters:
			{
				memoryAreaHoldingRegisters_ = memoryArea;
				break;
			}
			default:
			{
				return false;
			}
		}

		return true;
	}

	bool
	ModbusModel::checkType(MemoryType memoryType)
	{
		switch (memoryType) {
			case MemoryType::Coils:
			{
				if (memoryAreaCoils_ == nullptr) return false;
				break;
			}
			case MemoryType::Inputs:
			{
				if (memoryAreaInputs_ == nullptr) return false;
				break;
			}
			case MemoryType::InputRegisters:
			{
				if (memoryAreaInputRegisters_ == nullptr) return false;
				break;
			}
			case MemoryType::HoldingRegisters:
			{
				if (memoryAreaHoldingRegisters_ == nullptr) return false;
				break;
			}
			default:
			{
				return false;
			}
		}
		return true;
	}

	bool
	ModbusModel::checkAddress(MemoryType memoryType, uint16_t startAddress, uint16_t numValues)
	{
		switch (memoryType) {
			case MemoryType::Coils:
			{
				if (memoryAreaCoils_ == nullptr) return false;
				return memoryAreaCoils_->checkAddress(startAddress, numValues);
				break;
			}
			case MemoryType::Inputs:
			{
				if (memoryAreaInputs_ == nullptr) return false;
				return memoryAreaInputs_->checkAddress(startAddress, numValues);
				break;
			}
			case MemoryType::InputRegisters:
			{
				if (memoryAreaInputRegisters_ == nullptr) return false;
				return memoryAreaInputRegisters_->checkAddress(startAddress, numValues);
				break;
			}
			case MemoryType::HoldingRegisters:
			{
				if (memoryAreaHoldingRegisters_ == nullptr) return false;
				return memoryAreaHoldingRegisters_->checkAddress(startAddress, numValues);
				break;
			}
			default:
			{
				return false;
			}
		}
		return true;
	}

	bool
	ModbusModel::setValue(MemoryType memoryType, uint16_t startAddress, uint8_t* values, uint16_t numValues)
	{
		switch (memoryType) {
			case MemoryType::Coils:
			{
				if (memoryAreaCoils_ == nullptr) return false;
				return memoryAreaCoils_->setValue(startAddress, values, numValues);
				break;
			}
			case MemoryType::Inputs:
			{
				if (memoryAreaInputs_ == nullptr) return false;
				return memoryAreaInputs_->setValue(startAddress, values, numValues);
				break;
			}
			case MemoryType::InputRegisters:
			{
				if (memoryAreaInputRegisters_ == nullptr) return false;
				return memoryAreaInputRegisters_->setValue(startAddress, values, numValues);
				break;
			}
			case MemoryType::HoldingRegisters:
			{
				if (memoryAreaHoldingRegisters_ == nullptr) return false;
				return memoryAreaHoldingRegisters_->setValue(startAddress, values, numValues);
				break;
			}
			default:
			{
				return false;
			}
		}
		return true;
	}

	bool
	ModbusModel::getValue(MemoryType memoryType, uint16_t startAddress, uint8_t* values, uint16_t numValues)
	{
		switch (memoryType) {
			case MemoryType::Coils:
			{
				if (memoryAreaCoils_ == nullptr) return false;
				return memoryAreaCoils_->getValue(startAddress, values, numValues);
				break;
			}
			case MemoryType::Inputs:
			{
				if (memoryAreaInputs_ == nullptr) return false;
				return memoryAreaInputs_->getValue(startAddress, values, numValues);
				break;
			}
			case MemoryType::InputRegisters:
			{
				if (memoryAreaInputRegisters_ == nullptr) return false;
				return memoryAreaInputRegisters_->getValue(startAddress, values, numValues);
				break;
			}
			case MemoryType::HoldingRegisters:
			{
				if (memoryAreaHoldingRegisters_ == nullptr) return false;
				return memoryAreaHoldingRegisters_->getValue(startAddress, values, numValues);
				break;
			}
			default:
			{
				return false;
			}
		}
		return true;
	}

}
