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

#ifndef __ModbusProt_WriteMultipleCoils_h__
#define __ModbusProt_WriteMultipleCoils_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class WriteMultipleCoilsReqPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteMultipleCoilsReqPDU>;

		WriteMultipleCoilsReqPDU(void);
		virtual ~WriteMultipleCoilsReqPDU(void);

		void reset(void);
		uint16_t startingAddress(void);
		void startingAddress(uint16_t startingAddress);
		uint16_t quantityOfOutputs(void);
		void quantityOfOutputs(uint16_t quantityOfRegisters);
		bool setOutputsValue(uint16_t count, uint8_t* value);
		bool setOutputsValue(uint16_t idx, bool value);
		bool getOutputsValue(uint16_t count, uint8_t* value);
		bool getOutputsValue(uint16_t idx, bool& value);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfOutputs_ = 0x00;
		uint8_t byteCount_ = 0x00;
		byte_t outputsValue_[MAX_BYTE_LEN];
	};


	class WriteMultipleCoilsResPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteMultipleCoilsResPDU>;

		WriteMultipleCoilsResPDU(void);
		virtual ~WriteMultipleCoilsResPDU(void);

		uint16_t startingAddress(void);
		void startingAddress(uint16_t startingAddress);
		uint16_t quantityOfOutputs(void);
		void quantityOfOutputs(uint16_t quantityOfOutputs);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfOutputs_ = 0x00;
	};

}

#endif
