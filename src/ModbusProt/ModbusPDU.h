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
	std::string pduFunctionToString(PDUFunction pduFunction);


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
		void pduType(PDUType pduType);
		std::string pduToString(void);

		virtual bool encode(std::ostream& os) const;
		virtual bool decode(std::istream& is);

	  private:
		PDUFunction pduFunction_ = PDUFunction::None;
		PDUType pduType_ = PDUType::None;
	};

}

#endif
