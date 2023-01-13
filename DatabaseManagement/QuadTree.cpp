#include "QuadTree.h"
#include <cstdlib>

//Insert a node into the QuadTree
void QuadTree::insert(Node node)
{
	// Current QuadTree cannot contain it
	if (!inBoundary(node.location))
		return;

	//If the nodes in quad is greater than the max number of nodes, then we need to split it. Otherwise, insert.
	if (m_nodesInQuad.size() < m_maxNumberOfNodes && m_topLeftQuad == nullptr && m_topRightQuad == nullptr && m_bottomLeftQuad == nullptr && m_bottomRightQuad == nullptr)
	{
		for (Node& quadnode : m_nodesInQuad)
		{
			if (quadnode.location.latitude == node.location.latitude && quadnode.location.longitude == node.location.longitude)
			{
				for (int index = 0; index < node.fileOffset.size(); index++)
				{
					quadnode.fileOffset.push_back(node.fileOffset[index]);
					quadnode.databaseLine.push_back(node.databaseLine[index]);
				}
				return;
			}
		}

		m_nodesInQuad.push_back(node);
		return;
	}

	//If the absolute value of topleft - bottomright is less than 1, then we are within an area of a quad that can be subdivided.
	//if (abs(m_topLeft.longitude - m_bottomRight.longitude) <= 1 && abs(m_topLeft.latitude - m_bottomRight.latitude) <= 1)
	//{
	float longitudeHalf = (m_topLeft.longitude + m_bottomRight.longitude) / 2;
	float latitudeHalf = (m_topLeft.latitude + m_bottomRight.latitude) / 2;

	//If the node is within the left side of the boundry, insert it within the quad tree of the left side depending on location. Used to be longLatHalf
	if (node.location.longitude <= longitudeHalf)
	{
		// Indicates topLeftTree
		if (node.location.latitude >= latitudeHalf)
		{
			if (m_topLeftQuad == nullptr)
				m_topLeftQuad = std::shared_ptr<QuadTree>(new QuadTree(
					Coordinate(m_topLeft.longitude, m_topLeft.latitude),
					Coordinate(longitudeHalf, latitudeHalf)));

			m_topLeftQuad->insert(node);
		}
		// Indicates botLeftTree
		else
		{
			if (m_bottomLeftQuad == nullptr)
				m_bottomLeftQuad = std::shared_ptr<QuadTree>(new QuadTree(
					Coordinate(m_topLeft.longitude, latitudeHalf),
					Coordinate(longitudeHalf, m_bottomRight.latitude)));

			m_bottomLeftQuad->insert(node);
		}
	}//Insert the node on the right side of the quad
	else
	{
		// Indicates topRightTree
		if (node.location.latitude >= latitudeHalf)
		{
			if (m_topRightQuad == nullptr)
				m_topRightQuad = std::shared_ptr<QuadTree>(new QuadTree(
					Coordinate(longitudeHalf, m_topLeft.latitude),
					Coordinate(m_bottomRight.longitude, latitudeHalf)));

			m_topRightQuad->insert(node);
		}
		// Indicates botRightTree
		else
		{
			if (m_bottomRightQuad == nullptr)
				m_bottomRightQuad = std::shared_ptr<QuadTree>(new QuadTree(
					Coordinate(longitudeHalf, latitudeHalf),
					Coordinate(m_bottomRight.longitude, m_bottomRight.latitude)));

			m_bottomRightQuad->insert(node);
		}
	}

	if (!m_nodesInQuad.empty() && !beingProcessed)
	{
		beingProcessed = true;
		//We can't copy the unique pointers in m_nodesInQuad, so instead we make a reference to it.
		for (Node quadnode : m_nodesInQuad)
		{
			this->insert(quadnode);
		}

		m_nodesInQuad.clear();

		beingProcessed = false;
	}
}

void QuadTree::setBoundry(float minLat, float minLong, float maxLat, float maxLong)
{
	m_topLeft.latitude = maxLat;
	m_topLeft.longitude = minLong;
	m_bottomRight.latitude = minLat;
	m_bottomRight.longitude = maxLong;
	
	float halfWidth = (m_bottomRight.longitude - m_topLeft.longitude) / 2;
	float halfHeight = (m_topLeft.latitude - m_bottomRight.latitude) / 2;

	//Bounding box needs the center point and the half width/height. This is where that is calculated based on the top left and bottom right.
	m_boundingBox = BoundingBox(Coordinate(m_topLeft.longitude + halfWidth, m_topLeft.latitude - halfHeight), Coordinate(halfWidth, halfHeight));

}

