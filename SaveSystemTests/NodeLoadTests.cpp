#include "../UnitTest++/src/UnitTest++.h"
#include "../SaveSystem/SaveNode.h"

struct MyFixture
{
    SaveNode  node;

    MyFixture() : node("test node")
    {
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
    }

    ~MyFixture()
    {
        
    }
};

TEST_FIXTURE(MyFixture, LoadingSingleDouble)
{
	double d = 0;
	node.load("a single double", d);
	CHECK_EQUAL(d, 42);
}

TEST_FIXTURE(MyFixture, LoadingIntArray)
{
	int i[7];
	i[1] = 0;
	node.load("more ints", i, 7);
	CHECK_EQUAL(534, i[1]);
}

TEST_FIXTURE(MyFixture, LoadingFloatArray)
{
	float f[8];
	f[5] = 0.0f;
	node.load("floats", f, 8);
	CHECK_EQUAL(2231.42f, f[5]);
}

TEST_FIXTURE(MyFixture, LoadingBooleanArray)
{
	bool b[4];
	b[0] = false;
	node.load("truths", b, 4);
	CHECK_EQUAL(true, b[0]);
}