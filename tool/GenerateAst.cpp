#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const map<string, string> exprs = {
    {"Assign", "shared_ptr<Token> name, shared_ptr<Expr> value"},
    {"Binary", "shared_ptr<Expr> left, shared_ptr<Token> op, shared_ptr<Expr> right"},
    {"Call", "shared_ptr<Expr> callee, shared_ptr<Token> paren, vector<shared_ptr<Expr>> arguments"},
    {"Get", "shared_ptr<Expr> object, shared_ptr<Token> name"},
    {"Grouping", "shared_ptr<Expr> expression"},
    {"Literal", "shared_ptr<Object> value"},
    {"Logical", "shared_ptr<Expr> left, shared_ptr<Token> op, shared_ptr<Expr> right"},
    {"Set", "shared_ptr<Expr> object, shared_ptr<Token> name, shared_ptr<Expr> value"},
    {"Super", "shared_ptr<Token> keyword, shared_ptr<Token> method"},
    {"This", "shared_ptr<Token> keyword"},
    {"Unary", "shared_ptr<Token> op, shared_ptr<Expr> right"},
    {"Variable", "shared_ptr<Token> name"},
};

vector<string> split(string str, char del)
{
    vector<string> result;
    string subStr;

    for (const char c : str)
    {
        if (c == del)
        {
            if (subStr != "")
                result.push_back(subStr);
            subStr.clear();
        }
        else
        {
            subStr += c;
        }
    }

    if (subStr != "")
        result.push_back(subStr);
    return result;
}

string toMember(string str)
{
    str.at(0) = toupper(str.at(0));
    return "m" + str;
}

string defineVisitor()
{
    stringstream ss;

    ss << "template <class R> class Visitor";
    ss << " { ";
    ss << " public: ";
    for (auto &[className, fieldsAsStr] : exprs)
    {
        ss << "virtual R Visit(const " << className << " &expr) = 0;";
    }
    ss << " }; ";
    ss << endl << endl;

    return ss.str();
}

void defineAst()
{
    stringstream ss;

    for (auto &[className, fieldsAsStr] : exprs)
    {
        ss << "class " << className << ";";
    }
    ss << endl << endl;

    /* base expr */
    ss << "class Expr";
    ss << " { ";
    ss << " public: ";
    ss << defineVisitor();
    ss << "V_ACCEPT_METHODS" << endl;
    ss << " }; ";
    ss << endl << endl;

    /* sub exprs */
    for (auto &[className, fieldsAsStr] : exprs)
    {
        auto fields = split(fieldsAsStr, ',');

        ss << "class " << className << " : public Expr ";
        ss << " { ";
        ss << "public: " << endl;

        // constructor
        ss << className;
        ss << " ( ";
        for (int i = 0; i < fields.size(); i++)
        {
            if (i != 0)
                ss << ", ";
            ss << fields[i];
        }
        ss << " ) : ";
        for (int i = 0; i < fields.size(); i++)
        {
            auto varname = split(fields[i], ' ')[1];
            auto member = toMember(varname);

            if (i != 0)
                ss << ", ";
            ss << member << "(" << varname << ")";
        }
        ss << " {} ";
        ss << endl << endl;

        // member
        for (auto field : fields)
        {
            auto typeAndVarname = split(field, ' ');
            auto type = typeAndVarname[0];
            auto member = toMember(typeAndVarname[1]);

            ss << type << " " << member << ";";
        }
        ss << endl;

        // accept for visitor
        ss << endl;
        ss << "ACCEPT_METHODS";

        ss << " };";
        ss << endl << endl;
    }

    cout << ss.str() << endl;
}

void defineMacro()
{
    cout << "#define V_ACCEPT_METHODS virtual string Accept(Visitor<string> &visitor) const = 0;" << endl;
    cout << endl;

    cout << "#define ACCEPT_METHODS \\" << endl;
    cout << "  string Accept(Visitor<string> &visitor) const override \\" << endl;
    cout << " { \\" << endl;
    cout << "    return visitor.Visit(*this); \\" << endl;
    cout << " } " << endl;
    cout << endl << endl;
}

int main(int argc, char const *argv[])
{
    defineMacro();

    cout << "namespace Cclox";
    cout << " { ";
    cout << endl << endl;
    cout << "using std::shared_ptr;";
    cout << "using std::vector;";
    cout << endl << endl;

    defineAst();

    cout << " };";

    return 0;
}
