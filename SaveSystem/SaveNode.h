#ifndef SAVENODE_H
#define	SAVENODE_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <iostream>

class SaveNode {

private:
	std::string identifier;
	std::vector<std::string> dataIDs;
	std::vector<char> dataArray;
	std::vector<int> dataLengths;
	std::vector<int> typeSizes;
	std::vector<int> offsets;

	// Copy the given data in a char array (we want to save the data in 1 byte chunks)
	void dataToByteArray(void* data, size_t size, unsigned int length);

	void byteArrayToData(void* data, size_t size, unsigned int length, unsigned int index);

	size_t nodeInternalDataLength();

	void swap(SaveNode& orig);

public:

	SaveNode(const SaveNode& orig);

	SaveNode();

	SaveNode(std::string id);

	SaveNode& operator=(const SaveNode& other);

	void setID(std::string id);

	/*
	The function that saves the given data by "data" in to the node.
	*/
	template <typename TYPE>
	void add(const std::string& id, TYPE* data, unsigned int length = 1) 
	{
		if(data == NULL) throw std::runtime_error("Given data was NULL.");
		if (std::find(dataIDs.begin(), dataIDs.end(), id) != dataIDs.end())  throw std::runtime_error("Indentifier used in node.");

		dataIDs.push_back(id);
		dataToByteArray(data, sizeof(TYPE), length);
		dataLengths.push_back(length);
		typeSizes.push_back((int)sizeof(TYPE));
	}

	template <typename TYPE>
	void add(const std::string& id, TYPE data)
	{
		add(id, &data);
	}

	/*
	The function that loads the stored data by the node in the given container "data"
	*/
	template <typename TYPE>
	void poll(const std::string& id, TYPE* data, unsigned int length = 1) 
	{
		// Checks that the inputs are valid
		if(data == NULL) throw std::runtime_error("Given data was NULL.");
		unsigned int idIndex = std::find(dataIDs.begin(), dataIDs.end(), id) - dataIDs.begin();
		if (idIndex >= dataIDs.size()) throw std::runtime_error("Entry not found from the save node.");
		if(length < (unsigned int)dataLengths[idIndex] || length > (unsigned int)dataLengths[idIndex]) 
		{
			throw std::runtime_error("Incorrect dataLengths.");
		}
		byteArrayToData(data, sizeof(TYPE), length, idIndex);
	}

	template <typename TYPE>
	void poll(const std::string& id, TYPE &data) 
	{
		poll(id, &data);
	}

	std::string getIdentifier() 
	{
		return identifier;
	}


	int getEntryCount()
	{
		return dataIDs.size();
	}

	int getDataByteCount()
	{
		return dataArray.size();
	}

	void save(std::vector<char>& nodeData);

	void load(std::vector<char>& nodeData);

	virtual ~SaveNode();
};

#endif // !SAVENODE_H