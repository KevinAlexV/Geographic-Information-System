#include "CommandProcessor.h"
#include "FileManagement.h"
#include <iostream>
#include <vector>

//This processor utilizes REGEX for parsing commands.
//https://en.cppreference.com/w/cpp/regex
//https://en.cppreference.com/w/cpp/regex/basic_regex/constants
//https://regexr.com/
//regex_search()
//The function regex_search() is used to search for a pattern in the string that matches the regular expression, and returns the string

void CommandProcessor::updateDatabase(std::string filename)
{
	m_database.m_databaseFileName = filename;
	FileManagement::getInstance()->createNewFile(m_database.databaseHeader, filename);
}

#pragma region Database Interface

//Handle individual commands from script file.
void CommandProcessor::handleCommand(std::string command) 
{
	if (!continueProcessing)
		return;

	//std::cout << "OGCommand: " << command << std::endl;
	
	std::regex whitespace("\\s+", std::regex_constants::ECMAScript);
	
	//Replace whitespaces with delimiter
	command = std::regex_replace(command, whitespace, "|");
	
	//std::cout << "HandleCommand: " << command << std::endl;

	//Create vector to store arguments, and split command into arguments.
	std::vector<std::string> args;
	std::string delimiter = "|";
	
	int size = 0;
	while ((size = command.find(delimiter)) != std::string::npos) 
	{
		std::string arg = command.substr(0, size);
		args.push_back(arg);
		
		command.erase(0, size + delimiter.length());
	}

	args.push_back(command);

	std::string commandType = args[0];
	
	//Check if command is valid. If so, process command and pass arugments to function.
	if (FileManagement::getInstance()->regexString(commandType, FileManagement::importRegex))
	{
		processImport(args);
	}
	else if (FileManagement::getInstance()->regexString(commandType, FileManagement::worldRegex))
	{
		processWorld(args);
	}
	else if (FileManagement::getInstance()->regexString(commandType, FileManagement::whatisatRegex))
	{
		processWhatIsAt(args);
	}
	else if (FileManagement::getInstance()->regexString(commandType, FileManagement::whatisinRegex))
	{
		processWhatIsIn(args);
	}
	else if (FileManagement::getInstance()->regexString(commandType, FileManagement::whatisRegex))
	{
		processWhatIs(args);
	}
	else if (FileManagement::getInstance()->regexString(commandType, FileManagement::debugRegex))
	{
		processDebug(args);
	}
	else if (FileManagement::getInstance()->regexString(commandType, FileManagement::quitRegex))
	{
		processQuit(args);
	}
}
#pragma endregion
	
