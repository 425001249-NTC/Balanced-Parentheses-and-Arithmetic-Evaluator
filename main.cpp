#include "Stack.h"
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

enum class TokType { Number, Operator, LeftParen, RightParen };

struct Token {
    TokType type;
    double value;
    char op;
};

bool isOpening(char c) { return c == '(' || c == '[' || c == '{'; }
bool isClosing(char c) { return c == ')' || c == ']' || c == '}'; }
char openerFor(char c) { return c == ')' ? '(' : (c == ']' ? '[' : '{'); }
int precedence(char op) { return (op == '*' || op == '/') ? 2 : 1; }

string num(double v) {
    if (floor(v) == v && fabs(v) < 1e15) return to_string(static_cast<long long>(v));
    ostringstream o; o.precision(10); o << v; return o.str();
}

bool checkBrackets(const string& s, string& message) {
    Stack<char> open;
    Stack<size_t> where;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (isOpening(c)) { open.push(c); where.push(i + 1); }
        else if (isClosing(c)) {
            char top; size_t p;
            if (!open.pop(top)) {
                message = string("Unmatched closing bracket '") + c + "' at position " + to_string(i + 1);
                return false;
            }
            where.pop(p);
            if (top != openerFor(c)) {
                message = string("Mismatched brackets: '") + top + "' opened at position " + to_string(p)
                        + " but closed by '" + c + "' at position " + to_string(i + 1);
                return false;
            }
        }
    }
    if (!open.isEmpty()) {
        char top; size_t p;
        open.peek(top); where.peek(p);
        message = "Unclosed opening bracket: " + to_string(open.size()) + " left over; innermost '"
                + string(1, top) + "' at position " + to_string(p);
        return false;
    }
    message = "Balanced";
    return true;
}

bool tokenize(const string& s, vector<Token>& out, string& err) {
    out.clear();
    bool expectOperand = true;
    char prev = 's';
    size_t i = 0;
    while (i < s.size()) {
        char c = s[i];
        string at = " (position " + to_string(i + 1) + ")";
        if (isspace(static_cast<unsigned char>(c))) { ++i; continue; }
        if (isdigit(static_cast<unsigned char>(c))) {
            size_t j = i;
            while (j < s.size() && isdigit(static_cast<unsigned char>(s[j]))) ++j;
            if (j < s.size() && s[j] == '.') { err = "Decimals are not supported" + at; return false; }
            if (!expectOperand) { err = "Missing operator before number" + at; return false; }
            double v;
            try { v = stod(s.substr(i, j - i)); }
            catch (...) { err = "Number is too large to convert" + at; return false; }
            out.push_back({TokType::Number, v, 0});
            expectOperand = false; prev = 'n'; i = j; continue;
        }
        if (c == '.') { err = "Decimals are not supported" + at; return false; }
        if (isOpening(c)) {
            if (!expectOperand) { err = "Implicit multiplication is not supported" + at; return false; }
            out.push_back({TokType::LeftParen, 0, '('});
            prev = '('; ++i; continue;
        }
        if (isClosing(c)) {
            if (expectOperand) {
                err = (prev == '(') ? "Empty group" + at
                    : (prev == 'o') ? "Operator is missing its right operand" + at
                    : "Unexpected closing bracket" + at;
                return false;
            }
            out.push_back({TokType::RightParen, 0, ')'});
            prev = ')'; ++i; continue;
        }
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            if (expectOperand) {
                err = (c == '+' || c == '-') ? string("Unary sign is not supported") + at
                                             : string("Invalid operator sequence") + at;
                return false;
            }
            out.push_back({TokType::Operator, 0, c});
            expectOperand = true; prev = 'o'; ++i; continue;
        }
        err = string("Unsupported character '") + c + "'" + at;
        return false;
    }
    if (out.empty()) { err = "Empty expression"; return false; }
    if (expectOperand) { err = "Expression ends with an operator (missing operand)"; return false; }
    return true;
}

bool toPostfix(const vector<Token>& in, vector<Token>& out, string& err) {
    out.clear();
    Stack<char> ops;
    char top;
    for (const Token& t : in) {
        if (t.type == TokType::Number) out.push_back(t);
        else if (t.type == TokType::LeftParen) ops.push('(');
        else if (t.type == TokType::Operator) {
            while (ops.peek(top) && top != '(' && precedence(top) >= precedence(t.op)) {
                ops.pop(top); out.push_back({TokType::Operator, 0, top});
            }
            ops.push(t.op);
        } else {
            bool found = false;
            while (ops.pop(top)) {
                if (top == '(') { found = true; break; }
                out.push_back({TokType::Operator, 0, top});
            }
            if (!found) { err = "Unmatched closing bracket"; return false; }
        }
    }
    while (ops.pop(top)) {
        if (top == '(') { err = "Unclosed opening bracket"; return false; }
        out.push_back({TokType::Operator, 0, top});
    }
    return true;
}

