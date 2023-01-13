#pragma once
#include "QuadTree.h"

//Support finding ofsets of GIS records that match a given primary lat and long
//This stores GISRecords based on coordinates, and will be mapped as such for searches.
//Based on QuadTreeTrees
class CoordinateIndex
{
	public:
		void insertRecord(float latitude, float longitude, int fileOffset, int dbLine);
		std::vector<int> searchRecords(float latitude, float longitude);
		std::vector<int> searchRecords(Coordinate centralLocation, float height, float width);

		void updateBoundsOfTree(float minLat, float minLong, float maxLat, float maxLong){m_tree.setBoundry(minLat, minLong, maxLat, maxLong);}
		
		std::string printTree() { return m_tree.toString(1); }
	private:
		QuadTree m_tree;
};