#pragma region Commands
	//Process the world command.
	void CommandProcessor::processWorld(std::vector<std::string> command)
	{
		//Look out for cases where there is a negative sign in front  ***************************************

		GISRecord::DMS westLong, eastLong, northLat, southLat;

		if (command.size() < 4)
		{
			std::cerr << "[Command Processor] Not enough arguments for world command." << std::endl;
			return;
		}

		std::cout << "[Command Processor] Initializing the bounds of the world" << std::endl;

		//This would be in DMS format, and would have two digits per Degree, minute and second. Ie; 40 26 46 N
		/*for (int i = 1; i < command.size(); i++)
		{
			std::string currentArg = command[i];

			std::smatch matches = FileManagement::getInstance()->regexMatches(currentArg, FileManagement::longLatRegex);
			
			if (!matches.empty())
			{
				std::string currentArg = matches[0];

				command[i] = currentArg;
				std::cout << command[i];
			}
		}*/

		//This would be in DMS format, and would have two digits per Degree, minute and second. Ie; 40 26 46 N
		std::string westLongStr = command[1];
		std::string eastLongStr = command[2];
		std::string southLatStr = command[3];
		std::string northLatStr = command[4];
	
		westLong = FileManagement::getInstance()->fillDMS(westLongStr);
		eastLong = FileManagement::getInstance()->fillDMS(eastLongStr);
		northLat = FileManagement::getInstance()->fillDMS(northLatStr);
		southLat = FileManagement::getInstance()->fillDMS(southLatStr);

		m_database.setBounds(westLong, southLat, eastLong, northLat);
	}

	//Process the import command.
	void CommandProcessor::processImport(std::vector<std::string> command)
	{
		//If there isnt the command keyword, and the database file in the vector of command arguments, then it is too small to be processed.
		//The first argument should be the command: command[0] = import, while command[1] = dataBaseName
		if (command.size() != 2)
		{
			std::cerr << "[Command Processor] Not enough arguments for import command." << std::endl;
			return;
		}

		std::cout << "[Command Processor] Begining import of databasefile " << command[1] << std::endl;

		//Submit database so that it can be processed, and provided filtered indexed records.
		FileManagement::getInstance()->importDatabase(command[1], false, m_database);
	}

	//Process the what is commands, formatted as "what_is	Church	VA" or "what_is	Central Church	VA"
	void CommandProcessor::processWhatIs(std::vector<std::string> command) 
	{
		//The first argument should be the command: command[0] = import, while command[1] = dataBaseName
		if (command.size() <= 2)
		{
			std::cerr << "[Command Processor] Incorrect number of arguments for what is command (should be more than 3)." << std::endl;
			return;
		}

		
		//Sort through the arguments, and combine them into a single string for searches.
		std::string stateAbb = command[command.size()-1];
		command.pop_back();
		std::string featureName = command[1];
		
		for (std::string arg : command)
		{
			if (arg != command[0] && arg != command[1])
				featureName += " " + arg;
		}
		
		std::cout << "[Command Processor] Begining search for " << featureName << " | " << stateAbb << "... ";

		//Query our loaded database with featname and stateAbb. Returns all the file offsets that match
		std::vector<int> recordOffsets = m_database.getRecordOffsets(featureName, stateAbb);
		//Record offsets stores both the line number and byte offset. Thus, the found records would be half of the array size.
		int foundRecords = recordOffsets.size() / 2;


		std::string output = "what_is\t" + featureName + "\t" + stateAbb + "\n\n";
		
		if (recordOffsets.size() != 0)
			output += "\tThe following " + std::to_string(foundRecords) + " feature(s) were found at " + featureName + ", " + stateAbb + "\n";
		else
			output += "\tNo records match \"" + featureName + "\" and \"" + stateAbb + "\"";

		std::cout << "Found " << foundRecords << " records matching the search." << std::endl;

		//For each found record, jump two indexes, as every 2 index has data about where it is found in the data base.
		for (int index = 0; index < recordOffsets.size(); index += 2)
		{
			std::cout << "[Command Processor] Printing record at offset " << recordOffsets[index] << std::endl;
			
			//Query bufferpool to obtain records of fileoffsets.
			std::vector<std::string> record = m_bufferPool.search(recordOffsets[index], m_database.m_databaseFileName);

			if (record.size() == 0)
				continue;

			std::string line = "\t" + std::to_string(recordOffsets[index + 1]) + ": (" + record[GISRecord::COUNTY_NAME] + " " + record[GISRecord::PRIMARY_LAT_DMS] + ", " + record[GISRecord::PRIM_LONG_DMS] + ")\n";
		
			output += line;
			std::cout << line;
		}
		//std::cout << std::endl << m_bufferPool.getBufferPool();
		FileManagement::getInstance()->m_log.log(output, Logger::WHATIS, false, true);
	}

	void CommandProcessor::processWhatIsAt(std::vector<std::string> command) 
	{
		//The first argument should be the command: command[0] = import, while command[1] = dataBaseName
		if (command.size() != 3)
		{
			std::cerr << "[Command Processor] Incorrect number of arguments for what is at command (should be 3)." << std::endl;
			return;
		}
		
		//Sort through the arguments, and convert the coordinates into DMS format, then decimal.
		GISRecord::DMS latitude = FileManagement::getInstance()->fillDMS(command[1]);
		GISRecord::DMS longitude = FileManagement::getInstance()->fillDMS(command[2]);
		float decLatitude = m_database.convertDMS(latitude);
		float declongitude = m_database.convertDMS(longitude);
		
		//std::cout << "[Command Processor] Begining search for " <<  (latitude.degrees) << " " << (latitude.minutes) << " " << (latitude.seconds) << latitude.direction << " : " << decLatitude << " | " << (longitude.degrees) << " " << (longitude.minutes) << " " << (longitude.seconds) << longitude.direction << " : " << declongitude << std::endl;
		std::cout << "[Command Processor] Begining search for " << latitude.toString() << ", " << longitude.toString() << "... ";

		//Query our loaded database with featname and stateAbb. Returns all the file offsets that match
		std::vector<int> recordOffsets = m_database.getRecordOffsets(decLatitude, declongitude);
		std::string output = "what_is_at\t" + command[1] + "\t" + command[2] + "\n\n";
		int foundRecords = recordOffsets.size() / 2;

		if (recordOffsets.size() != 0)
			output += "\tThe following " + std::to_string(foundRecords) + " feature(s) were found at " + latitude.toString() + ", " + longitude.toString() + "\n";
		else
			output += "\tNo records found at " + latitude.toString() + ", " + longitude.toString();

		std::cout << "Found " << foundRecords << " records matching the search." << std::endl;


		//For each found record, jump two indexes, as every 2 index has data about where it is found in the data base.
		for (int index = 0; index < recordOffsets.size(); index += 2)
		{
			//Query bufferpool to obtain records of fileoffsets.
			std::vector<std::string> record = m_bufferPool.search(recordOffsets[index], m_database.m_databaseFileName);

			if (record.size() == 0)
				continue;

			std::string line = "\t" + std::to_string(recordOffsets[index+1]) + ": (" + record[GISRecord::FEATURE_NAME] + ", " + record[GISRecord::COUNTY_NAME] + ", " + record[GISRecord::STATE_ALPHA] + ")\n";
			output += line;
			std::cout << line;
		}


		FileManagement::getInstance()->m_log.log(output, Logger::WHATISAT, false, true);

		//std::cout << std::endl << m_bufferPool.getBufferPool();
	}
	
	void CommandProcessor::processWhatIsIn(std::vector<std::string> command)
	{
		bool hasFilter = command.size() == 7;
		bool hasLong = command.size() == 6;
		bool isStandardCommand = command.size() == 5;

		//what_is_in - filter	water	382850N	0793030W	120	240. Last values contain the height then width
		//Long filter displays a longer version of the record found.
		if (!hasFilter && !isStandardCommand && !hasLong)
		{
			std::cerr << "[Command Processor] Incorrect number of arguments for what is in command (should be 7, 6 or 5). Was " << command.size() << std::endl;
			for(std::string st : command)
				std::cout << st << std::endl;

			return;
		}
		
		//Sort through the arguments, and convert the coordinates into DMS format, then decimal.
		GISRecord::DMS latitude = FileManagement::getInstance()->fillDMS(command[command.size() - 4]);
		GISRecord::DMS longitude = FileManagement::getInstance()->fillDMS(command[command.size() - 3]);
		float decLatitude = m_database.convertDMS(latitude);
		float declongitude = m_database.convertDMS(longitude);

		//Convert seconds format of height and width to decimal.
		float height = (std::stoi(command[command.size() - 2]) / 3600.f);
		float width = (std::stoi(command[command.size() - 1]) / 3600.f);
		
		std::cout << "[Command Processor] Begining search around " << latitude.toString() << ", " << longitude.toString() << "... ";

		//Query our loaded database with featname and stateAbb. Returns all the file offsets that match
		std::vector<int> recordOffsets = m_database.getRecordOffsets(decLatitude, declongitude, height, width);
		int foundRecords = recordOffsets.size() / 2;


		std::cout << "Found " << foundRecords << " records matching the search." << std::endl;
		

		std::string output;
		
		for (std::string arg : command)
			output += "\t" + arg;

		//The following 7 feature(s) were found in (38d 28m 12s North +/- 60, 79d 31m 56s West +/- 90)
		if (recordOffsets.size() != 0)
			output += "\n\n\tThe following " + std::to_string(foundRecords) + " feature(s) were found in " + latitude.toString() + " +/- " + command[command.size() - 2] + ", " + longitude.toString() + " +/- " + command[command.size() - 1] + "\n\n";
		else
			output += "\n\tNo records found in " + latitude.toString() + " +/- " + command[command.size() - 2] + ", " + longitude.toString() + " +/- " + command[command.size() - 1] + "\n\n";;





		std::string filter;
		int features = 0;
		if (hasFilter) 
			filter = command[2];
		
		//For each found record, jump two indexes, as every 2 index has data about where it is found in the data base.
		for (int index = 0; index < recordOffsets.size(); index += 2)
		{
			//Query bufferpool to obtain records of fileoffsets.
			std::vector<std::string> record = m_bufferPool.search(recordOffsets[index], m_database.m_databaseFileName);
			std::string line;

			if (record.size() == 0)
				continue;

			//What Is in contains a filter, reults should be further filtered
			if (hasFilter && !filter.empty())
			{
				std::string recordClassType;

				if (!record[GISRecord::FEATURE_CLASS].empty() && m_database.featureType.contains(record[GISRecord::FEATURE_CLASS]))
				{
					recordClassType = m_database.featureType[record[GISRecord::FEATURE_CLASS]];

					if (recordClassType == filter)
					{
						line = "\t" + std::to_string(recordOffsets[index + 1]) + ": " + record[GISRecord::FEATURE_NAME] + ", " + record[GISRecord::FEATURE_CLASS] + ", " + record[GISRecord::STATE_ALPHA] + ", (" + record[GISRecord::PRIMARY_LAT_DMS] + ", " + record[GISRecord::PRIM_LONG_DMS] + ")\n";
						std::cout << line;
						output += line;
						features++;
					}
				}

			}
			else if (hasLong)
			{
				std::string longOutput;
				longOutput += "\tFeature ID\t:" + record[GISRecord::FEATURE_ID] + "\n";
				longOutput += "\tFeature Name\t:" + record[GISRecord::FEATURE_NAME] + "\n";
				longOutput += "\tFeature Cat\t:" + record[GISRecord::FEATURE_CLASS] + "\n";
				longOutput += "\tState\t:" + record[GISRecord::STATE_ALPHA] + "\n";
				longOutput += "\tCounty\t:" + record[GISRecord::COUNTY_NAME] + "\n";
				longOutput += "\tLongitude\t:" + record[GISRecord::PRIM_LONG_DMS] + "\n";
				longOutput += "\tLatitude\t:" + record[GISRecord::PRIMARY_LAT_DMS] + "\n";
				longOutput += "\tElev in ft\t:" + record[GISRecord::ELEV_IN_FT] + "\n";
				longOutput += "\tUSGS Quad\t:" + record[GISRecord::MAP_NAME] + "\n";
				longOutput += "\tData created\t:" + record[GISRecord::DATE_CREATED] + "\n\n";

				std::cout << std::endl << longOutput << std::endl;
				output += longOutput;
			}
			//What is in does not contain a filter, all results should be returned
			else if (isStandardCommand)
			{
				std::string line = "\t" + std::to_string(recordOffsets[index + 1]) + ": " + record[GISRecord::FEATURE_NAME] + ", " + record[GISRecord::STATE_ALPHA] + ", (" + record[GISRecord::PRIMARY_LAT_DMS] + ", " + record[GISRecord::PRIM_LONG_DMS] + ")\n";
				output += line;
				std::cout << line;

			}
		}

		if (hasFilter)
			output += "\n\n\tThere were " + std::to_string(features) + " feature(s) of type " + filter;

		std::cout << std::endl;

		FileManagement::getInstance()->m_log.log(output, Logger::WHATISIN, false, true);
	}

	void CommandProcessor::processDebug(std::vector<std::string> command) 
	{
		//command (debug) datastructure (quad, hash, etc)
		if (command.size() != 2)
		{
			std::cerr << "[Command Processor] Incorrect number of arguments for debug command (should be 2)." << std::endl;
			return;
		}
		
		if (command[1] == "quad")
		{
			FileManagement::getInstance()->m_log.log("debug\tquad\n\n" + m_database.getTree(), Logger::DEBUG, false, true);
		}
		else if (command[1] == "hash")
		{
			FileManagement::getInstance()->m_log.log("debug\thash\n\n" + m_database.getTable(), Logger::DEBUG, false, true);
		}
		else if (command[1] == "pool")
		{
			FileManagement::getInstance()->m_log.log("debug\tpool\n\n" + m_bufferPool.getBufferPool(), Logger::DEBUG, false, true);
		}
		else if (command[1] == "world")
		{
			//m_database.featureType.printTable();
		}
	
	}
	
	void CommandProcessor::processQuit(std::vector<std::string> command) 
	{
		continueProcessing = false;

		FileManagement::getInstance()->m_log.log("quit\n\nTerminating execution of commands.", Logger::END, false, true);
	
	}
#pragma endregion