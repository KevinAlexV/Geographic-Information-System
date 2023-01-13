#include "BufferPool.h"
#pragma once
//Hold up to 15 records in memory
//Least Recently Used (LRU) algorithm is a page replacement technique used for memory management. 
//According to this method, the page which is least recently used is replaced.

#include "../DatabaseInterface/FileManagement.h"


std::vector<std::string> BufferPool::search(int key, std::string databaseFileName)
{
    std::vector<std::string> result;

    auto foundRecord = bufferedRecords.find(key);

    //If the foundRecord is not in the bufferedRecords, find it in the data base, buffer the record, then return the value.
    if (foundRecord == bufferedRecords.end())
    {
        result = FileManagement::getInstance()->importRecord(databaseFileName, key);
        buffer(key, result);
        return result;
    }

    //With the foundRecord, move it to the front of the list as it is the most recently used
    auto indexItr = foundRecord->second;
    auto record = *indexItr;
    values.erase(indexItr);
    values.push_back(record);

    bufferedRecords[key] = prev(values.end());

    //Return the found value.
    return record.record;
}

//Buffer in a new record, with the key and record. The key is the fileoffset in the database
void BufferPool::buffer(int key, std::vector<std::string> newRecord)
{
    //See if record already exists in the bufferedRecords
    auto record = bufferedRecords.find(key);

    //If it doesnt, insert the record
    if (record == bufferedRecords.end())
    {
        //If the bufferedRecords is full, remove the least recently used record
        if (values.size() == capacity)
        {
            //buffered record at the front is removed from the bufferedRecords and map
            auto bufferedRecord = values.front();
            values.pop_front();
            auto indexItr = bufferedRecords.find(bufferedRecord.key);
            bufferedRecords.erase(indexItr);
        }

        //Add record to the bufferedRecords and map
        values.push_back({ key, newRecord });
        bufferedRecords[key] = prev(values.end());

    }//Otherwise, remove that record and re-add it.
    else
    {
        //Grab the value of record, and point to it so we can erase the record from the bufferedRecords.
        auto indexItr = record->second;
        auto record = *indexItr;
        values.erase(indexItr);

        //Insert a different value where the previous one was stored.
        record.record = newRecord;
        values.push_back(record);
        bufferedRecords[key] = prev(values.end());
    }
}

bool BufferPool::full()
{
    if (values.size() >= capacity)
        return true;

    return false;
}

//LRU is on the left of the pool (front of the list), while MRU is at the right (back of the list)
std::string BufferPool::getBufferPool()
{
    std::string str = "LRU\n";

    for (auto item : values)
    {
        str += "\t" + item.toString() + "\n";
    }

    str += "MRU\n";

    return str;
}