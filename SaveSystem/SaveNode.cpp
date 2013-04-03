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
	|	A value that tells the lenght of the whole node.
	|------------------------------------------------------------------------------
	|----------------		DATA OF AN ENTRY
	|------------------------------------------------------------------------------
	|	Lenght of the ID data.
	|------------------------------------------------------------------------------
	|	The ID data.
	|-----------------------------------------------------------------------------
	|	Lenght of the bytedata of the entry.
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
	|	Lenght of the ID data.
	|-----------------------------------------------------------------------------
	|	...rest of the data
	 -----------------------------------------------------------------------------
*/

void SaveNode::save(std::vector<char>& nodeData) 
{
	/*
		First thing is to calculate the need of space that the node takes
		when saving to the array. This is for avoiding multiple resizes.
	*/
	int entryCount = dataIDs.size();
	if( (dataIDs.size() != dataLengths.size()) && (dataIDs.size() != typeSizes.size()) && (dataIDs.size() != offsets.size()))
	{
		throw std::runtime_error("Something went really wrong.");
	}

	//std::cout << "entry count: " << entryCount << std::endl;
	size_t dataSize = 0;
	// endiannes
	dataSize += sizeof(bool);
	//std::cout << "bool size: " << sizeof(bool) << std::endl;
	// node lenght field
	dataSize += sizeof(int);
	//std::cout << "node lenght field: " << sizeof(int) << std::endl;

	// size needed for the ID lenght values
	dataSize += sizeof(int) * entryCount;
	//std::cout << "the size needed for ID lenghts: " << sizeof(int) * entryCount << std::endl;
	// size needed for the IDs
	int k = 0;
	for(unsigned int i = 0; i < dataIDs.size(); i++)
	{
		dataSize += sizeof(char) * dataIDs[i].size() + 1;
		//k += sizeof(char) * dataIDs[i].size();
	}
	//std::cout << "the string sizes were: " << k << std::endl;
	// size needed for the datalenght fields
	dataSize += sizeof(int) * entryCount;
	//std::cout << "the size needed for datalenghts: " << sizeof(int) * entryCount << std::endl;
	// size needed for the actual data
	dataSize += sizeof(char) * dataArray.size();
	//std::cout << "data size: " << dataArray.size() << std::endl;
	// size needed for the type sizes
	dataSize += sizeof(int) * entryCount;
	//std::cout << "the size needed for type sizes: " << sizeof(int) * entryCount << std::endl;
	// size needed for the offsets
	dataSize += sizeof(int) * entryCount;
	//std::cout << "the size needed for offsets: " << sizeof(int) * entryCount << std::endl;
	
	nodeData.resize((int)dataSize);

	// copy the endiannes info
	bool isBigEndian = is_big_endian();
	std::memcpy(&nodeData[0], &isBigEndian, sizeof(bool));
	//std::cout << "the data size in the function: " << dataSize << std::endl;
	// copy the size of the node
	std::memcpy(&nodeData[sizeof(bool)], &dataSize, sizeof(int));
	int offset = (int)sizeof(bool) + (int)sizeof(int);
	// save the entrys
	for(int i = 0; i < entryCount; i++)
	{
		// save the lenght of the ID
		int lenght = dataIDs[i].size();
		std::memcpy(&nodeData[offset], &lenght, sizeof(int));
		std::cout << "node ID lenght: " << lenght << std::endl;
		std::cout << "node ID lenght save offset: " << offset << std::endl;
		std::cout << "actual data saved: " << (int)nodeData[offset] << std::endl;
		offset++;
		// save the ID
		std::memcpy(&nodeData[offset], dataIDs[i].c_str(), sizeof(char)*lenght + 1);
		offset += sizeof(char)*lenght;
	}
}

void save(std::vector<char>& nodeData)
{

}

void load(std::vector<char>& nodeData)
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
