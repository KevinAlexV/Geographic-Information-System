#include "DatabaseInterface/CommandProcessor.h"
#include "DatabaseInterface/FileManagement.h"
#include "DatabaseInterface/Logger.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>


//Import new GIS records into database file
//Retrieve all GIS records from database file, with specific criteria (geographic coords, feature name and state, within a retangular geographic region)

//Input should be database file name, command script file name, and log file name.
int main(int argc, char** argv)
{
	//Database file    command script file    log file

	std::cout << "# of Arguments being passed" << std::endl;

	if (argc != 4)
	{
		std::cerr << "Please specify 3 arguments: database file name, script file name, and log file name.";
		return 0;
	}
	
	
	FileManagement::getInstance()->m_log.logFileName = argv[3];
	FileManagement::getInstance()->m_log.beginLogFile(argv[1], argv[2]);

	FileManagement::getInstance()->processScript(argv[2], true);

	return 0;
}