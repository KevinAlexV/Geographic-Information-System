#include "Logger.h"
#include "FileManagement.h"
#include <iomanip>
#include <ctime>
#include <sstream>
#pragma warning(disable:4996)

Logger::Logger(std::string dbfile, std::string scriptname)
{

	beginLogFile(dbfile, scriptname);
}

Logger::~Logger() 
{
	
}

std::string Logger::getTime() 
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%a %b %d %H:%M:%S") << " PDT " << std::put_time(&tm, "%Y");
	std::string time = oss.str();

	return time;

}

void Logger::beginLogFile(std::string dbfile, std::string scriptname)
{
	std::string time = getTime();
	std::string log = initialLog + "dbFile: " + dbfile + "\n" + "script: " + scriptname + "\n" + "log: " + logFileName + "\n" + "Start Time: " + time + "\n";

	FileManagement::getInstance()->updateDatabase(dbfile);
	FileManagement::getInstance()->createNewFile(log, logFileName);
}

void Logger::log(std::string message) 
{
	FileManagement::getInstance()->exportLine(message, logFileName);
}

void Logger::log(std::string message, LogType type, bool sepBegin, bool sepEnd)
{
	std::string log;
	
	if(sepBegin)
		log += commandSepLog;

	switch (type)
	{
	case INFO:
		log += "INFO: " + message;
		break;
	case WARNING:
		log += "WARNING: " + message;
		break;
	case ERROR:
		log += "ERROR: " + message;
		break;
	case WORLD:
		log += worldLog + message;
		break;
	case END:
		commandNumber++;
		log += std::to_string(commandNumber) + ": " + message + commandSepLog + "\nEnd time: " + getTime();
		break;
	case IMPORT:
		commandNumber++;
		log += "Command " + std::to_string(commandNumber) + ": " + message;
		break;
	case DEBUG:
		commandNumber++;
		log += "Command " + std::to_string(commandNumber) + ": " + message;
		break;
	case WHATIS:
		commandNumber++;
		log += "Command " + std::to_string(commandNumber) + ": " + message;
		break;
	case WHATISAT:
		commandNumber++;
		log += "Command " + std::to_string(commandNumber) + ": " + message;
		break;
	case WHATISIN:
		commandNumber++;
		log += "Command " + std::to_string(commandNumber) + ": " + message;
		break;
	}

	if (sepEnd)
		log += commandSepLog;

	FileManagement::getInstance()->exportLine(log, logFileName);
}