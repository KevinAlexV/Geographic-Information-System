#include "CoordinateIndex.h"

void CoordinateIndex::insertRecord(float latitude, float longitude, int fileOffset, int line)
{
	Node node;
	node.location.latitude = latitude;
	node.location.longitude = longitude;
	node.fileOffset.push_back(fileOffset);
	node.databaseLine.push_back(line);

 	m_tree.insert(node);
}

std::vector<int> CoordinateIndex::searchRecords(float latitude, float longitude)
{
	std::vector<Node> nodes;
	Coordinate coordinate;
	coordinate.latitude = latitude;
	coordinate.longitude = longitude;

	m_tree.search(nodes, coordinate);

	std::vector<int> fileOffsets;

	for (auto& node : nodes)
	{
		std::string output = std::to_string(node.location.latitude) + ", " + std::to_string(node.location.longitude) + ": " + std::to_string(node.fileOffset.size()) + " records found.\n";
		std::cout << output;

		int index = 0;
		
		for (index = 0; index < node.fileOffset.size(); index++)
		{
			fileOffsets.push_back(node.fileOffset[index]);
			fileOffsets.push_back(node.databaseLine[index]);
		}

	}

	return fileOffsets;
}


std::vector<int> CoordinateIndex::searchRecords(Coordinate centralLocation, float height, float width) 
{
	std::vector<int> results;
	std::vector<Node> resultsNode;

	Coordinate halfWidthHeight(width, height);
	BoundingBox box(centralLocation, halfWidthHeight);

	m_tree.searchBox(resultsNode, box);

	for (Node node : resultsNode) 
	{
		for(int index = 0; index < node.fileOffset.size(); index ++)
		{
			results.push_back(node.fileOffset[index]);
			results.push_back(node.databaseLine[index]);
		}
	}

	return results;
}