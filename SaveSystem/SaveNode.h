#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <iostream>

enum dataType{
	INT,
	DOUBLE,
	FLOAT,
	BOOL,
	CHAR,
	INT_ARRAY,
	DOUBLE_ARRAY,
	FLOAT_ARRAY,
	BOOL_ARRAY,
	CHAR_ARRAY
};

class SaveNode {
private:
	std::string identifier;
	std::vector<std::string> dataIDs;
	std::vector<char> dataArray;
	std::vector<int> dataLengths;
	std::vector<size_t> typeSizes;
	std::vector<int> offsets;
	
	size_t getTypeSize(dataType type);

	// Copy the given data in a char array (we want to save the data in 1 byte chunks)
	void dataToByteArray(void* data, size_t size, unsigned int length);

	void byteArrayToData(void* data, size_t size, unsigned int length, unsigned int index);

public:

	SaveNode(const SaveNode& orig);

	SaveNode();

	SaveNode(std::string id);

	SaveNode& operator=(const SaveNode& other);

	void swap(SaveNode& orig);

	void setID(std::string id);

	/*
		The function that saves the given data by "data" in to the node.
	*/
	template <typename TYPE>
	void add(const std::string& id, TYPE* data, unsigned int length = 1) 
	{
		if(data == NULL) throw std::runtime_error("Given data was NULL.");

		dataIDs.push_back(id);
		dataToByteArray(data, sizeof(TYPE), length);
		dataLengths.push_back(length);
		typeSizes.push_back(sizeof(TYPE));
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
	void load(const std::string& id, TYPE* data, unsigned int length = 1) 
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
	void load(const std::string& id, TYPE &data) 
	{
		load(id, &data);
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

	bool is_big_endian(void);

	virtual ~SaveNode();
};
