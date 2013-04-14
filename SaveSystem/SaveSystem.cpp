#include "SaveSystem.h"
#include <fstream>
#include <stdint.h>

SaveSystem::SaveSystem() : nodeMap() {}

SaveSystem::SaveSystem(const SaveSystem& other)
	: nodeMap(other.nodeMap){
}

SaveSystem& SaveSystem::operator=(const SaveSystem& other)
{
	if(this == &other) return *this;

	SaveSystem temp = SaveSystem(other);
	temp.swap(*this);
	return *this;
}

void SaveSystem::swap(SaveSystem& other)
{
	std::map<Serializable*,SaveNode> temp = other.nodeMap;
	other.nodeMap = nodeMap;
	nodeMap = temp;
}

void SaveSystem::attach(Serializable* saveObject, std::string id)
{
	SaveNode node(id);
	nodeMap.insert(std::pair<Serializable*,SaveNode>(saveObject, node)); 
}

void SaveSystem::save(char* path)
{
	std::vector<char> data;
	data.resize(sizeof(bool) + sizeof(int));
	int offset = 0;

	bool endianness = isBigEndian();
	std::memcpy(&data[offset], &endianness, sizeof(bool));
	offset += sizeof(bool);

	int countOfNodes = nodeMap.size();
	std::memcpy(&data[offset], &countOfNodes, sizeof(int));
	offset += sizeof(int);

	for(std::map<Serializable*,SaveNode>::iterator it = nodeMap.begin(); it!=nodeMap.end(); ++it)
	{
		it->first->save(it->second);

		std::vector<char> tempData;
		it->second.save(tempData);
		
		data.resize(offset + tempData.size());
		std::memcpy(&data[offset], &tempData[0], tempData.size() * sizeof(char));
		offset += tempData.size();
	}

	std::ofstream file;
	file.open(path, std::ios::binary | std::ios::out);
	file.write(&data[0], data.size());
	file.close();
}

void SaveSystem::load(char* path)
{
}

bool SaveSystem::isBigEndian() 
{
	union {
		uint32_t i;
		char c[4];
	} bint = {0x01020304};

	return bint.c[0] == 1; 
}