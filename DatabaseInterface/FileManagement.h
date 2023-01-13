#pragma once
#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include "../DatabaseInterface/CommandProcessor.h"
#include "../DatabaseManagement/GISRecord.h"
#include "../DatabaseInterface/Logger.h"
#pragma warning(disable:4996)

class FileManagement
{
	public:
		#pragma region Singleton Instance Management
		//Singletons should not be cloneable, this is to prevent clones.
		FileManagement(FileManagement& other) = delete;

		//Singletons should not be assignable, this is to prevent that.
		void operator=(const FileManagement&) = delete;

		//This retrieves a pointer to the current instance of FileManagement. If it doesn't exist, then one will be created and returned.
		static FileManagement* getInstance();
		#pragma endregion
		
		enum RegexType
		{
			importRegex,
			worldRegex,
			debugRegex,
			quitRegex,
			whatisRegex,
			whatisatRegex,
			whatisinRegex,
			longLatRegex
		};

		Logger m_log;
		CommandProcessor m_commandProcessor;

		std::vector<std::string> dataPaths{ "..\\Data", "..\\", "Data" };
		
		std::vector<std::string> getLines(std::string filename, bool log = false);
		std::string getLine(std::string filepath, int line);

		inline int convertStringToInt(std::string str){return std::stoi(str);}
		
		bool regexString(std::string toCompare, RegexType regex);
		std::smatch regexMatches(std::string toMatch, RegexType regex);

		void createNewFile(std::string message, std::string filename);
		void exportLines(std::string lines);
		void exportLine(std::string line, std::string filename);
		void exportToDatabase(std::string line, std::string databaseName);
		GISRecord::DMS fillDMS(std::string value);

		std::vector<std::string> splitString(std::string line, char delimiter);
		std::vector<std::string> importRecord(std::string filepath, int lineNum);
		void importDatabase(std::string filePath, bool log, GISRecord& database);
		void processScript(std::string filePath, bool log);

		void updateDatabase(std::string filename)
		{
			m_commandProcessor.updateDatabase(filename);
		}
	protected:
		FileManagement() 
		{
			m_importRegex = std::regex("^[Ii][Mm][Pp][Oo][Rr][Tt].*$", std::regex_constants::ECMAScript);
			m_worldRegex = std::regex("^[Ww][Oo][Rr][Ll][Dd].*$", std::regex_constants::ECMAScript);
			m_debugRegex = std::regex("^[Dd][Ee][Bb][Uu][Gg].*$", std::regex_constants::ECMAScript);
			m_quitRegex = std::regex("^[Qq][Uu][Ii][Tt].*$", std::regex_constants::ECMAScript);
			m_whatisRegex = std::regex("^[Ww][Hh][Aa][Tt]_[Ii][Ss].*$", std::regex_constants::ECMAScript);
			m_whatisatRegex = std::regex("^[Ww][Hh][Aa][Tt]_[Ii][Ss]_[Aa][Tt].*$", std::regex_constants::ECMAScript);
			m_whatisinRegex = std::regex("^[Ww][Hh][Aa][Tt]_[Ii][Ss]_[Ii][Nn].*$", std::regex_constants::ECMAScript);
			m_longLatRegex = std::regex("(\\d{1,2}\\d\\d\\d\\d\\d[wWnNeEsS])", std::regex_constants::ECMAScript);
		};
		~FileManagement() 
		{
			m_databaseFileStream.close();
		};
	private:
		static FileManagement* m_pinstance;
		char m_databaseDelimiter = '|';
		int importedLines = 1;

		std::ofstream m_databaseFileStream;

		std::regex m_importRegex;
		std::regex m_worldRegex;
		std::regex m_debugRegex;
		std::regex m_quitRegex;
		std::regex m_whatisRegex;
		std::regex m_whatisatRegex;
		std::regex m_whatisinRegex;
		std::regex m_longLatRegex;

		//Make inline function so disassembled code does not have extra overhead of calling the function, as it's incredibly small and will be used quite frequently
		//Small optimization suggestion to the compiler. Not a keyword command, and might be ignored depending on the compiler.
		inline bool doesFileExist(const std::string& name)
		{
			if (FILE* file = fopen(name.c_str(), "r")) 
			{
				fclose(file);
				return true;
			}
			else 
			{
				return false;
			}
		}

		std::string validatePath(std::string filePath);
};