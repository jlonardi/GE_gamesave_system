#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include <stdint.h>
#include <cstdio>
#include <bitset>

SUITE(NodeSavingTests)
{
	int ints[5] = {32575,534,345,345,6};
	float floats[8] = {3.554f,568.223f,456.844f,8.5654f,8789.848f,2231.42f,348.486f,7.844f};
	double singleDouble = 5894.6454;
	bool truths[4] = {true,true,false,true};
	double anOtherDouble = 42;
	int moreInts[7] = {2,534,345,345,6,564,4};

	struct MyFixture
	{
		SaveNode node;

		MyFixture() : node("test node")
		{
			node.add("ints", ints, 5);
			node.add("floats", floats, 8);
			node.add("a single double", singleDouble);
		}
		
		// Just a helper function for debug
		void printAsBits(char bytes[], int size)
		{
			for(int i = 0; i < size; i++)
			{
				char c = bytes[i];
				for (int j = 7; j >= 0; --j)
				{
					putchar( (c & (1 << j)) ? '1' : '0' );
				}
				putchar(' ');
				if(i%4 == 0) putchar('\n');
			}
			std::cout << std::endl;
		}

		~MyFixture()
		{
        
		}
	};

	TEST_FIXTURE(MyFixture, SavingTheNode)
	{
		
		/*
			The expected size of the node is assumed by calculating the bytes needed to store the data.
			For understanding of the data is structured please see the nodes .cpp file comments on the 
			save function. The values shown are assumptions but they are here to clarify.
			 ------------------------------------------------------------------------------
			|	1  byte for the boolean to tell the endianness of the save.
			|	4  bytes (one int) to tell the total size of the node.
			|	4  bytes for the length of the node name
			|	10 bytes for the name (includes null terminator)
			|------------------------------------------------------------------------------
			|	12  bytes for the ID lengths, 3*4 bytes. (ints)
			|------------------------------------------------------------------------------
			|	28  bytes for the node identifiers, 4 bytes + 6 bytes + 15 bytes + 3 null term
			|------------------------------------------------------------------------------
			|	12 bytes for the bytedata lengths of the entries 3*4 bytes. (ints)
			|----------------------------------------------------------------------------
			|	60 bytes for the data <= ints = 5*4 bytes, floats = 8*4, double = 8
			|------------------------------------------------------------------------------
			|	12 bytes for the actual datalengths of the entries 3*4 bytes. (ints)
			|------------------------------------------------------------------------------
			|	12 bytes for the type sizes of the entry data
			|------------------------------------------------------------------------------
			|	12 bytes for offsets
			 ------------------------------------------------------------------------------
		*/
		
		size_t expected_size = 0;

		expected_size += sizeof(bool);
		expected_size += sizeof(int);
		expected_size += sizeof(int);
		expected_size += sizeof(char)*10;

		expected_size += sizeof(int)*3;
		expected_size += sizeof(char)*28;
		expected_size += sizeof(int)*3;
		expected_size += sizeof(int)*5 + sizeof(float)*8 + sizeof(double);
		expected_size += sizeof(int)*3;
		expected_size += sizeof(int)*3;
		expected_size += sizeof(int)*3;

		std::vector<char> data;
		node.save(data);
		//printAsBits(&data[0], data.size());

		int size = 0;
		size_t offset = 0;

		offset += sizeof(bool);

		// test the node total length
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL((int)expected_size, size);
		offset += sizeof(int);

		// test the node name length
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(10, size);
		offset += sizeof(int);

		// test the node name
		char name[20];
		std::memcpy(name, &data[offset], sizeof(char)*10);
		char comparsionName[10] = {'t','e','s','t',' ','n','o','d','e','\0'};
		CHECK_ARRAY_EQUAL(comparsionName, name, 10);

		// jumps over the name
		offset += sizeof(char)*10;

		// read the length of the 1st entrys ID
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(5*sizeof(char), size);
		offset += sizeof(int);

		// check the name of the 1st node
		char firstNodeName[5];
		std::memcpy(firstNodeName, &data[offset], sizeof(char)*5);
		char firstNodeNameShouldBe[5] = {'i','n','t','s','\0'};
		CHECK_ARRAY_EQUAL(firstNodeNameShouldBe, firstNodeName, 5);
		offset += sizeof(char)*5;

		// read the size of the data bytes
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(sizeof(int)*sizeof(char)*5, size);
		offset += sizeof(int);

		// jump over the data
		offset += sizeof(int)*sizeof(char)*5;
		
		// read the "true length" of the saved 
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(5, size);
		offset += sizeof(int);

		// read the type size of the saved data
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(sizeof(int), size);
		offset += sizeof(int);

		// read the offset of the dataArray of the 1st entry
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(0, size);
		offset += sizeof(int);

		// the 2nd entrys name size (null terminator included)
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(7, size);
		offset += sizeof(int);

		// jump over the name
		offset += sizeof(char)*7;

		// the length of the 2nd entrys data, 8 floats as bytes
		std::memcpy(&size, &data[offset], sizeof(int));
		CHECK_EQUAL(sizeof(float)*sizeof(char)*8, size);
		offset += sizeof(int);

		// jump to the last one
		offset += sizeof(float)*7;
		// check its value

		float f = 0;
		std::memcpy(&f, &data[(int)offset], sizeof(float));
		CHECK_EQUAL(7.844f, f);

		// to the actual length of the float array
		offset += sizeof(float);
		// check it
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(8, size);
		offset += sizeof(int);

		// jump to the beginning of the last ofset (skip type size and offset)
		offset += sizeof(int)*2;
	
		// and jump over the ID info
		offset += sizeof(int);		// jump over the ID length
		offset += sizeof(char)*16;	// jump over the ID

		// test last entrys length
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(sizeof(double), size);
		offset += sizeof(int);		

		offset += sizeof(double);	// jump over the data
		offset += sizeof(int);		// jump over the actual length

		// check the doubles type size stored
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(sizeof(double), size);
		offset += sizeof(int);	
			
		// check the offset	
		size_t lastEntryOffset = sizeof(int)*5 + sizeof(float)*8;
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL((int)lastEntryOffset, size);
	}
}