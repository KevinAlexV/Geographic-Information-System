#pragma once
//Hold up to 15 records in memory
//Least Recently Used (LRU) algorithm is a page replacement technique used for memory management. 
//According to this method, the page which is least recently used is replaced.

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <string>

class BufferPool
{
public:
    struct BufferedRecord
    {
        int key;
        std::vector<std::string> record;

        std::string toString()
        {
			
            std::string s = std::to_string(key) + ": ";
			
            if (record.size() != 0)
                s += record[0];

			for (int index = 1; index < record.size(); index++)
			{
				s += "|" + record[index];
			}
            
            return s;
        }
    };
	
private:
    std::list<BufferedRecord> values;
    std::unordered_map<int, std::list<BufferedRecord>::iterator> bufferedRecords;
    int capacity;

public:
    BufferPool(int capacity = 15) : capacity(capacity) {}

    std::vector<std::string> search(int key, std::string databaseFileName);

    //Buffer in a new record, with the key and record. The key is the fileoffset in the database
    void buffer(int key, std::vector<std::string> newRecord);

    bool full();

	//LRU is on the left of the pool (front of the list), while MRU is at the right (back of the list)
    std::string getBufferPool();
};
