#include "AstPrinter.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;

class AstPrinterTestFixture : public CcloxTestFixtureBase
{
  public:
    AstPrinter p;

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
    // Literal
    ASSERT_EQ("foo", p.Visit(Literal(make_shared<Object>("foo"))));
    ASSERT_EQ("12.8", p.Visit(Literal(make_shared<Object>(12.8))));

    // TODO: delete me
    shared_ptr<Literal> sp = make_shared<Literal>(make_shared<Object>(3));
    ASSERT_EQ("3", sp->Accept(p));
    ASSERT_EQ("3", p.Visit(*sp));

    // Binary
    ASSERT_EQ("(+ 3 5)",
              p.Visit(Binary(make_shared<Literal>(make_shared<Object>(3)), make_shared<Token>(TOKEN_PLUS, "+", "", 1),
                             make_shared<Literal>(make_shared<Object>(5)))));

    // Assign
    ASSERT_EQ("(= var 5)", p.Visit(Assign(make_shared<Token>(TOKEN_IDENTIFIER, "var", "var", 1),
                                          make_shared<Literal>(make_shared<Object>(5)))));

    // Call
    ASSERT_EQ("(call myfunc  10 sval)",
              p.Visit(Call(make_shared<Variable>(make_shared<Token>(TOKEN_IDENTIFIER, "myfunc", "myfunc", 1)), nullptr,
                           vector<shared_ptr<Expr>>{
                               make_shared<Literal>(make_shared<Object>(10)),
                               make_shared<Literal>(make_shared<Object>("sval")),
                           })));
}

TEST_F(AstPrinterTestFixture, compound1)
{
    ASSERT_EQ("(* (- 123) (group 45.67))",
              p.Visit(Binary(make_shared<Unary>(make_shared<Token>(TOKEN_MINUS, "-", "", 1),
                                                make_shared<Literal>(make_shared<Object>(123))),
                             make_shared<Token>(TOKEN_STAR, "*", "", 1),
                             make_shared<Grouping>(make_shared<Literal>(make_shared<Object>(45.67))))));
}
