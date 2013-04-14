#include "SaveNode.h"
#include <string>
#include <stdint.h>
#include <iostream>

SaveNode::SaveNode() : identifier(""), dataIDs(), dataArray(), dataLengths(), typeSizes(), offsets()
{
}

SaveNode::SaveNode(const SaveNode& other) : identifier(other.identifier), dataIDs(other.dataIDs),
	dataArray(other.dataArray), dataLengths(other.dataLengths), typeSizes(other.typeSizes), offsets(other.offsets)
{
}

SaveNode::SaveNode(std::string id) : identifier(id), dataIDs(), dataArray(), dataLengths(), typeSizes(), offsets()
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
|	A value that tells the length of the whole node as bytes.
|------------------------------------------------------------------------------
|	A value that tells amount of entrys.
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
	int offset = 0;

	// copy the endiannes info
	//bool isBigEndian = is_big_endian();
	//std::memcpy(&nodeData[0], &isBigEndian, sizeof(bool));
	//offset += (int)sizeof(bool);

	// copy the size of the node
	std::memcpy(&nodeData[offset], &dataSize, sizeof(int));
	offset += (int)sizeof(int);

	int countOfEntrys = dataIDs.size();

	// copy the count of entrys
	std::memcpy(&nodeData[offset], &countOfEntrys, sizeof(int));
	offset += (int)sizeof(int);

	// copy the size of the node name
	int nodeNamelength = identifier.size()+1;
	std::memcpy(&nodeData[offset], &nodeNamelength, sizeof(int));
	offset += sizeof(int);

	// copy the node name
	std::memcpy(&nodeData[offset], identifier.c_str(), sizeof(char)*nodeNamelength);
	offset += sizeof(char)*nodeNamelength;
	//std::cout << "offset just before starting copy entry data: " << offset << std::endl;

	// save the entrys
	for(unsigned int i = 0; i < dataIDs.size(); i++)
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

void SaveNode::load(std::vector<char>& nodeData)
{
	size_t offset = 0;

	// Extracts information about the endianess
	//bool isBigEndian;
	//std::memcpy(&isBigEndian, &nodeData[0], sizeof(bool));
	//offset += sizeof(bool);

	// Gets the node bytecount and checks it matches with the given data
	int byteLenghtOfNode = 0;
	std::memcpy(&byteLenghtOfNode, &nodeData[offset], sizeof(int));
	offset += sizeof(int);

	if(byteLenghtOfNode != nodeData.size()) throw std::runtime_error("Data corrupted.");

	// Extracts the number of entrys stored in the node
	int entryCount = 0;
	std::memcpy(&entryCount, &nodeData[offset], sizeof(int));
	offset += sizeof(int);
	dataIDs.resize(entryCount);

	// Extracts the name of the node
	int nodeNameLength = 0;
	std::memcpy(&nodeNameLength, &nodeData[offset], sizeof(int));
	offset += sizeof(int);

	// And stores the name, vectors are used because the standard does not
	// guarantee that the characters of std::string are stored contiguously
	std::vector<char> name;
	name.resize(nodeNameLength);
	std::memcpy(&name[0], &nodeData[offset], nodeNameLength*sizeof(char));
	identifier.append(&name[0]);
	offset += nodeNameLength*sizeof(char);

	// Extracts the entrys
	for(int i = 0; i < entryCount; i++)
	{
		// Extracts length of the entrys name
		int entryNameLength = 0;
		std::memcpy(&entryNameLength, &nodeData[offset], sizeof(int));
		offset += sizeof(int);

		// Copys the name and stores it
		std::vector<char> entryName;
		entryName.resize(entryNameLength);
		std::memcpy(&entryName[0], &nodeData[offset], entryNameLength*sizeof(char));
		dataIDs[i].append(&entryName[0]);
		offset += entryNameLength*sizeof(char);

		// Extracts the size of the entry data in bytes
		int byteSizeOfEntryData = 0;
		std::memcpy(&byteSizeOfEntryData, &nodeData[offset], sizeof(int));
		dataArray.resize(dataArray.size() + byteSizeOfEntryData);
		offset += sizeof(int);

		// Extracts the data itself
		std::memcpy(&dataArray[dataArray.size() - byteSizeOfEntryData] ,&nodeData[offset], byteSizeOfEntryData*sizeof(char));
		offset += byteSizeOfEntryData*sizeof(char);

		// Extracts the actual length of the data
		int actualLength = 0;
		std::memcpy(&actualLength, &nodeData[offset], sizeof(int));
		dataLengths.push_back(actualLength);
		offset += sizeof(int);

		// Extracts the datatype size that was used when the data was saved
		int dataTypeSize = 0;
		std::memcpy(&dataTypeSize, &nodeData[offset], sizeof(int));
		typeSizes.push_back(dataTypeSize);
		offset += sizeof(int);

		// Extract the offset
		int entryOffset = 0;
		std::memcpy(&entryOffset, &nodeData[offset], sizeof(int));
		offsets.push_back(entryOffset);
		offset += sizeof(int);
	}

}

/*
A helper function to calculate the internal size of the node. The information
is used when the node saves itself into a bytearray.
*/
size_t SaveNode::nodeInternalDataLength()
{
	size_t size = 0;
	// endiannes
	//size += sizeof(bool);

	// node length field
	size += sizeof(int);

	// count of entrys
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

//bool SaveNode::is_big_endian(void) 
//{
//	union {
//		uint32_t i;
//		char c[4];
//	} bint = {0x01020304};
//
//	return bint.c[0] == 1; 
//}

SaveNode::~SaveNode()
{
}
