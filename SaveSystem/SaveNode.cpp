#include "SaveNode.h"
#include <string>
#include <stdint.h>
#include <iostream>

SaveNode::SaveNode() : identifier(""), dataLengths(), dataArray(), dataIDs()
{
}

SaveNode::SaveNode(const SaveNode& other) : identifier(other.identifier), dataLengths(other.dataLengths),
	dataArray(other.dataArray), dataIDs(other.dataIDs)
{
}

SaveNode::SaveNode(std::string id) : identifier(id), dataLengths(),
	dataArray(), dataIDs()
{ 
}

SaveNode& SaveNode::operator=(const SaveNode& other) 
{
	if(this == &other) return *this;

	SaveNode temp = SaveNode(other);
	temp.swap(*this);
	return *this;
}

void SaveNode::swap(SaveNode& other) {
	std::string tempIdentifier = identifier;
	std::vector<std::string> tempdataIDs = dataIDs;
	std::vector<char> tempDataArray = dataArray;
	std::vector<int> tempDataSize = dataLengths;

	identifier = other.identifier;
	dataIDs = other.dataIDs;
	dataArray = other.dataArray;
	dataLengths = other.dataLengths;

	other.identifier = tempIdentifier;
	other.dataIDs = tempdataIDs;
	other.dataArray = tempDataArray;
	other.dataLengths = tempDataSize;
}

void SaveNode::setID(std::string id)
{
	identifier = id;
}


/*
	Copyes the given data in to the data array as chars/bytes. NOTE this is
	safe since vector is guaranteed to store the data in a continuous block.
*/
void SaveNode::dataToByteArray(void* data, size_t size, unsigned int length) 
{
	int offset = dataArray.size();
	dataArray.resize(offset+size*length);
	offsets.push_back(offset);
	std::memcpy(&dataArray[offset],(char*)data, (size*length));
}
/*
	Loads the data from the byte array into the given data-array. The data
	size to be copied from the memory is determined by checking what was the
	size of the type when it was stored and what is the size of the type on
	the current machine. The smaller one of the two is picked to avoid overflow.
*/
void SaveNode::byteArrayToData(void* data, size_t size, unsigned int length, unsigned int index)
{
	int offset = offsets.at(index);
	int minSize = size;
	if(typeSizes[index] < size) minSize = typeSizes[index];
	std::memcpy(data, &dataArray[offset], (minSize*length));
}

/*
Saves the whole nodes information in an array of chars.
*/
void SaveNode::save(std::vector<char>& nodeData) 
{

}

void save(std::vector<char>& nodeData)
{

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
