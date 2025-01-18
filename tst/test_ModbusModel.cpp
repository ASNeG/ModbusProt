
#include <cpunit>
#include <cstring>
#include <iostream>

#include "ModbusProt/ModbusModel.h"

namespace TestModbusModel
{
	using namespace cpunit;

    CPUNIT_TEST(TestModbusModel, coil)
	{
    	ModbusProt::ModbusModel modbusModel;
    	CPUNIT_ASSERT(modbusModel.registerMemoryAreaDefaults() == true);

    	uint8_t data[3] = {0b10011001, 0b10011011, 0b10011001};
    	CPUNIT_ASSERT(modbusModel.setValue(ModbusProt::MemoryType::Coils, 1, data, 24) == true);

    	memset((char*)&data, 0x00, 3);
    	CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::Coils, 9, data, 24) == true);
    	CPUNIT_ASSERT(data[0] == 0b10011011);
    	CPUNIT_ASSERT(data[1] == 0b10011001);
    	CPUNIT_ASSERT(data[2] == 0b00000000);

    	memset((char*)&data, 0x00, 3);
        CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::Coils, 2, data, 24) == true);
        CPUNIT_ASSERT(data[0] == 0b11001100);
        CPUNIT_ASSERT(data[1] == 0b11001101);
        CPUNIT_ASSERT(data[2] == 0b01001100);

    	memset((char*)&data, 0x00, 3);
        CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::Coils, 4, data, 9) == true);
        CPUNIT_ASSERT(data[0] == 0b01110011);
        CPUNIT_ASSERT(data[1] == 0b00000001);
	}

    CPUNIT_TEST(TestModbusModel, inputs)
	{
    	ModbusProt::ModbusModel modbusModel;
    	CPUNIT_ASSERT(modbusModel.registerMemoryAreaDefaults() == true);

    	uint8_t data[3] = {0b10011001, 0b10011011, 0b10011001};
    	CPUNIT_ASSERT(modbusModel.setValue(ModbusProt::MemoryType::Inputs, 1, data, 24) == true);

    	memset((char*)&data, 0x00, 3);
    	CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::Inputs, 9, data, 24) == true);
    	CPUNIT_ASSERT(data[0] == 0b10011011);
    	CPUNIT_ASSERT(data[1] == 0b10011001);
    	CPUNIT_ASSERT(data[2] == 0b00000000);

    	memset((char*)&data, 0x00, 3);
        CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::Inputs, 2, data, 24) == true);
        CPUNIT_ASSERT(data[0] == 0b11001100);
        CPUNIT_ASSERT(data[1] == 0b11001101);
        CPUNIT_ASSERT(data[2] == 0b01001100);
	}

    CPUNIT_TEST(TestModbusModel, inputRegisters)
	{
    	ModbusProt::ModbusModel modbusModel;
    	CPUNIT_ASSERT(modbusModel.registerMemoryAreaDefaults() == true);

    	uint16_t data[500];
    	for (uint16_t idx = 0; idx < 500; idx++) {
    		data[idx] = idx;
    	}
    	CPUNIT_ASSERT(modbusModel.setValue(ModbusProt::MemoryType::InputRegisters, 1, (uint8_t*)data, 500) == true);

    	memset((char*)&data, 0x00, 1000);
    	CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::InputRegisters, 9, (uint8_t*)data, 500) == true);
    	for (uint16_t idx = 0; idx < 500-8; idx++) {
    		CPUNIT_ASSERT(data[idx] == idx+8);
    	}
    	CPUNIT_ASSERT(data[500-8] == 0);

    	memset((char*)&data, 0x00, 1000);
        CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::InputRegisters, 2, (uint8_t*)data, 500) == true);
    	for (uint16_t idx = 0; idx < 500-1; idx++) {
    		CPUNIT_ASSERT(data[idx] == idx+1);
    	}
    	CPUNIT_ASSERT(data[500-1] == 0);
	}

    CPUNIT_TEST(TestModbusModel, holdingRegisters)
	{
    	ModbusProt::ModbusModel modbusModel;
    	CPUNIT_ASSERT(modbusModel.registerMemoryAreaDefaults() == true);

    	uint16_t data[500];
    	for (uint16_t idx = 0; idx < 500; idx++) {
    		data[idx] = idx;
    	}
    	CPUNIT_ASSERT(modbusModel.setValue(ModbusProt::MemoryType::HoldingRegisters, 1, (uint8_t*)data, 500) == true);

    	memset((char*)&data, 0x00, 1000);
    	CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::HoldingRegisters, 9, (uint8_t*)data, 500) == true);
    	for (uint16_t idx = 0; idx < 500-8; idx++) {
    		CPUNIT_ASSERT(data[idx] == idx+8);
    	}
    	CPUNIT_ASSERT(data[500-8] == 0);

    	memset((char*)&data, 0x00, 1000);
        CPUNIT_ASSERT(modbusModel.getValue(ModbusProt::MemoryType::HoldingRegisters, 2, (uint8_t*)data, 500) == true);
    	for (uint16_t idx = 0; idx < 500-1; idx++) {
    		CPUNIT_ASSERT(data[idx] == idx+1);
    	}
    	CPUNIT_ASSERT(data[500-1] == 0);
	}

}
