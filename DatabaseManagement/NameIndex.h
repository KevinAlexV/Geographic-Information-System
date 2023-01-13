#pragma once
#include "HashTable.h"

/*
int x = 4;
^^^^    ^^^
lvalue  rvalue

std::move(x);
^^^^^^^^^^^
rvalue
*/
//This stores GISRecords based on names, and will be mapped as such for searches.
//Based on hash tables
//This will allow us to index records based on the name of the feature, and the state it is in. 
//During search functions, the record MUST match the name and state of the record being searched for. If only half match, the record will not be returned.
class NameIndex
{
	
	public:
		void insertRecord(std::string featureName, std::string stateName, int fileOffset, int dbLine);
		
		std::string getTable() { return m_table.getTable(); };

		int getLargestProbe() { return m_table.probeSeq; }
		void resetLargestProbe() { m_table.probeSeq = 0; }

		std::vector<int> searchRecords(std::string featureName, std::string stateName);
	private:
		//Create a hash table to contain strings, utilizing the GISRecordHash functions. Create 2 GISRecordHash tables, for the cuckoo hash system.
		HashTable<Record, GISRecordHash> m_table{ 1024 };
};
