/*
   Copyright 2025a Kai Huebl (kai@huebl-sgh.de)

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

#include <sstream>

#include "Base/LogHandler.h"

namespace Base
{

	LogHandler::LogHandler(void)
	{
	}

	LogHandler::~LogHandler(void)
	{
	}

	void
	LogHandler::logList(LogLevel logLevel, std::initializer_list<std::string> paraList)
	{
		bool firstParameter = true;
		std::stringstream ss;
		for (auto para : paraList) {
			if (firstParameter == false) ss << " ";
			firstParameter = false;
			ss << para;
		}
		log(logLevel, ss.str());
	}

}
