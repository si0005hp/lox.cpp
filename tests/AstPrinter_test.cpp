#include "AstPrinter.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;

class AstPrinterTestFixture : public ::testing::Test
{
  public:
    AstPrinterTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(AstPrinterTestFixture, base)
{
    AstPrinter p;

    ASSERT_EQ("foo", p.Visit(Literal(Object("foo"))));
    ASSERT_EQ("12.8", p.Visit(Literal(Object(12.8))));

    // const Expr &lref = literal;
    // vector<reference_wrapper<const Expr>> exprs{lref};

    // for (const Expr &expr : exprs)
    // {
    //     ASSERT_EQ("foo", expr.Accept(p));
    // }

    // const Expr *ep = &literal;
    // ASSERT_EQ("foo", ep->Accept(p));
}
