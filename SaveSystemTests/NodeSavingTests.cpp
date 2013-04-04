#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"
#include <stdint.h>

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
			|------------------------------------------------------------------------------
			|	12  bytes for the ID lenghts, 3*4 bytes. (ints)
			|------------------------------------------------------------------------------
			|	28  bytes for the node identifiers, 4 bytes + 6 bytes + 15 bytes + 3 null term
			|------------------------------------------------------------------------------
			|	12 bytes for the datalengths of the entries 3*4 bytes. (ints)
			|------------------------------------------------------------------------------
			|	60 bytes for the data <= ints = 5*4 bytes, floats = 8*4, double = 8
			|------------------------------------------------------------------------------
			|	12 bytes for the type sizes of the entry data
			|------------------------------------------------------------------------------
			|	12 bytes for offsets
			 ------------------------------------------------------------------------------
		*/
		
		size_t expected_size = 0;
		expected_size += sizeof(bool);
		expected_size += sizeof(int);
		expected_size += sizeof(int)*3;
		expected_size += sizeof(char)*28;
		expected_size += sizeof(int)*3;
		expected_size += sizeof(int)*5 + sizeof(float)*8 + sizeof(double);
		expected_size += sizeof(int)*3;
		expected_size += sizeof(int)*3;

		std::vector<char> data;
		node.save(data);
		int size = 0;
		std::memcpy(&size, &data[sizeof(bool)], sizeof(int));
		CHECK_EQUAL((int)expected_size, size);

		size_t offset = sizeof(bool) + sizeof(int);
		std::cout << "offset: " << offset << std::endl;
		// read the lenght of the 1st entrys ID
		size = 5;
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(4*sizeof(char), size); // TEST!?!?!?!?!
		std::memcpy(&size, &data[(int)offset+sizeof(int)], sizeof(int));
		CHECK_EQUAL(4*sizeof(char), size); // TEST!?!?!?!?!
		std::cout << sizeof(int) << std::endl;
		CHECK_EQUAL(4*sizeof(char), (int)data[(int)offset]);
		offset += sizeof(int);

		std::string s;
		s.clear();
		char string[5];
		std::memcpy(string, &data[(int)offset], sizeof(char)*4);
		s.append(string);
		std::cout << s << std::endl;

		// read the size of the data contained
		offset += sizeof(char)*4*sizeof(char);
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(sizeof(int)*sizeof(char)*5, size);

		// jump to the beginning of the next entry, the actual lenght + type size + offset
		offset += sizeof(int)*3;
		// the 2nd entrys size of the name
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(6*sizeof(char), size);
		// the lenght of the 2nd entrys data, 8 floats as bytes
		offset += sizeof(char)*6*sizeof(char);
		//std::memcpy(&size, &data[int(offset)], sizeof(int));
		CHECK_EQUAL(sizeof(float)*sizeof(char)*8, size);

		// the beginning of the 2nd entrys data
		offset += sizeof(int);
		// jump to the last one
		offset += sizeof(float)*7;
		// check its value
		float f = 0;
		std::memcpy(&f, &data[(int)offset], sizeof(float));
		CHECK_EQUAL(7.844f, f);

		// to the actual lenght of the float array
		offset += sizeof(float);
		// check it
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL(8, size);

		// jump to the beginning of the last ofset (skip type size and offset)
		offset += sizeof(int)*2;
		// store the offset
		size_t lastEntryOffset = offset;
		// and jump to the last entrys offset info
		offset += sizeof(int);		// jump over the ID lenght
		offset += sizeof(char)*15;	// jump over the ID
		offset += sizeof(int);		// jump over the datalenght
		offset += sizeof(double);	// jump over the data
		offset += sizeof(int);		// jump over the actual lenght
		offset += sizeof(int);		// jump over the type size

		// check the offset
		std::memcpy(&size, &data[(int)offset], sizeof(int));
		CHECK_EQUAL((int)lastEntryOffset, size);
	}

}