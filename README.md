# CC105 Week 8 Laboratory: Stacks (Balanced Brackets and Arithmetic Evaluator)

## Files
- `Stack.h`: template `Stack<T>` using a dynamic array. Starts at capacity 2 and doubles when full. Copy constructor and copy assignment are deleted. The destructor calls `delete[]`. It does not use `std::stack`.
- `main.cpp`: bracket checker, tokenizer, infix-to-postfix conversion, postfix evaluation, and the menu (1 Check brackets, 2 Convert and evaluate, 3 Stack demonstration, 0 Exit).
