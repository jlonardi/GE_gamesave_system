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
	std::vector<std::string> valueIDs;
	std::vector<char> dataArray;
	std::vector<int> dataLength;
	std::vector<dataType> types;
	
	size_t getTypeSize(dataType type);

	// Copy the given data in a char array (we want to save the data in 1 byte chunks)
	void dataToByteArray(void* data, size_t size, unsigned int length);

	void byteArrayToData(void* data, size_t size, unsigned int length);

public:

	SaveNode(const SaveNode& orig);

	SaveNode();

	SaveNode(std::string id);

	SaveNode& operator=(const SaveNode& other);

	void swap(SaveNode& orig);

	void save(std::vector<char>& nodeData);

	bool is_big_endian(void);

	template <typename TYPE>
	void add(const std::string& id, TYPE* value, unsigned int length = 1) 
	{
		if(value == NULL) throw std::runtime_error("Given value was NULL");

		valueIDs.push_back(id);
		std::vector<char> v;
		dataToByteArray(value, sizeof(TYPE), length);
		dataLength.push_back(length);
	}

	template <typename TYPE>
	void add(const std::string& id, TYPE value)
	{
		add(id, &value);
	}

	template <typename TYPE>
	void load(const std::string& id, TYPE* value, unsigned int length = 1) 
	{
		if (std::find(valueIDs.begin(), valueIDs.end(), id) != valueIDs.end())
		{
			std::cout << "found " << id << std::endl;
		} else 
		{
			throw "Given identifier not found";
		}
	}

	template <typename TYPE>
	void load(const std::string& id, TYPE value) 
	{
		load(id, &value);
	}
	
	void setID(std::string id);

	std::string getIdentifier() 
	{
		return identifier;
	}

	std::vector<std::string> getValueIDs() 
	{
		return valueIDs;
	}

	std::vector<char> getDataArray()
	{
		return dataArray;
	}

	std::vector<int> getDataLengths ()
	{
		return dataLength;
	}

	virtual ~SaveNode();
};
