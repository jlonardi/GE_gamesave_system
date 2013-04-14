#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"

SUITE(NodeConstructorTests)
{
	int ints[5] = {32575,534,345,345,6};
	float floats[8] = {3.554f,568.223f,456.844f,8.5654f,8789.848f,2231.42f,348.486f,7.844f};
	double singleDouble = 5894.6454;
	bool truths[4] = {true,true,false,true};
	double anOtherDouble = 42;
	int moreInts[7] = {2,534,345,345,6,564,4};

	SaveNode node1("node1");

	TEST(CopyConstructor)
	{
		node1.add("ints", ints, 5);
		node1.add("floats", floats, 8);
		node1.add("a single double", singleDouble);

		SaveNode node2(node1);
		std::string id = node2.getIdentifier();
		std::string idShouldBe("node1");

		CHECK_ARRAY_EQUAL(node1.getIdentifier().c_str(), node2.getIdentifier().c_str(), 6);

		std::vector<char> data1;
		std::vector<char> data2;

		node1.save(data1);
		node2.save(data2);

		CHECK_EQUAL(data1.size(), data2.size());

		node1.add("moreInts", moreInts, 7);

		data1.clear();
		node1.save(data1);

		CHECK(data1.size() != data2.size());

	}

	TEST(Assingment)
	{
		SaveNode node3 = node1;
		std::string id = node3.getIdentifier();
		std::string idShouldBe("node1");

		CHECK_ARRAY_EQUAL(node1.getIdentifier().c_str(), node3.getIdentifier().c_str(), 6);

		std::vector<char> data1;
		std::vector<char> data2;

		node1.save(data1);
		node3.save(data2);

		CHECK_EQUAL(data1.size(), data2.size());

		node1.add("truths", truths, 4);

		data1.clear();
		node1.save(data1);

		CHECK(data1.size() != data2.size());
	}
}