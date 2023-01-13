#include "FileManagement.h"
#include <iostream>
#include <sstream>
#include <limits>

#pragma region Singleton Instance Management
FileManagement* FileManagement::m_pinstance{ nullptr };

//Return the instance of file management. If one does not exist, create it, and return the pointer.
FileManagement* FileManagement::getInstance()
{
	if (m_pinstance == nullptr)
	{
		m_pinstance = new FileManagement();
	}

	return m_pinstance;
}

#pragma endregion

#pragma region Utility Functions

bool FileManagement::regexString(std::string toCompare, RegexType regex) 
{
	switch (regex)
	{
		case importRegex:
			return std::regex_match(toCompare, m_importRegex);
		case worldRegex:
			return std::regex_match(toCompare, m_worldRegex);
		case debugRegex:
			return std::regex_match(toCompare, m_debugRegex);
		case quitRegex:
			return std::regex_match(toCompare, m_quitRegex);
		case whatisRegex:
			return std::regex_match(toCompare, m_whatisRegex);
		case whatisatRegex:
			return std::regex_match(toCompare, m_whatisatRegex);
		case whatisinRegex:
			return std::regex_match(toCompare, m_whatisinRegex);
		case longLatRegex:
			return std::regex_match(toCompare, m_longLatRegex);
		default:
			return false;
	}
}

std::smatch FileManagement::regexMatches(std::string toMatch, RegexType regex) 
{
	std::smatch matches;

	switch (regex)
	{
		case importRegex:
		{
			std::regex_search(toMatch, matches, m_importRegex);
		}
		case worldRegex:
		{
			std::regex_search(toMatch, matches, m_worldRegex);
		}
		case debugRegex:
		{
			std::regex_search(toMatch, matches, m_debugRegex);
		}
		case quitRegex:
		{
			std::regex_search(toMatch, matches, m_quitRegex);
		}
		case whatisRegex:
		{
			std::regex_search(toMatch, matches, m_whatisRegex);
		}
		case whatisatRegex:
		{
			std::regex_search(toMatch, matches, m_whatisatRegex);
		}
		case whatisinRegex:
		{
			std::regex_search(toMatch, matches, m_whatisinRegex);
		}
		case longLatRegex:
		{
			std::regex_search(toMatch, matches, m_longLatRegex);
		}
	}
	
	return matches;

}

//Fill DMS struct with values from string, and return DMS.
GISRecord::DMS FileManagement::fillDMS(std::string value)
{
	GISRecord::DMS dms;

	int degreeSize;
	int minSecSize = 2;
	int charSize = 1;

	//std::cout << "[File Management] Filling DMS struct with value: " << value << "... ";

	if (value.length() == 8)
	{
		//std::cout << value.length() << " characters are within the value, with 3 digits for degrees: " << value << std::endl;

		//if (value.substr(0, 7).find("N") != NULL && value.substr(0, 7).find("S") != NULL)
			//std::cout << "Value has N or S within the first 7 characters. \n" << value.substr(0, 7) << "\n\n";

		degreeSize = 3;

		dms.degrees = std::stoi(value.substr(0, degreeSize));
		dms.minutes = std::stoi(value.substr(3, minSecSize));
		dms.seconds = std::stoi(value.substr(5, minSecSize));
		dms.direction = (char)((value.substr(7, charSize))[0]);

		//std::cout << "Successfully converted!";
	}
	else if (value.length() == 7)
	{

		//std::cout << "Value has two digits for degrees: " << value << std::endl;

	//	if (value.substr(0, 6).find("N") != NULL && value.substr(0, 6).find("S") != NULL)
			//std::cout << "Value has N or S within the first 6 characters.\n" << value.substr(0, 6) << "\n\n";

		//std::cout << "Value has two digits for degrees. ";
		degreeSize = 2;

		dms.degrees = std::stoi(value.substr(0, degreeSize));
		dms.minutes = std::stoi(value.substr(2, minSecSize));
		dms.seconds = std::stoi(value.substr(4, minSecSize));
		dms.direction = (char)((value.substr(6, charSize))[0]);
		//std::cout << "Successfully converted!";
	}
	else
	{
		std::cout << "[File Management] Invalid DMS value: " << value << std::endl;

	}

	return dms;
}

//Splits a string based on a provided char delimiter. Returns vector with each split string.
std::vector<std::string> FileManagement::splitString(std::string line, char delimiter)
{
	std::vector<std::string> result;

	//std::cout << "[File Management] Splitting line: " << line << "\nDelimiter: " << delimiter << "... ";

	if (line.empty() || isblank(delimiter))
		return result;

	if (line[line.size() - 1] == delimiter)
		line.pop_back();

	std::string tmp;
	std::stringstream ss(line);

	while (getline(ss, tmp, delimiter))
	{
		result.push_back(tmp);
	}

	//std::cout << "Successfully parsed.\n\n";

	return result;
}

