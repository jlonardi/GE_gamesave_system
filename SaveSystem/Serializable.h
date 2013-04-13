#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
/*
	Abstract class that all that wishes to use the SaveSystem has to inherit
*/
#include "SaveNode.h"

class Serializable
{
public:
	virtual void save(SaveNode node);
	virtual void load(SaveNode node);
};
#endif // !SERIALIZABLE_H