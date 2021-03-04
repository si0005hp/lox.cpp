#include "Lox.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace lox;
using namespace std;

class IntegrationTestFixture : public CcloxTestFixtureBase
{
  public:
    IntegrationTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }

    string FilePath(const string &fileName)
    {
        // TODO fix
        return "../test/integration_test/" + fileName;
    }

    void AssertOutput(const string &expected, const string &testFileName)
    {
        std::ostringstream testOs;
        Lox::RunFile(FilePath(testFileName), testOs);
        ASSERT_EQ(expected, testOs.str());
    }
};

TEST_F(IntegrationTestFixture, function)
{
    AssertOutput("9\nHi, there\n2\nnil\n99\n2\n9\n4.5\n99\n10\n-7\n-3.5\n12\n180\n<fn child1>\n<fn parent>\n",
                 "function/basic.lox");

    AssertOutput("9\n1\n2\n6\n7\n", "function/local_function_and_closures.lox");
}

TEST_F(IntegrationTestFixture, class)
{
    AssertOutput("Car\nCar instance\nCrunch!\n", "class/basic.lox");

    AssertOutput("The mint cake is delicious!\nThing instance\n", "class/this.lox");

    AssertOutput("initializing\n1\n2\nb \nb \nBar instance\nhoge\nhoge\nHoge instance\n", "class/constructor.lox");

    AssertOutput("Fry.\nFry.\nPipe.\nA method\n", "class/inheritance.lox");
}
