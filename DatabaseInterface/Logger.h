#pragma once
#include <string>

class Logger
{
	public:
		Logger(std::string dbfile, std::string scriptname);
		Logger() {};
		~Logger();
		
		enum LogType
		{
			INFO,
			WARNING,
			ERROR,
			END,
			WORLD,
			IMPORT,
			DEBUG,
			WHATIS,
			WHATISAT,
			WHATISIN
		};
		
		void beginLogFile(std::string dbfile, std::string scriptname);
		void log(std::string message);
		void log(std::string message, LogType type, bool sepBegin, bool sepEnd);
		std::string logFileName;
		std::string commandSepLog = "\n------------------------------------------------------------------------------------------\n";
			
	private:
		std::string initialLog = "Course Project for COMP 8042\nStudent Name : Kevin Vilanova, Student Id : A01019400\nBegin of GIS Program log : \n";
		std::string worldLog = "Latitude / longitude values in index entries are shown as floating point values, in decimal long/lat format."+ commandSepLog + "\t\t\t\t\t\tWorld boundaries are set to:\n";
		
		std::string getTime();

		int commandNumber = 0;
};