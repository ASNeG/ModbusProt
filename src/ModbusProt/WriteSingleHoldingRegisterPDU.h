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

#ifndef __ModbusProt_WriteSingleHoldingRegister_h__
#define __ModbusProt_WriteSingleHoldingRegister_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class WriteSingleHoldingRegisterReqPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteSingleHoldingRegisterReqPDU>;

		WriteSingleHoldingRegisterReqPDU(void);
		virtual ~WriteSingleHoldingRegisterReqPDU(void);

		void address(uint16_t address);
		uint16_t address(void);
		void registerValue(uint16_t value);
		uint16_t registerValue(void);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t address_ = 0x00;
		uint16_t registerValue_ = 0;
	};

	class WriteSingleHoldingRegisterResPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteSingleHoldingRegisterResPDU>;

		WriteSingleHoldingRegisterResPDU(void);
		virtual ~WriteSingleHoldingRegisterResPDU(void);

		void address(uint16_t address);
		uint16_t address(void);
		void registerValue(uint16_t value);
		uint16_t registerValue(void);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t address_ = 0x00;
		uint16_t registerValue_ = 0;
	};

}

#endif
