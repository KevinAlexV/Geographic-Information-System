#pragma once
#include <string>
#include <unordered_map>
#include <regex>
#include "../DatabaseManagement/BufferPool.h"
#include "../DatabaseManagement/GISRecord.h"

class CommandProcessor
{
	public:
		void handleCommand(std::string command);
		void updateDatabase(std::string fileName);

		bool continueProcessing = true;
		
		CommandProcessor() {};
		~CommandProcessor() {};

		enum Command
		{
			world,
			import,
			quit,
			debug,
			whatis,
			whatisat,
			whatisin
		};

	private:
		
		std::unordered_map<std::string, Command> const table = 
		{
			{"world",world}, 
			{"import",import}, 
			{"quit",quit}, 
			{"debug",debug}, 
			{"whatis",whatis}, 
			{"whatisat",whatisat}, 
			{"whatisin",whatisin}
		};

		GISRecord m_database;
		BufferPool m_bufferPool;

		void processWorld(std::vector<std::string> command);
		void processImport(std::vector<std::string> command);
		void processWhatIs(std::vector<std::string> command);
		void processWhatIsAt(std::vector<std::string> command);
		void processWhatIsIn(std::vector<std::string> command);
		void processDebug(std::vector<std::string> command);
		void processQuit(std::vector<std::string> command);
};