#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "SaveNode.h"
#include "Serializable.h"
#include <cstring>

class SaveSystem
{
private:

	std::map<Serializable*,SaveNode> nodeMap;
	std::vector<Serializable*> keys;

	void swap(SaveSystem& other);

public:

	SaveSystem();

	SaveSystem(const SaveSystem& other);

	SaveSystem& operator=(const SaveSystem& other);

	void attach(Serializable* saveObject, std::string id);

	void detachByObject(Serializable* saveObject);

	void detachById(std::string id);

	void reset();

	std::vector<Serializable*> getAttachedObjects();

	void save(char* path);

	void load(char* path);

	bool isBigEndian();

	~SaveSystem() {}
};

#endif // !SAVESYSTEM_H