string postfixToString(const vector<Token>& p) {
    string s;
    for (size_t i = 0; i < p.size(); ++i) {
        if (i) s += ' ';
        s += (p[i].type == TokType::Number) ? num(p[i].value) : string(1, p[i].op);
    }
    return s;
}

bool evaluatePostfix(const vector<Token>& p, double& result, string& err) {
    Stack<double> vals;
    for (const Token& t : p) {
        if (t.type == TokType::Number) { vals.push(t.value); continue; }
        double right, left;
        if (!vals.pop(right) || !vals.pop(left)) { err = "Operator without two operands"; return false; }
        double r = 0;
        switch (t.op) {
            case '+': r = left + right; break;
            case '-': r = left - right; break;
            case '*': r = left * right; break;
            case '/':
                if (right == 0) { err = "Division by zero"; return false; }
                r = left / right; break;
        }
        if (!isfinite(r)) { err = "Result is not finite"; return false; }
        vals.push(r);
    }
    if (vals.size() != 1) { err = vals.isEmpty() ? "No value to evaluate" : "Extra operands left on the stack"; return false; }
    vals.pop(result);
    return true;
}

struct Outcome {
    bool ok = false;
    string bracketStatus, postfix, error;
    double value = 0;
};

Outcome processExpression(const string& input) {
    Outcome o;
    if (!checkBrackets(input, o.bracketStatus)) { o.error = o.bracketStatus; return o; }
    vector<Token> toks, post;
    if (!tokenize(input, toks, o.error)) return o;
    if (!toPostfix(toks, post, o.error)) return o;
    o.postfix = postfixToString(post);
    if (!evaluatePostfix(post, o.value, o.error)) return o;
    o.ok = true;
    return o;
}

static void checkBracketsMenu() {
    cout << "Enter expression: ";
    string line; getline(cin, line);
    string msg;
    bool ok = checkBrackets(line, msg);
    cout << (ok ? "[OK] " : "[ERROR] ") << msg << "\n";
}

static void convertAndEvaluateMenu() {
    cout << "Enter expression: ";
    string line; getline(cin, line);
    Outcome o = processExpression(line);
    cout << "Input:           " << line << "\n";
    cout << "Bracket status:  " << (o.bracketStatus.empty() ? "n/a" : o.bracketStatus) << "\n";
    if (!o.ok) { cout << "[ERROR] " << o.error << "\n"; return; }
    cout << "Postfix:         " << o.postfix << "\n";
    cout << "Result:          " << num(o.value) << "\n";
}

static void stackDemo() {
    Stack<int> s; int v;
    auto show = [&](const string& op, const string& res) {
        cout << "  " << op << " -> " << res << " | contents " << s.toString()
             << " | size " << s.size() << " | capacity " << s.capacity() << "\n";
    };
    show("(start)", "-");
    bool r = s.pop(v);       show("pop on empty", r ? "true" : "false");
    s.push(10);              show("push 10", "-");
    s.push(20);              show("push 20", "-");
    r = s.peek(v);           show("peek", r ? to_string(v) : "false");
    s.push(30);              show("push 30 (array grows)", "-");
    for (int i = 0; i < 3; ++i) { r = s.pop(v); show("pop", r ? to_string(v) : "false"); }
    r = s.peek(v);           show("peek on empty", r ? to_string(v) : "false");
}

int main() {
    while (true) {
        cout << "\n=== Stack Laboratory ===\n"
                "1 Check brackets\n2 Convert and evaluate expression\n3 Run stack demonstration\n0 Exit\nChoice: ";
        string choice;
        if (!getline(cin, choice)) break;                 // end of input
        size_t a = choice.find_first_not_of(" \t"), b = choice.find_last_not_of(" \t");
        choice = (a == string::npos) ? "" : choice.substr(a, b - a + 1);
        if (choice == "1") checkBracketsMenu();
        else if (choice == "2") convertAndEvaluateMenu();
        else if (choice == "3") stackDemo();
        else if (choice == "0") { cout << "Goodbye.\n"; break; }
        else cout << "Invalid choice. Please enter 1, 2, 3, or 0.\n";
    }
    return 0;
}
