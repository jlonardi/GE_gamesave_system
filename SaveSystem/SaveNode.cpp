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
	if(typeSizes[index] < (int)size) minSize = typeSizes[index];
	std::memcpy(data, &dataArray[offset], (minSize*length));
}

/*
	The function saves the whole node's information in an vector of chars given as parameter.

	The following shows the structure of the data saved in the vector.

	 ------------------------------------------------------------------------------
	|----------------	THE HEADER OF THE NODE DATA
	|------------------------------------------------------------------------------
	|	A single byte to tell the endianness of the save.
	|------------------------------------------------------------------------------
	|	A value that tells the length of the whole node.
	|------------------------------------------------------------------------------
	|	A value that tells the size of the nodes name (null terminator inc.).
	|------------------------------------------------------------------------------
	|	The node name data.
	|------------------------------------------------------------------------------
	|----------------		DATA OF AN ENTRY
	|------------------------------------------------------------------------------
	|	length of the ID data  (null terminator inc.).
	|------------------------------------------------------------------------------
	|	The ID data.
	|-----------------------------------------------------------------------------
	|	length of the bytedata of the entry.
	|-----------------------------------------------------------------------------
	|	The data itself.
	|-----------------------------------------------------------------------------
	|	The length of actual data array that will be constructed
	|-----------------------------------------------------------------------------
	|	The size that the type of the data had on the machine it was saved
	|-------------------------------------------------------------------------------
	|	The offset of the data
	|----------------------------------------------------------------------------
	|----------------		THE DATA OF THE OTHER ENTRY
	|-----------------------------------------------------------------------------
	|	length of the ID data.
	|-----------------------------------------------------------------------------
	|	...rest of the data
	 -----------------------------------------------------------------------------
*/

void SaveNode::save(std::vector<char>& nodeData) 
{	
	if( (dataIDs.size() != dataLengths.size()) && (dataIDs.size() != typeSizes.size()) && (dataIDs.size() != offsets.size()))
	{
		throw std::runtime_error("Something went really wrong.");
	}

	size_t dataSize = nodeInternalDataLength();
	
	nodeData.resize((int)dataSize);

	// copy the endiannes info
	bool isBigEndian = is_big_endian();
	std::memcpy(&nodeData[0], &isBigEndian, sizeof(bool));

	// copy the size of the node
	std::memcpy(&nodeData[sizeof(bool)], &dataSize, sizeof(int));
	int offset = (int)sizeof(bool) + (int)sizeof(int);
	
	// copy the size of the node name
	int nodeNamelength = identifier.size()+1;
	std::memcpy(&nodeData[offset], &nodeNamelength, sizeof(int));
	offset += sizeof(int);

	// copy the node name
	std::memcpy(&nodeData[offset], identifier.c_str(), sizeof(char)*nodeNamelength);
	offset += sizeof(char)*nodeNamelength;
	//std::cout << "offset just before starting copy entry data: " << offset << std::endl;

	// save the entrys
	for(int i = 0; i < dataIDs.size(); i++)
	{
		//std::cout << "offset when starting to write entry: " << offset << std::endl;
		// save the length of the ID
		int length = dataIDs[i].size() + 1;
		std::memcpy(&nodeData[offset], &length, sizeof(int));
		offset += sizeof(int);

		// save the ID
		std::memcpy(&nodeData[offset], dataIDs[i].c_str(), sizeof(char)*length);
		offset += sizeof(char)*length;

		int lengthOfByteData = sizeof(char)*typeSizes[i]*dataLengths[i];

		// save the size of the data in bytes
		std::memcpy(&nodeData[offset], &lengthOfByteData, sizeof(int));
		offset += sizeof(int);

		// copy the data itself
		std::memcpy(&nodeData[offset], &dataArray[offsets[i]] ,lengthOfByteData);
		offset += lengthOfByteData;

		// copy the actual length of the corresponding data type
		std::memcpy(&nodeData[offset], &dataLengths[i], sizeof(int));
		offset += sizeof(int);

		//// copy the type size that was used when the data was saved
		std::memcpy(&nodeData[offset], &typeSizes[i] , sizeof(int));
		offset += sizeof(int);

		//// copy the size of the offset of the data in the dataArray
		std::memcpy(&nodeData[offset], &offsets[i] , sizeof(int));
		offset += sizeof(int);
	}
}

void load(std::vector<char>& nodeData)
{

}

/*
	A helper function to calculate the internal size of the node. The information
	is used when the node saves itself into a bytearray.
*/
size_t SaveNode::nodeInternalDataLength()
{
	size_t size = 0;
	// endiannes
	size += sizeof(bool);

	// node length field
	size += sizeof(int);

	// the length of the node name field
	size += sizeof(int);

	// the size needed for the name itself
	size += sizeof(char) * identifier.size() + 1;

	int entryCount = dataIDs.size();

	size += sizeof(int)*entryCount;

	// size needed for the IDs and their length values
	for(unsigned int i = 0; i < dataIDs.size(); i++)
	{
		size += sizeof(char) * dataIDs[i].size() + 1;

	}

	// size needed for the the bytedata length
	size += sizeof(int) * entryCount;

	// size needed for the actual data
	size += sizeof(char) * dataArray.size();

	// size needed for the datalength fields
	size += sizeof(int) * entryCount;

	// size needed for the type sizes
	size += sizeof(int) * entryCount;

	// size needed for the offsets
	size += sizeof(int) * entryCount;

	return size;
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
