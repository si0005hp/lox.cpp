#include "Parser.h"
#include "Scanner.h"
#include "TestUtil.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace cclox;
using namespace std;

class ParserTestFixture : public CcloxTestFixtureBase
{
  public:
    ParserTestFixture()
    {
    }
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(ParserTestFixture, Primary)
{
    auto p = GenerateParserFromSource("12 3.4 \"foobar\" true false nil myvar");

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

    auto var = As<Variable>(p->ParsePrimary());
    ASSERT_EQ("myvar", var.mName->Lexeme());
}

TEST_F(ParserTestFixture, Unary)
{
    auto p = GenerateParserFromSource("-3 !false --4.8");

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
    auto p = GenerateParserFromSource("3*5 14/-7*6");

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
    auto p = GenerateParserFromSource("2*3+14/7");

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
    auto p = GenerateParserFromSource("4>7 3<=5 1+2<8");

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
    auto p = GenerateParserFromSource("8==2 2>3!=6+11*4");

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

TEST_F(ParserTestFixture, Assign)
{
    auto p = GenerateParserFromSource("a=1 999 b=c=2");

    auto assign = As<Assign>(p->ParseAssignment());
    ASSERT_EQ("a", assign.mName->Lexeme());
    ASSERT_EQ(1, As<Literal>(assign.mValue).mValue->Number());

    auto literal = As<Literal>(p->ParseAssignment());
    ASSERT_EQ(999, literal.mValue->Number());

    assign = As<Assign>(p->ParseAssignment());
    ASSERT_EQ("b", assign.mName->Lexeme());
    assign = As<Assign>(assign.mValue);
    ASSERT_EQ("c", assign.mName->Lexeme());
    ASSERT_EQ(2, As<Literal>(assign.mValue).mValue->Number());
}

TEST_F(ParserTestFixture, Logical)
{
    auto p = GenerateParserFromSource("1 and 2 3 or 4 and 5");

    auto and1 = As<Logical>(p->ParseAnd());
    ASSERT_EQ(1, As<Literal>(and1.mLeft).mValue->Number());
    ASSERT_EQ("and", and1.mOp->Lexeme());
    ASSERT_EQ(2, As<Literal>(and1.mRight).mValue->Number());

    auto or1 = As<Logical>(p->ParseOr());
    ASSERT_EQ(3, As<Literal>(or1.mLeft).mValue->Number());
    ASSERT_NO_THROW(As<Logical>(or1.mRight));
}

TEST_F(ParserTestFixture, Print)
{
    auto p = GenerateParserFromSource("print 999; print \"Hello\";");

    auto print = As<Print>(p->ParseStatement());
    ASSERT_EQ(999, As<Literal>(print.mExpression).mValue->Number());

    print = As<Print>(p->ParseStatement());
    ASSERT_EQ("Hello", As<Literal>(print.mExpression).mValue->Text());
}

TEST_F(ParserTestFixture, Expression)
{
    auto p = GenerateParserFromSource("999; \"Hello\";");

    auto expression = As<Expression>(p->ParseStatement());
    ASSERT_EQ(999, As<Literal>(expression.mExpression).mValue->Number());

    expression = As<Expression>(p->ParseStatement());
    ASSERT_EQ("Hello", As<Literal>(expression.mExpression).mValue->Text());
}

TEST_F(ParserTestFixture, VarDeclaration)
{
    auto p = GenerateParserFromSource("var foo; var i = 10; var s = \"hoge\";");

    auto var = As<Var>(p->ParseDeclaration());
    ASSERT_EQ("foo", var.mName->Lexeme());
    ASSERT_FALSE(var.mInitializer);

    var = As<Var>(p->ParseDeclaration());
    ASSERT_EQ("i", var.mName->Lexeme());
    ASSERT_EQ(10, As<Literal>(var.mInitializer).mValue->Number());

    var = As<Var>(p->ParseDeclaration());
    ASSERT_EQ("s", var.mName->Lexeme());
    ASSERT_EQ("hoge", As<Literal>(var.mInitializer).mValue->Text());
}

TEST_F(ParserTestFixture, Block)
{
    auto p = GenerateParserFromSource("{ var a = 1; print a; }");

    auto block = As<Block>(p->ParseStatement());

    ASSERT_EQ(2, block.mStatements.size());
    // TODO fix to typeid check
    ASSERT_NO_THROW(As<Var>(block.mStatements.at(0)));
    ASSERT_NO_THROW(As<Print>(block.mStatements.at(1)));
}

TEST_F(ParserTestFixture, If)
{
    stringstream ss;
    ss << "if (true) print 1; ";
    ss << "if (false) print 1; else { var a = 2; print a; } ";

    auto p = GenerateParserFromSource(ss.str());

    auto if1 = As<If>(p->ParseStatement());
    ASSERT_NO_THROW(As<Literal>(if1.mCondition));
    ASSERT_NO_THROW(As<Print>(if1.mThenBranch));
    ASSERT_FALSE(if1.mElseBranch);

    auto ifElse1 = As<If>(p->ParseStatement());
    ASSERT_NO_THROW(As<Literal>(ifElse1.mCondition));
    ASSERT_NO_THROW(As<Print>(ifElse1.mThenBranch));
    ASSERT_NO_THROW(As<Block>(ifElse1.mElseBranch));
}

TEST_F(ParserTestFixture, While)
{
    stringstream ss;
    ss << "while (true) print 1; ";
    ss << "while (false) { print 2; }; ";
    auto p = GenerateParserFromSource(ss.str());

    auto while1 = As<While>(p->ParseStatement());
    ASSERT_NO_THROW(As<Literal>(while1.mCondition));
    ASSERT_NO_THROW(As<Print>(while1.mBody));

    auto while2 = As<While>(p->ParseStatement());
    ASSERT_NO_THROW(As<Literal>(while2.mCondition));
    ASSERT_NO_THROW(As<Block>(while2.mBody));
}

TEST_F(ParserTestFixture, For)
{
    stringstream ss;
    ss << "for (var a = 0; a < 3; a = a + 1) { print a; }";
    ss << "for (;;) print 1;";
    auto p = GenerateParserFromSource(ss.str());

    auto for1 = As<Block>(p->ParseStatement());
    ASSERT_NO_THROW(As<Var>(for1.mStatements.front()));
    auto for1While = As<While>(for1.mStatements.back());
    ASSERT_NO_THROW(As<Literal>(for1While.mCondition));
    auto for1WhileBody = As<Block>(for1While.mBody);
    ASSERT_NO_THROW(As<Print>(for1WhileBody.mStatements.at(0)));
    ASSERT_NO_THROW(As<Expression>(for1WhileBody.mStatements.back()));

    auto for2 = As<While>(p->ParseStatement());
    ASSERT_TRUE(As<Literal>(for2.mCondition).mValue->Bool());
    ASSERT_NO_THROW(As<Print>(for2.mBody));
}
