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

#include <bit>

#include "ModbusProt/ByteOrder.h"

namespace ModbusProt
{

	uint16_t
	ByteOrder::toBig(uint16_t value)
	{
		if constexpr (std::endian::native == std::endian::big) {
			return value;
		}
		else {
			return (((value & 0xFF00) >> 8) | ((value & 0xFF) << 8));
		}
	}

	uint16_t
	ByteOrder::fromBig(uint16_t value)
	{
		if constexpr (std::endian::native == std::endian::big) {
			return value;
		}
		else {
			return (((value & 0xFF00) >> 8) | ((value & 0xFF) << 8));
		}
	}

}
