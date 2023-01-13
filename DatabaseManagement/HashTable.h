#pragma once
#include <cstdlib>
#include <cstddef>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>

//Since each GIS record occupies one line in the file, we can use the line number as the key for the hash table.
struct Record
{
    bool operator==(const Record& other) const
    {
        return featureName == other.featureName && stateName == other.stateName;
    }

    std::string featureName;
    std::string stateName;
    std::vector<int> databaseLine;
    std::vector<int> fileOffsets;
    int hashTablePosition;

    std::string toString()
    {
        std::string output = featureName + ":" + stateName;

        for (int line : databaseLine)
            output += ", [" + std::to_string(line) + "]";

        return output;
    }
};

//The HashFamily GISRecord Hash
class GISRecordHash
{
public:
    GISRecordHash(){}
	
    int hash(const Record& x) const
    {
        int hashVal = 0;

        std::string d = x.featureName + x.stateName;
		
        for (char character : d)
        {
            hashVal = 37 * hashVal + ((int)character);
        }

        return hashVal;
    }
};


template<typename T, typename HashFamily>
class HashTable
{
private:
    struct HashEntry
    {
        T element;
        bool isActive = false;
        HashEntry(const T& e = T(), bool a = false) : element{ e }, isActive{ a } {}
        HashEntry(T&& e, bool a = false) : element{ std::move(e) }, isActive{ a } {}
    };

    std::vector<HashEntry> array;
    int currentSize;
    int rehashes;
    int numOfHashes;
    int probingLimit;
    HashFamily hashFunctions;

    static const int ALLOWED_REHASHES = 5;
    float MAX_LOAD = 0.70f;

    /**
 * Internal method to test if a positive number is prime.
 * Not an efficient algorithm.
 */
    bool isPrime(int n)
    {
        if (n == 2 || n == 3)
            return true;

        if (n == 1 || n % 2 == 0)
            return false;

        for (int i = 3; i * i <= n; i += 2)
            if (n % i == 0)
                return false;

        return true;
    }

    /**
     * Internal method to return a prime number at least as large as n.
     * Assumes n > 0.
     */
    int nextPrime(int n)
    {
        if (n % 2 == 0)
            ++n;

        for (; !isPrime(n); n += 2);

        return n;
    }

    bool isActive(int currentPos) const { return currentPos != -1 && array[currentPos].isActive; }
    void expand() { rehash(static_cast<int>(array.size() * 2)); }

    void rehash()
    {
        hashFunctions.generateNewFunctions();
        rehash(array.size());
    }

    void rehash(int newSize)
    {
        std::vector<HashEntry> oldArray = array;

        // Create new double-sized, empty table
        array.resize(nextPrime(newSize));
        for (auto& entry : array)
            entry.isActive = false;

        // Copy table over
        currentSize = 0;
        for (auto& entry : oldArray)
            if (entry.isActive)
                insert(entry.element);
    }

    // Method that search all hash function places
    int findPos(const T& x)
    {
		int probe = (numOfHashes * numOfHashes + numOfHashes) / 2;

        bool isEmpty = false;
		
            
        while (!isEmpty)
        {
            int pos = myhash(x) + probe;

            numOfHashes++;

            if (pos >= array.size())
            {
                int numOfArraySizes = pos / array.size();
                pos -= (array.size() * numOfArraySizes);
            }
            auto tocompare = array[pos].element;

            if (!isActive(pos))
                isEmpty = true;
            if (isActive(pos) && tocompare == x)
            {
                return pos;
            }

            probe = (numOfHashes * numOfHashes + numOfHashes) / 2;
        }

        numOfHashes = 0;

        return -1;
    }

    int myhash(const T& x) const 
    {
        int size = array.size();
        int hashedval = hashFunctions.hash(x);
        int totalHash = hashedval % array.size();

        return totalHash; 
    }
public:

    int probeSeq = 0;
	
    std::vector<Record> searchRecords(std::string featureName, std::string stateName)
    {
        std::vector<Record> records;

        Record record;
        record.featureName = featureName;
        record.stateName = stateName;
	
        bool isEmpty = false;

        int probe = (numOfHashes * numOfHashes + numOfHashes) / 2;

		while (!isEmpty)
		{ 
            
            int pos = myhash(record) + probe;

			numOfHashes++;

			if (pos >= array.size())
			{
				int numOfArraySizes = pos / array.size();
				pos -= (array.size() * numOfArraySizes);
			}

			auto tocompare = array[pos].element;

			if (!isActive(pos))
				isEmpty = true;
			if (isActive(pos) && tocompare == record)
			{
				records.push_back(array[pos].element);
			}

			probe = (numOfHashes * numOfHashes + numOfHashes) / 2;
		}

        numOfHashes = 0;
		
        return records;
    }

    explicit HashTable(int size = 101) : array(size)
    {
        rehashes = 0;
        currentSize = 0;
        numOfHashes = 0;
        probingLimit = 500;
        makeEmpty();
    }

    bool contains(const T& x) { return findPos(x) != -1; }

    int size() const { return currentSize; }

    int capacity() const { return array.size(); }

    void makeEmpty()
    {
        currentSize = 0;
        for (auto& entry : array)
            entry.isActive = false;
    }

    bool insert(T& x)
    {
        int numOfProbes = 0;
		
        const int COUNT_LIMIT = 100;

        if (currentSize >= array.size() * MAX_LOAD)
            expand();

        int foundXPos = findPos(x);

        //If hash table already contains X, return false
        if (foundXPos != -1)
        {
            std::vector<int> fileOffsets = array[foundXPos].element.fileOffsets;

            for (int offset : fileOffsets)
            {
                if (offset == x.fileOffsets[0])
                    return false;
            }
            array[foundXPos].element.fileOffsets.push_back(x.fileOffsets[0]);
            array[foundXPos].element.fileOffsets.push_back(x.databaseLine[0]);
            
            return true;
        }

        int probe = (numOfHashes * numOfHashes + numOfHashes) / 2;
		
        while (true)
        {
            int lastPos = -1;
            int pos;
			
            pos = myhash(x) + probe;

            if (pos >= array.size()) 
            {
                int numOfArraySizes = pos / array.size();
                pos -= (array.size() * numOfArraySizes);
            }

            //Successfully hashed! Insert it into the array
            if (!isActive(pos))
            {
                x.hashTablePosition = pos;
                array[pos] = HashEntry{ x, true };
                currentSize++;
                numOfHashes = 0;
                if(numOfProbes > probeSeq)
                    probeSeq = numOfProbes;
                return true;
            }
					
            numOfHashes++;
            probe = (numOfHashes * numOfHashes + numOfHashes) / 2;
            numOfProbes++;
        }
        numOfHashes = 0;
        return false;
    }

    std::string printOffsets(std::vector<int> offsets)
    {
        std::string results = "";
        for (int offset : offsets)
        {
            results += std::to_string(offset) + ", ";
        }
        return results;
    }

    std::string getTable()
    {
        std::ostringstream os;
		os << "Format of display is" << std::endl << "Slot numer: data record" << std::endl << "Current table size is " << array.size() << std::endl << "Number of elements in table is " << currentSize << std::endl << std::endl;
        for (auto& content : array) 
        {
            if (content.isActive)
                os << "\t" << content.element.hashTablePosition << ": [" << content.element.toString() << "]" << std::endl;
        }
        return os.str();
    }
};