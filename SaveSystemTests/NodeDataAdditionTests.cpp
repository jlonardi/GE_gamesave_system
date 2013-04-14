#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include <stdint.h>

SUITE(AddingToNode)
{
	SaveNode node;

	TEST(NodeIdentifierTest)
	{
		node = SaveNode("test node");
		CHECK_EQUAL( "test node", node.getIdentifier());
	}
	/*
		From here starts the tests for checking that the data inserted in the node
		is saved in the wanted manner into a char array.
	*/
	TEST(NumberOfItems)
	{
		node = SaveNode("node");
		uint32_t numbers[5] = {2,534,345,345,6};
		node.add("test", numbers, 5);
		
		CHECK_EQUAL(1, node.getEntryCount());
		
		node.add("more", 42);

		CHECK_EQUAL(2, node.getEntryCount());
	}

	TEST(AddingIntsToNode)
	{
		node = SaveNode("int node");
		uint32_t numbers[5] = {2,534,345,345,6};
		node.add("test", numbers, 5);

		CHECK_EQUAL(sizeof(uint32_t)*5, node.getDataByteCount());
	}

	TEST(AddDoublesToNode)
	{
		node = SaveNode("double node");
		double numbers[5] = {3.56454,5648.2234,456.8484,8.564654,8789.848};
		node.add("test", numbers, 5);

		CHECK_EQUAL((sizeof(double)*5)/sizeof(char), node.getDataByteCount());
	}

	TEST(AddFloatsToNode)
	{
		node = SaveNode("float node");
		float numbers[5] = {3.56454f,5648.2234f,456.8484f,8.564654f,8789.848f};
		node.add("test", numbers, 5);

		CHECK_EQUAL((sizeof(float)*5)/sizeof(char), node.getDataByteCount());
	}

	TEST(AddCharsToNode)
	{
		node = SaveNode("char node");
		char chars[5] = {'d','e','g','s','f'};
		node.add("test", chars, 5);

		CHECK_EQUAL((sizeof(char)*5)/sizeof(char), node.getDataByteCount());
	}

	TEST(AddBooleansToNode)
	{
		node = SaveNode("bool node");
		bool truths[5] = {true,true,false,true,true};
		node.add("test", truths, 5);

		CHECK_EQUAL((sizeof(bool)*5)/sizeof(char), node.getDataByteCount());
	}

	TEST(AddSameIdentifierTwice)
	{
		node = SaveNode("bool node");
		bool truths[5] = {true,true,false,true,true};
		node.add("test", truths, 5);

		char chars[5] = {'d','e','g','s','f'};

		try
		{
			node.add("test", chars, 5);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}

		CHECK_EQUAL((sizeof(bool)*5)/sizeof(char), node.getDataByteCount());
	}


	TEST(AddMultipleEntrysToNode)
	{
		node = SaveNode("multinode");

		int ints[5] = {32575,534,345,345,6};
		float floats[8] = {3.554f,568.223f,456.844f,8.5654f,8789.848f,2231.42f,348.486f,7.844f};
		double singleDouble = 5894.6454;
		bool truths[4] = {true,true,false,true};
		double anOtherDouble = 42;
		int moreInts[7] = {2,534,345,345,6,564,4};

		node.add("ints", ints, 5);
		node.add("floats", floats, 8);
		node.add("a single double", singleDouble);
		node.add("truths", truths, 4);
		node.add("an other single double", anOtherDouble);
		node.add("more ints", moreInts, 7);

		int totalSize = (sizeof(int)*12)/sizeof(char) + (sizeof(float)*8)/sizeof(char)
					+ (sizeof(double)*2)/sizeof(char) + (sizeof(bool)*4)/sizeof(char);

		CHECK_EQUAL(totalSize, node.getDataByteCount());
		CHECK_EQUAL(6, node.getEntryCount());
	}

	TEST(invalid_inserts)
	{
		node = SaveNode("exception node");
		double* d;
		d = NULL;
		
		try
		{
			node.add("bad data length", d, 3);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}

		try
		{
			node.add("bad data length", d);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}

	}
}
