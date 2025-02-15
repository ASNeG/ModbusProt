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

#ifndef __ModbusProt_WriteSingleCoil_h__
#define __ModbusProt_WriteSingleCoil_h__

#include "ModbusProt/ModbusPDU.h"

namespace ModbusProt
{

	class WriteSingleCoilReqPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteSingleCoilReqPDU>;

		WriteSingleCoilReqPDU(void);
		virtual ~WriteSingleCoilReqPDU(void);

		void address(uint16_t address);
		uint16_t address(void);
		void value(bool value);
		bool value(void);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t address_ = 0x00;
		bool value_ = true;
	};

	class WriteSingleCoilResPDU
	: public ModbusPDU
	{
	  public:
		using SPtr = std::shared_ptr<WriteSingleCoilResPDU>;

		WriteSingleCoilResPDU(void);
		virtual ~WriteSingleCoilResPDU(void);

		void address(uint16_t address);
		uint16_t address(void);
		void value(bool value);
		bool value(void);

		virtual bool encode(std::ostream& os) const override;
		virtual bool decode(std::istream& is) override;

	  private:
		uint16_t address_ = 0x00;
		bool value_ = true;
	};

}

#endif
