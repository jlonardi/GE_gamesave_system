#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include "../SaveSystem/SaveSystem.h"
#include "../SaveSystem/Serializable.h"
#include <fstream>

SUITE(SaveSystemgGeneralTests)
{
	int ints[5] = {32575,534,345,345,6};
	float floats[8] = {3.554f,568.223f,456.844f,8.5654f,8789.848f,2231.42f,348.486f,7.844f};
	double singleDouble = 5894.6454;
	bool truths[4] = {true,true,false,true};
	double anOtherDouble = 42;
	int moreInts[5] = {2,534,345,345,6};

	class GameObject1 : public Serializable
	{
		public:
			void save(SaveNode& node){}
			void load(SaveNode& node) {}
	};

	class GameObject2 : public Serializable
	{
		public:
			void save(SaveNode& node){}
			void load(SaveNode& node) {}
	};

	struct MyFixture
	{
		SaveSystem ss;
		GameObject1 obj1;
		GameObject2 obj2;

		MyFixture() : ss(), obj1(), obj2()
		{
			ss.attach(&obj1, "object1");
			ss.attach(&obj2, "object2");
		}
		GameObject1* getObject1() { return &obj1; }
		GameObject2* getObject2() { return &obj2; }
		~MyFixture(){}
	};

	TEST_FIXTURE(MyFixture, removeAttachedObjectByPointer)
	{
		std::vector<Serializable*> v = MyFixture::ss.getAttachedObjects();
		CHECK_EQUAL(2, v.size());
		MyFixture::ss.detachByObject(MyFixture::getObject2());
		v = MyFixture::ss.getAttachedObjects();
		CHECK_EQUAL(1, v.size());
		CHECK_EQUAL(MyFixture::getObject1(), v[0]);
	}
}