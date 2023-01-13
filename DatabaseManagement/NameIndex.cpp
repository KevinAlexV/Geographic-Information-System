#include "NameIndex.h"

void NameIndex::insertRecord(std::string featureName, std::string stateName, int fileOffset, int dbLine)
{
	Record record;
	record.featureName = featureName;
	record.stateName = stateName;
	record.fileOffsets.push_back(fileOffset);
	record.databaseLine.push_back(dbLine);

	m_table.insert(record);
}


std::vector<int> NameIndex::searchRecords(std::string featureName, std::string stateName)
{
	std::vector<Record> records = m_table.searchRecords(featureName, stateName);

	std::vector<int> fileOffsets;
	
	for (auto& record : records)
	{
		std::string output = record.featureName + ", " + record.stateName + ": " + std::to_string(record.fileOffsets.size()) + " records found.\n";
		std::cout << output;

		int index = 0;

		for (index = 0; index < record.fileOffsets.size(); index++)
		{
			fileOffsets.push_back(record.fileOffsets[index]);
			fileOffsets.push_back(record.databaseLine[index]);
		}
	}

	return fileOffsets;

}