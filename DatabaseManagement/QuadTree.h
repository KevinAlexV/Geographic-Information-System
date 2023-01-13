#pragma once
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

#pragma region QuadTree structs
//Each Long/Lat pair is a node in the tree
struct Coordinate
{
	float longitude;
	float latitude;
	Coordinate(float _x, float _y)
	{
		longitude = _x;
		latitude = _y;
	}
	Coordinate()
	{
		longitude = 0;
		latitude = 0;
	}
};

//The individual Node based on the coordinate
struct Node
{
	Coordinate location;
	std::vector<int> fileOffset, databaseLine;
	Node() = default;
	Node(Coordinate _pos, int _fileOffset, int _dbLine)
	{
		location = _pos;
		fileOffset.push_back(_fileOffset);
		databaseLine.push_back(_dbLine);
	}
};

//Bounding Box for AABB collision checks
struct BoundingBox
{
	Coordinate centerPoint;
	Coordinate boxHalfWidth;
	Coordinate topLeft;
	Coordinate bottomRight;

	BoundingBox() = default;

	BoundingBox(const Coordinate& center, const Coordinate& halfwidths) : centerPoint(center), boxHalfWidth(halfwidths) 
	{
		topLeft = Coordinate(centerPoint.longitude - boxHalfWidth.longitude, centerPoint.latitude + boxHalfWidth.latitude);
		bottomRight = Coordinate(centerPoint.longitude + boxHalfWidth.longitude, centerPoint.latitude - boxHalfWidth.latitude);
	
	}

	BoundingBox(const Coordinate& center, const Coordinate& halfwidths, const Coordinate& tLeft, const Coordinate& bRight) : centerPoint(center), boxHalfWidth(halfwidths), topLeft(tLeft), bottomRight(bRight) {}

};
#pragma endregion

//A QuadTree, containing 4 sub-trees, and a node
class QuadTree
{
private:
	//The boundries for this node.
	Coordinate m_topLeft;
	Coordinate m_bottomRight;
	BoundingBox m_boundingBox;

	bool beingProcessed = false;

	//The Nodes for this quadrant of the tree.
	int m_maxNumberOfNodes = 4;
	std::vector<Node> m_nodesInQuad;

	// Children of this tree
	std::shared_ptr<QuadTree> m_topLeftQuad;
	std::shared_ptr<QuadTree> m_topRightQuad;
	std::shared_ptr<QuadTree> m_bottomLeftQuad;
	std::shared_ptr<QuadTree> m_bottomRightQuad;

public:
#pragma region Constructors
	QuadTree()
	{
		m_topLeft = Coordinate(0, 0);
		m_bottomRight = Coordinate(0, 0);
		
		m_boundingBox = BoundingBox(Coordinate(0, 0), Coordinate(0, 0));
	}
	QuadTree(Coordinate topLeft, Coordinate bottomRight)
	{
		m_topLeft = topLeft;
		m_bottomRight = bottomRight;
		
		float halfWidth = (bottomRight.longitude - topLeft.longitude)/2;
		float halfHeight = (topLeft.latitude - bottomRight.latitude)/2;

		//Bounding box needs the center point and the half width/height. This is where that is calculated based on the top left and bottom right.
		m_boundingBox = BoundingBox(Coordinate(m_topLeft.longitude + halfWidth, m_topLeft.latitude - halfHeight), Coordinate(halfWidth, halfHeight));
	}
#pragma endregion

	void insert(Node node);
	void setBoundry(float minLat, float minLong, float maxLat, float maxLong);

	bool inBoundary(Coordinate toBeInserted);
	bool inBoundary(BoundingBox box);
	bool inBoundary(Coordinate toBeInserted, BoundingBox box);

	void search(std::vector<Node>& results, Coordinate location);
	void searchBox(std::vector<Node>& results, BoundingBox box);
	
	std::string toString(int depth);
};