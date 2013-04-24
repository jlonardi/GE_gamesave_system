#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include "../SaveSystem/SaveSystem.h"
#include "../SaveSystem/Serializable.h"
#include <fstream>

SUITE(SaveSystemLoadingTests)
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
			void save(SaveNode& node)
			{
				node.add("ints", ints, 5);
				node.add("floats", floats, 8);
				node.add("single double", singleDouble);
			}
			void load(SaveNode& node) {}
	};

	class GameObject2 : public Serializable
	{
		public:
			void save(SaveNode& node)
			{
				node.add("truths", truths, 4);
				node.add("moreInts", moreInts, 5);
			}
			void load(SaveNode& node) {}
	};

	struct MyFixture
	{
		SaveSystem Ss;
		GameObject1 obj1;
		GameObject2 obj2;

		MyFixture() : Ss(), obj1(), obj2()
		{
			Ss.attach(&obj1, "object1");
			Ss.attach(&obj2, "object2");
		}

		~MyFixture(){}
	};

	TEST_FIXTURE(MyFixture, SavingDataToFile)
	{
		// Saves the data with one savesystem
		MyFixture::Ss.save("load.dat");
		// And loads with an other
		SaveSystem ss2;
		// First its needed to attach two new serializable objects
		class GameObject1 : public Serializable
		{
			public:
				
				int intArray[5];
				float floatArray[8];
				double aDouble;

				void save(SaveNode& node) {}
				void load(SaveNode& node) 
				{
					node.poll("ints", intArray, 5);
					node.poll("floats", floatArray, 8);
					node.poll("single double", aDouble);
				}
		};

		class GameObject2 : public Serializable
		{
			public:
				bool boolArray[4];
				int intArray[5];
				void save(SaveNode& node) {}
				void load(SaveNode& node) 
				{
					node.poll("truths", boolArray, 4);
					node.poll("moreInts", intArray, 5);
				}
		};

		GameObject1 obj3;
		GameObject2 obj4;
		ss2.attach(&obj3, "object1");
		ss2.attach(&obj4, "object2");
		ss2.load("load.dat");
		CHECK_ARRAY_EQUAL(ints, obj3.intArray, 5);
		CHECK_ARRAY_EQUAL(floats, obj3.floatArray, 8);
		CHECK_EQUAL(singleDouble, obj3.aDouble);

		CHECK_ARRAY_EQUAL(truths, obj4.boolArray, 4);
		CHECK_ARRAY_EQUAL(moreInts, obj4.intArray, 5);
	}
}