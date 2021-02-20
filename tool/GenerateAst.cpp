#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const static map<string, string> exprs = {
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

const static map<string, string> stmts = {
    {"Expression", "shared_ptr<Expr> expression"},
    {"Print", "shared_ptr<Expr> expression"},
    {"Var", "shared_ptr<Token> name, shared_ptr<Expr> initializer"},
    {"Block", "vector<shared_ptr<Stmt>> statements"},
    {"If", "shared_ptr<Expr> condition, shared_ptr<Stmt> thenBranch, shared_ptr<Stmt> elseBranch"},
    {"While", "shared_ptr<Expr> condition, shared_ptr<Stmt> body"},
};

const static vector<string> exprVisitorTypes = {"string", "shared_ptr<Value>"};

const static vector<string> stmtVisitorTypes = {"string", "void"};

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

string toLower(const string &str)
{
    string copy = str;
    transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
    return copy;
}

string toUpper(const string &str)
{
    string copy = str;
    transform(copy.begin(), copy.end(), copy.begin(), ::toupper);
    return copy;
}

string defineVisitor(const string &baseName, const map<string, string> &types)
{
    stringstream ss;

    ss << "template <class R> class Visitor";
    ss << " { ";
    ss << " public: ";
    for (auto &[className, fieldsAsStr] : types)
    {
        ss << "virtual R Visit(const " << className << " "
           << "&" + toLower(baseName) << ") = 0;";
    }
    ss << " }; ";
    ss << endl << endl;

    return ss.str();
}

string toConstRefArg(string &fieldEntry)
{
    fieldEntry.erase(0, fieldEntry.find_first_not_of(' '));
    string ref = regex_replace(fieldEntry, regex(" "), " &");
    return "const " + ref;
}

void defineAst(const string &baseName, const map<string, string> &types)
{
    stringstream ss;

    for (auto &[className, fieldsAsStr] : types)
    {
        ss << "class " << className << ";";
    }
    ss << endl << endl;

    /* base class */
    ss << "class " << baseName;
    ss << " { ";
    ss << " public: ";
    ss << defineVisitor(baseName, types);
    ss << "V_" << toUpper(baseName) << "_ACCEPT_METHODS" << endl;
    ss << " }; ";
    ss << endl << endl;

    /* sub classes */
    for (auto &[className, fieldsAsStr] : types)
    {
        auto fields = split(fieldsAsStr, ',');

        ss << "class " << className << " : public " << baseName;
        ss << " { ";
        ss << "public: " << endl;

        // constructor
        ss << className;
        ss << " ( ";
        for (int i = 0; i < fields.size(); i++)
        {
            if (i != 0)
                ss << ", ";
            ss << toConstRefArg(fields[i]);
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
        ss << toUpper(baseName) << "_ACCEPT_METHODS";

        ss << " };";
        ss << endl << endl;
    }

    cout << ss.str() << endl;
}

void defineMacro(const string &baseName, const vector<string> &visitorTypes)
{
    // base
    cout << "#define V_" << toUpper(baseName) << "_ACCEPT_METHODS \\" << endl;
    for (int i = 0; i < visitorTypes.size(); i++)
    {
        if (i != 0)
            cout << " \\" << endl;
        cout << " virtual " << visitorTypes[i] << " Accept(Visitor<" << visitorTypes[i] << "> &visitor) const = 0; ";
    }
    cout << endl << endl;
    // sub
    cout << "#define " << toUpper(baseName) << "_ACCEPT_METHODS \\" << endl;
    for (int i = 0; i < visitorTypes.size(); i++)
    {
        if (i != 0)
            cout << " \\" << endl;
        cout << visitorTypes[i] << " Accept(Visitor<" << visitorTypes[i]
             << "> &visitor) const override { return visitor.Visit(*this); }";
    }
    cout << endl << endl;
}

bool IsStmt(int argc, char const *argv[])
{
    return argc > 1 && string(argv[1]) == "stmt";
}

int main(int argc, char const *argv[])
{
    if (IsStmt(argc, argv))
        defineMacro("Stmt", stmtVisitorTypes);
    else
        defineMacro("Expr", exprVisitorTypes);

    cout << "namespace Cclox";
    cout << " { ";
    cout << endl << endl;
    cout << "using std::shared_ptr;";
    cout << "using std::vector;";
    cout << endl << endl;

    if (IsStmt(argc, argv))
        defineAst("Stmt", stmts);
    else
        defineAst("Expr", exprs);

    cout << " };";

    return 0;
}
