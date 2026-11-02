# CC105 Week 8 Laboratory: Stacks (Balanced Brackets and Arithmetic Evaluator)

## Build and run
Compiler: GNU GCC (g++) (default); check the version with `g++ --version`.
The program uses no C++17-specific features. It compiles under C++11, C++14, and C++17.
```
g++ -std=c++11 -Wall -Wextra -o stack_lab main.cpp
./stack_lab
```
Code::Blocks: open Settings > Compiler > Global compiler settings, tick "Have g++ follow the C++11 ISO C++ language standard, then build and run `main.cpp`. Make sure `Stack.h` is in the same folder as `main.cpp`.

## Supported syntax and limitations
Nonnegative integers, spaces, `+ - * /`, and `() [] {}` (all grouping symbols are treated as parentheses during conversion). Evaluation uses `double`, so `7 / 2 = 3.5`. Rejected: letters, decimals, unary signs, implicit multiplication, empty groups, and malformed expressions. Exponents are not supported. Integers too large for `double` conversion are rejected.

## Algorithm summary
1. **Bracket check:** push each opening bracket (with its position). On a closing bracket, pop and compare the type. The expression is balanced only if the stack ends empty. Errors reported: unmatched closing, mismatched pair, unclosed opening.
2. **Tokenizer:** scans left to right, combines digits into one integer token, and uses an operand/operator state rule to reject bad token order.
3. **Infix to postfix:** numbers go straight to the output. `(` is pushed. An operator pops operators of greater or equal precedence first (left-associative), then is pushed. `)` pops until the matching `(`. At the end, remaining operators are popped.
4. **Postfix evaluation:** numbers are pushed onto a `Stack<double>`. An operator pops the right operand first, then the left, computes `left op right`, and pushes the result. It rejects division by zero, missing operands, and any final stack size other than 1.

## Files
- `Stack.h`: template `Stack<T>` using a dynamic array. Starts at capacity 2 and doubles when full. Copy constructor and copy assignment are deleted. The destructor calls `delete[]`. It does not use `std::stack`.
- `main.cpp`: bracket checker, tokenizer, infix-to-postfix conversion, postfix evaluation, and the menu (1 Check brackets, 2 Convert and evaluate, 3 Stack demonstration, 0 Exit).
- `Documentation`: Parts 1 to 5, Test-Case Results, Screenshots, and Reflection.
