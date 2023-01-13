#include "GISRecord.h"
#include "../DatabaseInterface/FileManagement.h"
#include <iostream>

#pragma region Database Management
GISRecord::GISRecord()
{
	m_nameIndexedDatabase = new NameIndex();
	m_coordinateIndexedDatabase = new CoordinateIndex();
}

void GISRecord::clearData()
{
	delete m_nameIndexedDatabase;
	delete m_coordinateIndexedDatabase;

	m_nameIndexedDatabase = new NameIndex();
	m_coordinateIndexedDatabase = new CoordinateIndex();
}

//Add record to both coordinate Index database and name index database, which basically indexes one record in two different ways to simplify searching.
void GISRecord::addRecord(std::vector<std::string> record, int line, int offset)
{
	//std::cout << "[GIS Record] Adding record to database" << std::endl;

	//Convert DMS to decimal
	DMS latitude = FileManagement::getInstance()->fillDMS(record[PRIMARY_LAT_DMS]);
	DMS longitude = FileManagement::getInstance()->fillDMS(record[PRIM_LONG_DMS]);

	float latDec = convertDMS(latitude);
	float longDec = convertDMS(longitude);

	m_coordinateIndexedDatabase->insertRecord(latDec, longDec, offset, line);

	m_nameIndexedDatabase->insertRecord(record[FEATURE_NAME], record[STATE_ALPHA], offset, line);
	
	m_importedRecords++;
}

//Hold database DMS records in memory and export to database
void GISRecord::setBounds(DMS _minLong, DMS _minLat, DMS _maxLong, DMS _maxLat)
{
	m_dmsMinLong = _minLong;
	m_dmsMinLat = _minLat;
	m_dmsMaxLong = _maxLong;
	m_dmsMaxLat = _maxLat;

	m_minLong = convertDMS(_minLong);
	m_minLat = convertDMS(_minLat);
	m_maxLong = convertDMS(_maxLong);
	m_maxLat = convertDMS(_maxLat);
	
	std::string logLine = "\t\t\t\t\t\t\t\t\t" + std::to_string(m_maxLat) + "\n\t\t\t\t\t\t" + std::to_string(m_minLong) + "\t\t\t" + std::to_string(m_maxLong) + "\n\t\t\t\t\t\t\t\t\t" + std::to_string(m_minLat);

	FileManagement::getInstance()->m_log.log(logLine, Logger::WORLD, true, true);

	m_coordinateIndexedDatabase->updateBoundsOfTree(m_minLat, m_minLong, m_maxLat, m_maxLong);
}
#pragma endregion

#pragma region Utility
bool GISRecord::compareBounds(DMS longDMS, DMS latDMS)
{
	float longDec = convertDMS(longDMS);
	float latDec = convertDMS(latDMS);

	//std::cout << "[GIS Record] Bounds being compares: | Long: " << longDec << "| Lat, " << latDec << "| Left Long, " << m_minLong << "| Down Lat, " << m_minLat << "| Right Long, " << m_maxLong << "| Up Lat, " << m_maxLat << "|" << std::endl;

	if (longDec >= m_minLong && longDec <= m_maxLong && latDec >= m_minLat && latDec <= m_maxLat)
		return true;
	else
		return false;
}

float GISRecord::convertDMS(DMS dms)
{
	float decimalVersion = dms.degrees;
	decimalVersion += dms.minutes / 60.f;
	decimalVersion += dms.seconds / 3600.f;

	if (dms.direction == 'N' || dms.direction == 'E')
		return decimalVersion;
	else
		return decimalVersion * -1;
}
#pragma endregion

#pragma region Search
//Used in What is command
std::vector<int> GISRecord::getRecordOffsets(std::string featureName, std::string stateAbbr)
{
	//std::cout << "\n\nDatabase: " <<  m_nameIndexedDatabase->getTable() << "\n\n";
	
	//Search name index database for feature name
	return m_nameIndexedDatabase->searchRecords(featureName, stateAbbr);
}

std::vector<int> GISRecord::getRecordOffsets(float latitude, float longitude) 
{
	//Search coordinate index database for long/lat
	return m_coordinateIndexedDatabase->searchRecords(latitude, longitude);

}
std::vector<int> GISRecord::getRecordOffsets(float centerLatitude, float centerLongitude, float halfHeight, float halfWidth) 
{
	Coordinate center(centerLongitude, centerLatitude);

	//Search coordinate index database for coords in bounding box
	return m_coordinateIndexedDatabase->searchRecords(center, halfHeight, halfWidth);

}
#pragma endregion