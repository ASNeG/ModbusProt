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

#ifndef __ModbusProt_ReadInputRegisters_h__
#define __ModbusProt_ReadInputRegisters_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class ReadInputRegistersReqPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<ReadInputRegistersReqPDU>;

		ReadInputRegistersReqPDU(void);
		virtual ~ReadInputRegistersReqPDU(void);

		void startingAddress(uint16_t startingAddress);
		uint16_t startingAddress(void);
		void quantityOfInputs(uint16_t quantityOfInputs);
		uint16_t quantityOfInputs(void);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t startingAddress_ = 0x00;
		uint16_t quantityOfInputs_ = 0x00;
	};


	class ReadInputRegistersResPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<ReadInputRegistersResPDU>;

		ReadInputRegistersResPDU(void);
		virtual ~ReadInputRegistersResPDU(void);

		void reset(void);
		uint8_t byteCount(void);
		bool setInputRegisters(uint16_t count, uint16_t* value);
		bool setInputRegisters(uint16_t idx, uint16_t value);
		bool getInputRegisters(uint16_t count, uint16_t* value);
		bool getInputRegisters(uint16_t idx, uint16_t& value);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint8_t byteCount_ = 0x00;
		uint16_t inputRegisters_[MAX_BYTE_LEN/2];

		bool value_ = true;
	};

}

#endif
