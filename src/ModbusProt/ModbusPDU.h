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

#ifndef __ModbusProt_ModbusPDU_h__
#define __ModbusProt_ModbusPDU_h__

#include <iostream>
#include <stdint.h>
#include <map>
#include <memory>

#include "ModbusProt/Config.h"

namespace ModbusProt
{

	using byte_t = unsigned char;

	enum class PDUFunction {
		None,

		//
		// Data Access
		//
		// Bit access - Physical Discrete Inputs
		ReadDiscreteInputs,						// 0x02
		// Bit access - Internal Bits or Physical Coils
		ReadCoils,								// 0x01
		WriteSingleCoil,						// 05
		WriteMultipleCoils,						// 15
		// 16-bit access - Physical Input Registers
		ReadInputRegisters,						// 04
		// 16-bit access - Internal Registers or Physical Output Registers
		ReadMultipleHoldingRegisters,			// 03
		WriteSingleHoldingRegister,				// 06
		WriteMultipleHoldingRegisters,			// 16
		ReadWriteMultipleRegisters,				// 23
		MaskWriteRegister,						// 22
		ReadFifoQueue,							// 24
		// File Record Access
		ReadFileRecord,							// 20
		WriteFileRecord,						// 21

		//
		// Diagnostic
		//
		ReadAxceptionStatus,					// 7
		Diagnostic,								// 8
		GetComEventCounter,						// 11
		GetComEventLog,							// 12
		ReportSlaveID,							// 17
		ReadDeviceIdentification,				// 43

		//
		// Other
		//
		EncapsulatedInterfaceTransport			// 43
	};


	enum class PDUType {
		None,
		Request,
		Response,
		Error
	};

	class ModbusPDU {
	  public:
		using FuncCodeMap = std::map<PDUFunction, uint8_t>;
		using SPtr = std::shared_ptr<ModbusPDU>;

		ModbusPDU(PDUFunction pduFunction, PDUType pduType);
		virtual ~ModbusPDU(void);

		static FuncCodeMap funcCodeMap_;

		PDUFunction pduFunction(void);
		PDUType pduType(void);

		virtual bool encode(std::ostream& os) const;
		virtual bool decode(std::istream& is);

	  private:
		PDUFunction pduFunction_ = PDUFunction::None;
		PDUType pduType_ = PDUType::None;
	};
#if 0
	class ReadDiscreteInputsReq {
	  public:
		uint8_t functionCode_ = 0x02;
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfInputs_ = 0x00;
	};

	class ReadDiscreteInputRes {
	  public:
		uint8_t byteCount_ = 0x00;
		byte_t inputStatus_[MAX_BYTE_LEN];
	};

	class ReadMultipleHoldingRegistersReq {
	  public:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfInputs_ = 0x00;
	};

	class ReadMultipleHoldingRegistersRes {
	  public:
		uint8_t byteCount_ = 0x00;
		byte_t registerValue_[MAX_BYTE_LEN];
	};

	class ReadInputRegistersReq {
	  public:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfInputRegisters_ = 0x00;
	};

	class ReadMultipleInputRegistersRes {
	  public:
		uint8_t byteCount_ = 0x00;
		byte_t inputRegisters_[MAX_BYTE_LEN];
	};

	class WriteSingleCoilReq {
	  public:
		uint16_t outputAddress_ = 0x00;
		uint16_t outputValue_ = 0x00;
	};

	class WriteSingleCoilRes {
	  public:
		uint16_t ouputAddress_ =  0x00;
		uint16_t outputValue_ = 0x00;
	};

	class WriteSingleHoldingReq {
	  public:
		uint16_t registerAddress_ = 0x00;
		uint16_t registerValue_ = 0x00;
	};

	class WriteSingleHoldingRes {
	  public:
		uint16_t registerAddress_ =  0x00;
		uint16_t registerValue_ = 0x00;
	};

	class WriteMultiCoilsReq {
	  public:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfOutputs_ = 0x00;
		uint8_t byteCount_ = 0x00;
		byte_t outputsValue_[MAX_BYTE_LEN];
	};

	class WriteMultiCoilsRes {
	  public:
		uint8_t functionCode_ = 0x0F;
		uint16_t startingAddress_ =  0x00;
		uint16_t quantityOfOutputs_ = 0x00;
	};

	class WriteMultiHoldingRegistersReq {
	  public:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfRegisters_ = 0x00;
		uint8_t byteCount_ = 0x00;
		byte_t registersValue_[MAX_BYTE_LEN];
	};

	class WriteMultiHoldingRegistersRes {
	  public:
		uint16_t startingAddress_ =  0x00;
		uint16_t quantityOfRegisters_ = 0x00;
	};
#endif

}

#endif
