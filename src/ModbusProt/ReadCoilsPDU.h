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

#ifndef __ModbusProt_ReadCoils_h__
#define __ModbusProt_ReadCoils_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class ReadCoilsReqPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<ReadCoilsReqPDU>;

		ReadCoilsReqPDU(void);
		virtual ~ReadCoilsReqPDU(void);

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


	class ReadCoilsRes
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<ReadCoilsRes>;

		ReadCoilsRes(void);
		virtual ~ReadCoilsRes(void);

		void reset(void);
		uint8_t byteCount(void);
		bool setCoilStatus(uint16_t  idx, bool value);
		bool getCoilStatus(uint16_t idx, bool& value);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint8_t byteCount_ = 0x00;
		uint8_t coilStatus_[MAX_BYTE_LEN];

		bool value_ = true;
	};

}

#endif
