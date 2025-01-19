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

#include "ModbusProt/ModbusPDUFactory.h"

#include "ErrorPDU.h"
#include "ReadCoilsPDU.h"
#include "WriteSingleCoilPDU.h"
#include "WriteMultipleCoilsPDU.h"
#include "ReadDiscreteInputsPDU.h"
#include "ReadInputRegistersPDU.h"
#include "ReadMultipleHoldingRegistersPDU.h"

namespace ModbusProt
{
	ModbusPDU::SPtr
	ModbusPDUFactory::createModbusPDU(PDUFunction pduFunction, PDUType pduType)
	{
		if (pduType == PDUType::Error) {
			return std::make_shared<ErrorPDU>(pduFunction);
		}

		switch (static_cast<int>(pduFunction))
		{
			case static_cast<int>(PDUFunction::ReadCoils):
				if (pduType == PDUType::Request) return std::make_shared<ReadCoilsReqPDU>();
				else return std::make_shared<ReadCoilsResPDU>();
				break;
			case static_cast<int>(PDUFunction::WriteSingleCoil):
				if (pduType == PDUType::Request) return std::make_shared<WriteSingleCoilReqPDU>();
				else return std::make_shared<WriteSingleCoilResPDU>();
				break;
			case static_cast<int>(PDUFunction::WriteMultipleCoils):
				if (pduType == PDUType::Request) return std::make_shared<WriteMultipleCoilsReqPDU>();
				else return std::make_shared<WriteMultipleCoilsResPDU>();
				break;
			case static_cast<int>(PDUFunction::ReadDiscreteInputs):
				if (pduType == PDUType::Request) return std::make_shared<ReadDiscreteInputsReqPDU>();
				else return std::make_shared<ReadDiscreteInputsResPDU>();
				break;
			case static_cast<int>(PDUFunction::ReadInputRegisters):
				if (pduType == PDUType::Request) return std::make_shared<ReadInputRegistersReqPDU>();
				else return std::make_shared<ReadInputRegistersResPDU>();
				break;
			case static_cast<int>(PDUFunction::ReadMultipleHoldingRegisters):
				if (pduType == PDUType::Request) return std::make_shared<ReadMultipleHoldingRegistersReqPDU>();
				else return std::make_shared<ReadMultipleHoldingRegistersResPDU>();
				break;
			default:
				return nullptr;
		}

		return nullptr;
	}

}
