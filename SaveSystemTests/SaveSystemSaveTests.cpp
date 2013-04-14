#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include "../SaveSystem/SaveSystem.h"
#include "../SaveSystem/Serializable.h"
#include <fstream>

SUITE(SaveSystemSavingTests)
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
				node.add("moreInts", moreInts, 7);
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
		MyFixture::Ss.save("testfile.dat");

		SaveNode n1("object1");
		SaveNode n2("object2");

		n1.add("ints", ints, 5);
		n1.add("floats", floats, 8);
		n1.add("single double", singleDouble);

		n2.add("truths", truths, 4);
		n2.add("moreInts", moreInts, 7);

		std::vector<char> data1, data2;

		n1.save(data1);
		n2.save(data2);

		int totalSize = sizeof(bool) + sizeof(int) + data1.size() + data2.size();

		std::streampos fsize = 0;
		std::ifstream file( "testfile.dat", std::ios::binary );

		fsize = file.tellg();
		file.seekg( 0, std::ios::end );
		fsize = file.tellg() - fsize;
		file.close();

		CHECK_EQUAL(totalSize, fsize);
	}
}