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

#ifndef __ModbusTCP_TCPServerModel_h__
#define __ModbusTCP_TCPServerModel_h__

#include <memory>
#include <map>

#include "ModbusProt/ModbusModelInterface.h"
#include "ModbusTCP/TCPServerHandler.h"

namespace ModbusTCP
{

	class TCPServerModel
	: public TCPServerHandler
	{
	  public:
		using SPtr = std::shared_ptr<TCPServerModel>;
		using Map = std::map<uint32_t, SPtr>;

		TCPServerModel(
			asio::io_context& ctx
		);
		TCPServerModel(
			void
		);
		~TCPServerModel(
			void
		);

		void addModbusModel(
			ModbusProt::ModbusModelInterface::SPtr modbusModel
		);

		bool handleModbusReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		) override;

	  private:

		ModbusProt::ModbusModelInterface::SPtr modbusModel_ = nullptr;

		bool handleReadCoilsReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleWriteSingleCoilReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleWriteMultipleCoilsReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleReadDiscreteInputsReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleReadInputRegistersReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleReadMultipleHoldingRegistersReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleWriteSingleHoldingRegisterReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
		bool handleWriteMultipleHoldingRegistersReq(
			uint8_t unitIdentifier,
			ModbusProt::ModbusPDU::SPtr& req,
			ModbusProt::ModbusPDU::SPtr& res
		);
	};


}

#endif
