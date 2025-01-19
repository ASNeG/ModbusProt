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

#ifndef __ModbusProt_WriteMultipleHoldingRegisters_h__
#define __ModbusProt_WriteMultipleHoldingRegisters_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class WriteMultipleHoldingRegistersReqPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteMultipleHoldingRegistersReqPDU>;

		WriteMultipleHoldingRegistersReqPDU(void);
		virtual ~WriteMultipleHoldingRegistersReqPDU(void);

		void reset(void);
		uint16_t startingAddress(void);
		void startingAddress(uint16_t startingAddress);
		uint16_t quantityOfRegisters(void);
		void quantityOfRegisters(uint16_t quantityOfRegisters);
		bool setRegistersValue(uint16_t count, uint16_t* value);
		bool setRegistersValue(uint16_t idx, uint16_t value);
		bool getRegistersValue(uint16_t count, uint16_t* value);
		bool getRegistersValue(uint16_t idx, uint16_t& value);
		uint8_t* outputsValue(void);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfRegisters_ = 0x00;
		uint8_t byteCount_ = 0x00;
		byte_t registersValue_[MAX_BYTE_LEN];
	};


	class WriteMultipleHoldingRegistersResPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteMultipleHoldingRegistersResPDU>;

		WriteMultipleHoldingRegistersResPDU(void);
		virtual ~WriteMultipleHoldingRegistersResPDU(void);

		uint16_t startingAddress(void);
		void startingAddress(uint16_t startingAddress);
		uint16_t quantityOfRegisters(void);
		void quantityOfRegisters(uint16_t quantityOfRegisters);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfRegisters_ = 0x00;
	};

}

#endif