//Validate if the path of a file exists
std::string FileManagement::validatePath(std::string filePath)
{
	std::string fileName;

	//If the provided filepath does not exist, strip down the string to just the file name, and check stored data paths in order to find the file.
	if (!doesFileExist(filePath))
	{
		int forwardBracketPos = filePath.find_last_of('/');
		int backBracketPos = filePath.find_last_of('\\');

		if (forwardBracketPos != std::string::npos || backBracketPos != std::string::npos)
		{
			int higherVal = forwardBracketPos > backBracketPos ? forwardBracketPos : backBracketPos;

			fileName = filePath.substr(higherVal + 1);
		}
		else
		{
			fileName = filePath;
		}

		//Check all locations under datapaths, which contains the possible locations for scripts and DB files used in this program
		for (std::string location : dataPaths)
		{
			filePath = location + "\\" + fileName;

			//std::cout << "" << filePath << std::endl;

			if (doesFileExist(filePath))
			{
				std::cout << "[File Management] " << fileName << " is a valid file. " << filePath << std::endl;
				break;
			}
		}
	}

	return filePath;
}
#pragma endregion

std::vector<std::string> FileManagement::getLines(std::string filePath, bool log)
{
	std::string filename = validatePath(filePath);

	std::vector<std::string> doc;
	std::ifstream infile(filename);
	std::string line;

	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			if(log)
				m_log.log(line);

			if (line[0] != ';')
				doc.push_back(line);
		}
		infile.close();
	}

	return doc;
}

void FileManagement::processScript(std::string filePath, bool log)
{
	std::string filename = validatePath(filePath);

	std::vector<std::string> doc;
	std::ifstream infile(filename);
	std::string line;

	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			if (!m_commandProcessor.continueProcessing)
				break;

			if (log && line[0] == ';')
				m_log.log(line);

			if (line[0] != ';')
				m_commandProcessor.handleCommand(line);
		}
		infile.close();
	}
}

std::string FileManagement::getLine(std::string filepath, int lineNum) 
{
	std::string filename = validatePath(filepath);
	std::ifstream file(filepath);
	std::string line;
	
	if (file.is_open())
	{
		//for (int i = 1; i <= lineNum; i++)
		//Moves read head to the line number specified
		file.seekg(lineNum);
		
		std::getline(file, line);
		
		file.close();
	}


	return line;
}

std::vector<std::string> FileManagement::importRecord(std::string filepath, int lineNum)
{
	std::string line = getLine(filepath, lineNum);
	
	std::vector<std::string> processedLine = splitString(line, m_databaseDelimiter);

	return processedLine;
}

void FileManagement::importDatabase(std::string filePath, bool log, GISRecord& database)
{
	std::string filename = validatePath(filePath);

	std::vector<std::vector<std::string>> doc;
	std::ifstream infile(filename);
	std::string line;
	
	database.resetLargestProbe();
	int currentLine = 0;
	int importedLinesFromThisFile = 0;

	m_log.log("import\t" + filename, Logger::IMPORT, false, false);

	//If file is open, process each line from database and determine if it should be stored in the new, filtered database.
	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			currentLine++;

			if (currentLine == 1)
				continue;

			if (log)
				m_log.log(line);
			
			std::vector<std::string> processedLine = splitString(line, m_databaseDelimiter);

			std::string primLatDMS = processedLine[GISRecord::PRIMARY_LAT_DMS];
			std::string primLongDMS = processedLine[GISRecord::PRIM_LONG_DMS];

			//Convert the DMS of the vector to the DMS struct in GISRecord to compare to the world bounds.
			if (!primLatDMS.empty() && !primLatDMS.empty() && regexString(primLatDMS, longLatRegex) && regexString(primLongDMS, longLatRegex))
			{
				GISRecord::DMS dbLat = fillDMS(primLatDMS);
				GISRecord::DMS dbLong = fillDMS(primLongDMS);

				//filter database line here based on specifically need values and whether it falls in the bounds of the world coords.
				if (database.compareBounds(dbLong, dbLat))
				{
					importedLinesFromThisFile++;
					importedLines++;
					
					int offset = m_databaseFileStream.tellp();

					exportToDatabase(line, database.m_databaseFileName);
					//If the line is valid, add it to the database.
					database.addRecord(processedLine, importedLines, offset);
					//std::cout << "[File Management] Line " << currentLine << ":\n" << line << "\nWithin the bounds of the world.\n" << std::endl;
				}
			}
			
		}
		infile.close();
	}


	m_log.log("Imported Records: " + std::to_string(importedLinesFromThisFile) + "\nLongest probe sequence: " + std::to_string(database.getLargestProbe()) + m_log.commandSepLog);
	

	std::cout << "[File Management] " << filePath << " has been imported.\n\n" << std::endl;
}


void FileManagement::exportLines(std::string lines)
{

}


void FileManagement::exportToDatabase(std::string line, std::string databaseName)
{
	if(!m_databaseFileStream.is_open())
		m_databaseFileStream.open(databaseName, std::ios_base::app);

	m_databaseFileStream << line << "\n";

	m_databaseFileStream.flush();

}

void FileManagement::createNewFile(std::string line, std::string filename)
{
	// output file
	std::ofstream out(filename, std::ofstream::trunc);

	out << line << "\n";
	out.close();
}

//Export one line to a file.
void FileManagement::exportLine(std::string line, std::string filename)
{
	// output file
	std::ofstream out;

	out.open(filename, std::ios_base::app);

	out << line << "\n";
	out.close();
}