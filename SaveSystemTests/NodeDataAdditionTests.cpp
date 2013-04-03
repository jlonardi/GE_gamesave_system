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
		
		int amount_of_numbers = node.getDataLengths().at(0);
		CHECK_EQUAL(5, amount_of_numbers);
		
		node = SaveNode("node");
		node.add("test", 42);

		amount_of_numbers = node.getDataLengths().at(0);
		CHECK_EQUAL(1, amount_of_numbers);
	}

	TEST(AddingIntsToNode)
	{
		node = SaveNode("int node");
		uint32_t numbers[5] = {2,534,345,345,6};
		node.add("test", numbers, 5);

		std::vector<char> theData = node.getDataArray();
		CHECK_EQUAL(sizeof(uint32_t)*5 ,theData.size());
	}

	TEST(AddDoublesToNode)
	{
		node = SaveNode("double node");
		double numbers[5] = {3.56454,5648.2234,456.8484,8.564654,8789.848};
		node.add("test", numbers, 5);

		std::vector<char> theData = node.getDataArray();
		CHECK_EQUAL((sizeof(double)*5)/sizeof(char) ,theData.size());
	}

	TEST(AddFloatsToNode)
	{
		node = SaveNode("float node");
		float numbers[5] = {3.56454f,5648.2234f,456.8484f,8.564654f,8789.848f};
		node.add("test", numbers, 5);

		std::vector<char> theData = node.getDataArray();
		CHECK_EQUAL((sizeof(float)*5)/sizeof(char) ,theData.size());
	}

	TEST(AddCharsToNode)
	{
		node = SaveNode("char node");
		char chars[5] = {'d','e','g','s','f'};
		node.add("test", chars, 5);

		std::vector<char> theData = node.getDataArray();
		CHECK_EQUAL((sizeof(char)*5)/sizeof(char) ,theData.size());
	}

	TEST(AddBooleansToNode)
	{
		node = SaveNode("bool node");
		bool truths[5] = {true,true,false,true,true};
		node.add("test", truths, 5);

		std::vector<char> theData = node.getDataArray();
		CHECK_EQUAL((sizeof(bool)*5)/sizeof(char) ,theData.size());
	}

	TEST(AddMultipleEntrysToNode)
	{
		// This test contains multiple different tests to make sure the data
		// is stored corectly in the char array.

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

		std::vector<char> theData = node.getDataArray();
		/*
			Now what we try is to extract the value of the single double of the value
			42 from the array stored all in chars.

			The offset of the data where the double starts can be calculated from the
			size of the given arrays and multiplying those with the sizeof() of the
			corresponding type.
		*/
		unsigned int offset = sizeof(int)*5 + sizeof(float)*8 + sizeof(double) + sizeof(bool)*4;

		double d;
		std::memcpy(&d, &theData[offset], sizeof(double));
		CHECK_EQUAL(42, d);

		/*
			The same but now for the 5th float.
		*/

		offset = sizeof(int)*5 + sizeof(float)*4;

		float f;
		std::memcpy(&f, &theData[offset], sizeof(float));
		CHECK_EQUAL(8789.848f, f);

		/*
			And once more for the 3rd boolean value.
		*/ 
		offset = sizeof(int)*5 + sizeof(float)*8 + sizeof(double) + sizeof(bool)*2;

		bool truth;
		std::memcpy(&truth, &theData[offset], sizeof(bool));
		CHECK(!truth);

		/* 
			And the last element.
		*/
		offset = sizeof(int)*5 + sizeof(float)*8 + sizeof(double) + sizeof(bool)*4 + sizeof(double) + sizeof(int)*6;

		int i;
		std::memcpy(&i, &theData[offset], sizeof(int));
		CHECK_EQUAL(4, i);
	}

	TEST(invalid_inserts)
	{
		node = SaveNode("exception node");

		
		try
		{
			double* d;
			d = NULL;
			node.add("bad data length", d, 3);
			CHECK(false);
		} catch(std::runtime_error e)
		{
			CHECK(true);
		}

	}
}
