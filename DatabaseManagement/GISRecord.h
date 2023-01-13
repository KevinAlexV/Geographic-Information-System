#pragma once
#include <string>
#include <unordered_map>
#include "NameIndex.h"
#include "CoordinateIndex.h"

class GISRecord
{
	public:
		GISRecord();
		~GISRecord() 
		{
			delete m_coordinateIndexedDatabase;
			delete m_nameIndexedDatabase;
		}
		
		//Starting from 0, this can be treated as an int, with the Enum being equal to the int - ascending in value as you go through this list.
		enum GIS_Record_Header
		{
			FEATURE_ID = 0,
			FEATURE_NAME,
			FEATURE_CLASS,
			STATE_ALPHA,
			STATE_NUMERIC,
			COUNTY_NAME,
			COUNTY_NUMERIC,
			PRIMARY_LAT_DMS,
			PRIM_LONG_DMS,
			PRIM_LAT_DEC,
			PRIM_LONG_DEC,
			SOURCE_LAT_DMS,
			SOURCE_LONG_DMS,
			SOURCE_LAT_DEC,
			SOURCE_LONG_DEC,
			ELEV_IN_M,
			ELEV_IN_FT,
			MAP_NAME,
			DATE_CREATED,
			DATE_EDITED
		};

		struct DMS
		{
			DMS() = default;

			int degrees, minutes, seconds;
			char direction;

			std::string toString() 
			{
				return std::to_string(degrees) + "d " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s " + direction;
			}
		};
		
		//A map containing each category of feature
		std::unordered_map<std::string, std::string> featureType = 
		{
			{"School", "structure"},
			{"Tower", "structure"},
			{"Bridge", "structure"},
			{"Hospital", "structure"},
			{"Airport", "structure"},
			{"Post Office", "structure"},
			{"Building", "structure"},
			{"Levee", "structure"},
			{"Park", "structure"},
			{"Church", "structure"},
			{"Dam", "structure"},
			{"Tunnel", "structure"},
			{"Populated Place", "pop"},
			{"Rapids", "water"},
			{"Falls", "water"},
			{"Glacier", "water"},
			{"Resevoir", "water"},
			{"Sea", "water"},
			{"Gut", "water"},
			{"Harbor", "water"},
			{"Spring", "water"},
			{"Stream", "water"},
			{"Lake", "water"},
			{"Canal", "water"},
			{"Arroyo", "water"},
			{"Bay", "water"},
			{"Bend", "water"},
			{"Channel", "water"},
			{"Swamp", "water"},
			{"Well", "water"},
		};

		#pragma region Database Management
		std::string m_databaseFileName; 
		std::string databaseHeader = "FEATURE_ID|FEATURE_NAME|FEATURE_CLASS|STATE_ALPHA|STATE_NUMERIC|COUNTY_NAME|COUNTY_NUMERIC|PRIMARY_LAT_DMS|PRIM_LONG_DMS|PRIM_LAT_DEC|PRIM_LONG_DEC|SOURCE_LAT_DMS|SOURCE_LONG_DMS|SOURCE_LAT_DEC|SOURCE_LONG_DEC|ELEV_IN_M|ELEV_IN_FT|MAP_NAME|DATE_CREATED|DATE_EDITED";

		void resetLargestProbe() { m_nameIndexedDatabase->resetLargestProbe(); }
		void addRecord(std::vector<std::string> record, int line, int offset);
		void clearData();
		//Bounding box management for the world space we're interacting with.
		void setBounds(DMS minLong, DMS minLat, DMS maxLong, DMS maxLat);
		bool compareBounds(DMS longDMS, DMS latDMS);
		#pragma endregion

		#pragma region Navigate Database
		std::vector<int> getRecordOffsets(std::string featureName, std::string stateAbbr);
		std::vector<int> getRecordOffsets(float latitude, float longitude);
		std::vector<int> getRecordOffsets(float centerLatitude, float centerLongitude, float halfHeight, float halfWidth);
		int getLargestProbe() { return m_nameIndexedDatabase->getLargestProbe(); };
		#pragma endregion
		
		#pragma region Utilities
		float convertDMS(DMS dms);

		//Print coordinate indexed database
		std::string getTable()
		{ 
			std::cout << "[GIS Record] Currently indexed records by name: " << m_importedRecords << std::endl;
			return m_nameIndexedDatabase->getTable();
		}

		std::string getTree()
		{
			std::cout << "[GIS Record] Currently indexed records by coordinates: " << m_importedRecords << std::endl;
			return m_coordinateIndexedDatabase->printTree();
		}
		#pragma endregion
	private:
		DMS m_dmsMinLat = {}, m_dmsMinLong = {}, m_dmsMaxLat = {}, m_dmsMaxLong = {};
		float m_minLat = 0.f, m_minLong = 0.f, m_maxLat = 0.f, m_maxLong = 0.f;
		int m_importedRecords = 0;
		
		NameIndex* m_nameIndexedDatabase;
		CoordinateIndex* m_coordinateIndexedDatabase;
};