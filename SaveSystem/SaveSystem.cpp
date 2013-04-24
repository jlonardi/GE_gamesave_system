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

void SaveSystem::detachByObject(Serializable* saveObject)
{
	nodeMap.erase(saveObject);
}

void SaveSystem::detachById(std::string id)
{
	for(std::map<Serializable*,SaveNode>::iterator it = nodeMap.begin(); it!=nodeMap.end(); it++)
		{
			std::string identifier = it->second.getIdentifier();
			if(!identifier.compare(id)) {
				nodeMap.erase(it);
				break;
			}
		}
}
std::vector<Serializable*> SaveSystem::getAttachedObjects()
{
	keys.clear();
	for(std::map<Serializable*,SaveNode>::iterator it = nodeMap.begin(); it != nodeMap.end(); ++it)
	{
	  keys.push_back(it->first);
	}
	return keys;
}

void SaveSystem::reset()
{
	nodeMap.clear();
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

	for(std::map<Serializable*,SaveNode>::iterator it = nodeMap.begin(); it!=nodeMap.end(); it++)
	{
		it->second = SaveNode(it->second.getIdentifier());
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
	std::streampos fsize = 0;
	std::ifstream file( path, std::ios::binary | std::ios::in);

	fsize = file.tellg();
	file.seekg( 0, std::ios::end );
	fsize = file.tellg() - fsize;
	file.seekg(0, std::ios::beg);

	// Something is wrong if the lenght is less than 5 bytes since even
	// a savefile without any nodes has to have atleast 5 bytes of internal
	// data.
	if(fsize < 5) return;

	std::vector<char> data;
	data.resize((int)fsize);
	// Reads the data from the file in the array
	file.read (&data[0], data.size());
    file.close();

	int offset = 0;

	bool isBigEndian = true;
	std::memcpy(&isBigEndian, &data[offset], sizeof(bool));
	offset += sizeof(bool);

	int countOfNodes = 0;
	std::memcpy(&countOfNodes, &data[offset], sizeof(int));
	offset += sizeof(int);

	for(int i = 0; i < countOfNodes; i++) 
	{
		unsigned int nodeLenght = 0;
		std::memcpy(&nodeLenght, &data[offset], sizeof(int));
		// Cheks that we are not reading something crazy if the nodelenght
		// data has been corrupted.
		if((offset + nodeLenght) > fsize) throw std::runtime_error("Data corrupted.");

		std::vector<char> nodeData;
		nodeData.resize(nodeLenght);
		std::memcpy(&nodeData[0], &data[offset], nodeLenght*sizeof(char));
		offset += nodeLenght;
		SaveNode node = SaveNode();
		node.load(nodeData);
		Serializable* attachedObject;
		for(std::map<Serializable*,SaveNode>::iterator it = nodeMap.begin(); it!=nodeMap.end(); it++)
		{
			std::string identifier = it->second.getIdentifier();
			if(!identifier.compare(node.getIdentifier())) {
				it->first->load(node);
				attachedObject = it->first;
				// removes the mapped Serializable pointer
				break;
			}
		}
		// attaches the previously removed Serializable with the loaded node
		attach(attachedObject, node.getIdentifier());
	}

	file.close();
}

bool SaveSystem::isBigEndian() 
{
	union {
		uint32_t i;
		char c[4];
	} bint = {0x01020304};

	return bint.c[0] == 1; 
}