#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"

int ints[5] = {32575,534,345,345,6};
float floats[8] = {3.554f,568.223f,456.844f,8.5654f,8789.848f,2231.42f,348.486f,7.844f};
double singleDouble = 5894.6454;
bool truths[4] = {true,true,false,true};
double anOtherDouble = 42;
int moreInts[7] = {2,534,345,345,6,564,4};

struct MyFixture
{
    SaveNode  node;;

    MyFixture() : node("test node")
    {
       
		
		node.add("ints", ints, 5);
		node.add("floats", floats, 8);
		node.add("a single double", singleDouble);
		node.add("truths", truths, 4);
		node.add("an other single double", anOtherDouble);
		node.add("more ints", moreInts, 7);
    }

    ~MyFixture()
    {
        
    }
};

TEST_FIXTURE(MyFixture, LoadingSingleDouble)
{
	double d;
	node.load("a single double", d);
	CHECK_EQUAL(singleDouble, d);
}

TEST_FIXTURE(MyFixture, LoadingIntArray)
{
	int i[7];
	i[1] = 0;
	node.load("more ints", i, 7);
	CHECK_EQUAL(534, i[1]);
	CHECK_ARRAY_EQUAL(moreInts, i, 7);
}

TEST_FIXTURE(MyFixture, LoadingFloatArray)
{
	float f[8];
	node.load("floats", f, 8);
	CHECK_EQUAL(2231.42f, f[5]);
	CHECK_ARRAY_EQUAL(floats, f, 8);
}

TEST_FIXTURE(MyFixture, LoadingBooleanArray)
{
	bool b[4];
	node.load("truths", b, 4);
	CHECK_EQUAL(true, b[0]);
	CHECK_ARRAY_EQUAL(truths, b, 4);
}

TEST_FIXTURE(MyFixture, LoadingBadValues)
{
		double* d;
		d = NULL;
		try
		{
			node.load("a single double", d);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}

		int dummyInts[4];
		try
		{
			node.load("you wont find this", dummyInts);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}

		float dummyFloats[8];
		try
		{
			node.load("floats", dummyFloats, 7);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}
		try
		{
			node.load("floats", dummyFloats, 9);
			CHECK(false);
		} catch(std::runtime_error)
		{
			CHECK(true);
		}
}