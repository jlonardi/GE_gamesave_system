#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
/*
	Abstract class that a class has to inherit for using SaveSystem
*/
#include "SaveNode.h"

class Serializable
{
public:
	virtual void save(SaveNode& node)=0;
	virtual void load(SaveNode& node)=0;
};
#endif // !SERIALIZABLE_H