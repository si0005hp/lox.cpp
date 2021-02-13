#include "Parser.h"
#include "Scanner.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Cclox;
using namespace std;

class ParserTestFixture : public ::testing::Test
{
  public:
    shared_ptr<Scanner> s;
    shared_ptr<Parser> p;

    ParserTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
    void SetupParser(const string &source)
    {
        s = make_shared<Scanner>(source);
        p = make_shared<Parser>(s->ScanTokens());
    }
};

template <typename T> T As(shared_ptr<Expr> expr)
{
    shared_ptr<T> tp = static_pointer_cast<T>(expr);
    return *tp;
}

TEST_F(ParserTestFixture, Primary)
{
    SetupParser("12 3.4 \"foobar\" true false nil");

    auto literal = As<Literal>(p->ParsePrimary());
    ASSERT_EQ(12, literal.mValue->Number());

    literal = As<Literal>(p->ParsePrimary());
    ASSERT_EQ(3.4, literal.mValue->Number());

    literal = As<Literal>(p->ParsePrimary());
    ASSERT_EQ("foobar", literal.mValue->Text());

    literal = As<Literal>(p->ParsePrimary());
    ASSERT_EQ(true, literal.mValue->Bool());

    literal = As<Literal>(p->ParsePrimary());
    ASSERT_EQ(false, literal.mValue->Bool());

    literal = As<Literal>(p->ParsePrimary());
    ASSERT_EQ(true, literal.mValue->IsNil());
}

TEST_F(ParserTestFixture, Unary)
{
    SetupParser("-3 !false --4.8");

    auto unary = As<Unary>(p->ParseUnary());
    ASSERT_EQ("-", unary.mOp->Lexeme());
    ASSERT_EQ(3, As<Literal>(unary.mRight).mValue->Number());

    unary = As<Unary>(p->ParseUnary());
    ASSERT_EQ(TOKEN_BANG, unary.mOp->Type());
    ASSERT_EQ(false, As<Literal>(unary.mRight).mValue->Bool());

    unary = As<Unary>(p->ParseUnary());
    ASSERT_EQ("-", unary.mOp->Lexeme());
    auto unaryNest1 = As<Unary>(unary.mRight);
    ASSERT_EQ("-", unaryNest1.mOp->Lexeme());
    ASSERT_EQ(4.8, As<Literal>(unaryNest1.mRight).mValue->Number());
}

TEST_F(ParserTestFixture, Factor)
{
    SetupParser("3*5 14/-7*6");

    // 3*5
    auto factor = As<Binary>(p->ParseFactor());
    ASSERT_EQ(3, As<Literal>(factor.mLeft).mValue->Number());
    ASSERT_EQ("*", factor.mOp->Lexeme());
    ASSERT_EQ(5, As<Literal>(factor.mRight).mValue->Number());

    // 14/-7*6
    factor = As<Binary>(p->ParseFactor());

    auto leftAsBinary = As<Binary>(factor.mLeft);
    ASSERT_EQ(14, As<Literal>(leftAsBinary.mLeft).mValue->Number());
    ASSERT_EQ("/", leftAsBinary.mOp->Lexeme());
    auto unary = As<Unary>(leftAsBinary.mRight);
    ASSERT_EQ("-", unary.mOp->Lexeme());
    ASSERT_EQ(7, As<Literal>(unary.mRight).mValue->Number());

    ASSERT_EQ("*", factor.mOp->Lexeme());
    ASSERT_EQ(6, As<Literal>(factor.mRight).mValue->Number());
}

TEST_F(ParserTestFixture, Term)
{
    SetupParser("2*3+14/7");

    auto term = As<Binary>(p->ParseTerm());
    auto lFactor = As<Binary>(term.mLeft);
    ASSERT_EQ(2, As<Literal>(lFactor.mLeft).mValue->Number());
    ASSERT_EQ("*", lFactor.mOp->Lexeme());
    ASSERT_EQ(3, As<Literal>(lFactor.mRight).mValue->Number());

    auto rFactor = As<Binary>(term.mRight);
    ASSERT_EQ(14, As<Literal>(rFactor.mLeft).mValue->Number());
    ASSERT_EQ("/", rFactor.mOp->Lexeme());
    ASSERT_EQ(7, As<Literal>(rFactor.mRight).mValue->Number());
}

TEST_F(ParserTestFixture, Comparison)
{
    SetupParser("4>7 3<=5 1+2<8");

    auto comp = As<Binary>(p->ParseComparison());
    ASSERT_EQ(4, As<Literal>(comp.mLeft).mValue->Number());
    ASSERT_EQ(">", comp.mOp->Lexeme());
    ASSERT_EQ(7, As<Literal>(comp.mRight).mValue->Number());

    comp = As<Binary>(p->ParseComparison());
    ASSERT_EQ(3, As<Literal>(comp.mLeft).mValue->Number());
    ASSERT_EQ("<=", comp.mOp->Lexeme());
    ASSERT_EQ(5, As<Literal>(comp.mRight).mValue->Number());

    comp = As<Binary>(p->ParseComparison());
    auto term = As<Binary>(comp.mLeft);
    ASSERT_EQ(1, As<Literal>(term.mLeft).mValue->Number());
    ASSERT_EQ("+", term.mOp->Lexeme());
    ASSERT_EQ(2, As<Literal>(term.mRight).mValue->Number());
    ASSERT_EQ("<", comp.mOp->Lexeme());
    ASSERT_EQ(8, As<Literal>(comp.mRight).mValue->Number());
}

TEST_F(ParserTestFixture, Equality)
{
    SetupParser("8==2 2>3!=6+11*4");

    auto eq = As<Binary>(p->ParseEquality());
    ASSERT_EQ(8, As<Literal>(eq.mLeft).mValue->Number());
    ASSERT_EQ("==", eq.mOp->Lexeme());
    ASSERT_EQ(2, As<Literal>(eq.mRight).mValue->Number());

    eq = As<Binary>(p->ParseEquality());
    ASSERT_EQ("!=", eq.mOp->Lexeme());
    auto leftAsComp = As<Binary>(eq.mLeft);
    ASSERT_EQ(">", leftAsComp.mOp->Lexeme());
    auto rightAsTerm = As<Binary>(eq.mRight);
    ASSERT_EQ("+", rightAsTerm.mOp->Lexeme());
    ASSERT_EQ("*", As<Binary>(rightAsTerm.mRight).mOp->Lexeme());
}