//Search for one node in the tree.
void QuadTree::search(std::vector<Node>& results, Coordinate location)
{
	// Current QuadTree cannot contain it
	if (!inBoundary(location))
		return;

	for (Node node : m_nodesInQuad)
	{
		if (node.location.latitude == location.latitude && node.location.longitude == location.longitude)
			results.push_back(node);
	}

	float longitudeHalf = (m_topLeft.longitude + m_bottomRight.longitude) / 2;
	float latitudeHalf = (m_topLeft.latitude + m_bottomRight.latitude) / 2;

	if (location.longitude <= longitudeHalf)
	{
		//topLeft
		if (location.latitude >= latitudeHalf)
		{
			if (m_topLeftQuad != nullptr)
				m_topLeftQuad->search(results, location);
		}
		//bottomLeft
		else
		{
			if (m_bottomLeftQuad != nullptr)
				m_bottomLeftQuad->search(results, location);
		}
	}
	else
	{
		//topRight
		if (location.latitude >= latitudeHalf)
		{
			if (m_topRightQuad != nullptr)
				m_topRightQuad->search(results, location);
		}
		// Indicates botRightTree
		else
		{
			if (m_bottomRightQuad != nullptr)
				m_bottomRightQuad->search(results, location);
		}
	}
};


//Search for nodes in tree that are within a bounding box.
void QuadTree::searchBox(std::vector<Node>& results, BoundingBox box)
{
	//If else, begin search within the tree
	if (m_topLeftQuad != NULL && (m_topLeftQuad->inBoundary(box)))
	{
		m_topLeftQuad->searchBox(results, box);
	}

	if (m_topRightQuad != NULL && (m_topRightQuad->inBoundary(box)))
	{
		m_topRightQuad->searchBox(results, box);
	}

	if (m_bottomLeftQuad != NULL && (m_bottomLeftQuad->inBoundary(box)))
	{
		m_bottomLeftQuad->searchBox(results, box);
	}

	if (m_bottomRightQuad != NULL && (m_bottomRightQuad->inBoundary(box)))
	{
		m_bottomRightQuad->searchBox(results, box);
	}

	for (Node node : m_nodesInQuad)
	{
		// If node is in bounding box boundries, insert into results
		if (inBoundary(node.location, box))
			results.push_back(node);
	}
}

//@ sign indicates a node, and will split between each quad in the node (0-K). If a node has another quads, @ will indicate the begining of that node. * indicates an empty child node.
std::string QuadTree::toString(int depth)
{
	std::string indent(depth, '\t');
	std::string output = "";
	if (this->m_topLeftQuad != nullptr || this->m_topRightQuad != nullptr || this->m_bottomLeftQuad != nullptr || this->m_bottomRightQuad != nullptr)
	{
		if (m_topLeftQuad != nullptr)
		{
			output += m_topLeftQuad->toString(depth + 1) + "\n";
		}
		else
			output += indent + "\t*\n";

		if (m_topRightQuad != nullptr)
		{
			output += m_topRightQuad->toString(depth + 1) + "\n";
		}
		else
			output += indent + "\t*\n";

		//Bounds of current node
		output += indent + "@" + std::to_string(depth) + " " + "\n";


		if (m_bottomLeftQuad != nullptr)
		{
			output += m_bottomLeftQuad->toString(depth + 1) + "\n";
		}
		else
			output += indent + "\t*\n";

		if (m_bottomRightQuad != nullptr)
		{
			output += m_bottomRightQuad->toString(depth + 1) + "\n";
		}
		else
			output += indent + "\t*\n";
	}
	else
	{
		output += indent + "@" + std::to_string(depth) + " ";

		for (Node node : m_nodesInQuad)
		{
			for (int offset : node.fileOffset)
			{
				output += "[(";

				output += std::to_string(node.location.longitude) + ", " + std::to_string(node.location.latitude);

				output += "), " + std::to_string(offset) + "] ";
			}
		}
		output += "\n";
	}
	return output;
}


bool QuadTree::inBoundary(BoundingBox box)
{
	//If the bounding box provided is not within the boundryBox, return false.
	if (std::abs(box.centerPoint.latitude - m_boundingBox.centerPoint.latitude) > (box.boxHalfWidth.latitude + m_boundingBox.boxHalfWidth.latitude))
		return false;
	if (std::abs(box.centerPoint.longitude - m_boundingBox.centerPoint.longitude) > (box.boxHalfWidth.longitude + m_boundingBox.boxHalfWidth.longitude))
		return false;

	return true;
}

bool QuadTree::inBoundary(Coordinate toBeInserted, BoundingBox box)
{
	//If the point provided is not within the bounding box, return false.
	bool isInBoundsLong = (toBeInserted.longitude >= box.topLeft.longitude && toBeInserted.longitude <= box.bottomRight.longitude);
	bool isInBoundsLat = (toBeInserted.latitude <= box.topLeft.latitude && toBeInserted.latitude >= box.bottomRight.latitude);

	return isInBoundsLat && isInBoundsLong;
}

bool QuadTree::inBoundary(Coordinate toBeInserted)
{
	bool isInBoundsLong = (toBeInserted.longitude >= m_topLeft.longitude && toBeInserted.longitude <= m_bottomRight.longitude);
	bool isInBoundsLat = (toBeInserted.latitude <= m_topLeft.latitude && toBeInserted.latitude >= m_bottomRight.latitude);

	return isInBoundsLat && isInBoundsLong;
}