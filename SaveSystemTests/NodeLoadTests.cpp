#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include <cstring>

SUITE(LoadTests)
{

	double doubles[3] = {2.5843545, 3.848468, 8484.6546};
	int ints[4] = {4, 56, 48, 86};

	struct MyFixture
	{
		SaveNode node1;
		SaveNode node2;

		MyFixture() : node1("test node"), node2()
		{
			node1.add("doubles", doubles, 3);
			node1.add("ints", ints, 4);

			std::vector<char> data;
			node1.save(data);

			node2.load(data);
		}
		~MyFixture() {}
	};

	TEST_FIXTURE(MyFixture, EntryCount)
	{
		int entryCount = node2.getEntryCount();
		CHECK_EQUAL(2, entryCount);
	}

	TEST_FIXTURE(MyFixture, NodeName)
	{
		std::string name, savedName("test node");
		name = node2.getIdentifier();
		CHECK_ARRAY_EQUAL(savedName.c_str(), name.c_str(), 10);
	}

	TEST_FIXTURE(MyFixture, pollFirstEntryAfterLoad)
	{
		double otherDoubles[3];
		node2.poll("doubles", otherDoubles, 3);
		CHECK_ARRAY_EQUAL(doubles, otherDoubles, 3);
	}

	TEST_FIXTURE(MyFixture, pollSecondEntryAfterLoad)
	{
		int otherInts[4];
		node2.poll("ints", otherInts, 4);
		CHECK_ARRAY_EQUAL(ints, otherInts, 4);
	}
}