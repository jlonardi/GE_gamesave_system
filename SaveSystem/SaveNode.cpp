#include "SaveNode.h"
#include <string>
#include <stdint.h>
#include <iostream>

SaveNode::SaveNode() : identifier(""), dataLength(), dataArray(), valueIDs()
{
}

SaveNode::SaveNode(const SaveNode& other) : identifier(other.identifier), dataLength(other.dataLength),
	dataArray(other.dataArray), valueIDs(other.valueIDs)
{
}

SaveNode::SaveNode(std::string id) : identifier(id), dataLength(),
	dataArray(), valueIDs()
{ 
}

void SaveNode::swap(SaveNode& other) {
	std::string tempIdentifier = identifier;
	std::vector<std::string> tempValueIDs = valueIDs;
	std::vector<char> tempDataArray = dataArray;
	std::vector<int> tempDataSize = dataLength;

	identifier = other.identifier;
	valueIDs = other.valueIDs;
	dataArray = other.dataArray;
	dataLength = other.dataLength;

	other.identifier = tempIdentifier;
	other.valueIDs = tempValueIDs;
	other.dataArray = tempDataArray;
	other.dataLength = tempDataSize;
}

void SaveNode::setID(std::string id)
{
	identifier = id;
}

SaveNode& SaveNode::operator=(const SaveNode& other) 
{
	if(this == &other) return *this;

	SaveNode temp = SaveNode(other);
	temp.swap(*this);
	return *this;
}

void SaveNode::dataToByteArray(void* data, size_t size, unsigned int length) 
{
	//char *byteData = (char*)data;
	////std::vector<char> v(byteData, byteData + length);
	//for(size_t i = 0; i < (length*size); i++)
	//{
	//	dataArray.push_back(byteData[i]);
	//}
	int offset = dataArray.size();
	dataArray.resize(offset+size*length);
	std::memcpy(&dataArray[offset],(char*)data, (size*length));
}
/*
Saves the whole nodes information in an array of chars.
*/
void SaveNode::save(std::vector<char>& nodeData) 
{
	// First saving the lenght of the identifying string
	nodeData.push_back(identifier.length());
	// Then the identifier itself
	std::copy(identifier.begin(), identifier.end(), back_inserter(nodeData));

}

bool SaveNode::is_big_endian(void) 
{
    union {
		uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

SaveNode::~SaveNode()
{
}
