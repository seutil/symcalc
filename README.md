# SymCalc Documentation


# Overview

SymCalc is a C++ header-only library for generic symbolic calculations which supports user defined constants and operators(functions) in different notation styles([infix](https://en.wikipedia.org/wiki/Infix_notation) and [prefix](https://en.wikipedia.org/wiki/Polish_notation) notations), floating point numbers and supportive functions which you can use to transform infix notation to postfix.

For more information about adding new constants and operators(function) see [Constants](#constats) and [Operators](#operators) sections.

## Algorithms
In this section described algorithms which used in SymCalc.

*Note: This algorithms are written on pseudocode*
### Infix to postfix
Algorithm that transform algebraic expression in infix notation to postfix(also known as [reverse polish](https://en.wikipedia.org/wiki/Reverse_Polish_notation)) notation is a modification of [shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting_yard_algorithm) which allow parse not only already predefined operators(such as +, -, /, \*) but also user defined operators(for example functions like *sin*, *cos*, *sqrt* or *foo*) and constants.

``` Python
stack = []
output = []

for token in formula:
    if is_number(token)
        output.push(token)
    else if token = "("
        stack.push(token)
    else if token = ")"
        output.push(stack.pop()) while (stack.top != "(")
        stack.pop() # remove left parenthesis from stack
        
        # checking that parenthesis are associates with function
        # or more generaly, with operator which written on prefix notation
        output.push(stack.pop()) if has_prefix_notation(stack.top) 
    else
        if is_operator(token)
            stack.push(token) and continue if stack.empty() or stack.top = "("
            
            op1_p, op2_p = precedence(token), precedence(stack.top)
            op1_la = is_operator_left_associated(token)
            while !stack.empty() and (op2_p > op1_p or (op1_p = op2_p and op1_la))
                output.push(stack.pop())
                op2_p = precedence(stack.top) # new operator precedence
            stack.push(token)
        else if is_contant(token)
            output.push(get_constant_value(token))
        
output.push(stack.pop()) while !stack.empty()
return output
```

### Calculating formula in postfix notation
Alogirhtm that calculate formula in postfix notation is a default postfix algorithm modification.
This algorithm allows function get variable number of arguments and also return variable number of calculations(see [operators](#operators)) 
``` Python
output = [] # array of floating point numbers

for token in formula:
    if is_number(token)
        output.push(token)
    else
        # extract operator arguments: [0..op_max_arguments_count)
        args, op_max_arguments_count = [], get_max_operator_args_count(token)
        while op_max_arguments_count-- and !output.empty()
            args.push(stack.pop())
        
        # reversing arguments because in reverse notation they are reversed
        args.reverse()
        operator_output = execute_operator(token)(args)
        operator_output.reverse()
        output.append(operator_output)

output.reverse()
return output
```


# Operators
This section describes how you can add new operators and get information about already existed operators.

## Adding new operators
For adding new operators you should use **add** function that are declared in **SymCalc::Operators** namespace and **DECLARE_OPERATOR** macros that declared in **SymCalc** namespace. You can use "add" function with DECLARE_OPERATOR if don't want declare C++ function.

For example, we add new operator with name "foo" which writing in infix notation and return:
* 1, if sum of arguments higher than  0
* 0, if sum equal 0
* -1, if sum lower than 0

``` C++
SymCalc::Operators::add(DECLARE_OPERATOR(
    "foo", // operator name,
    2, // maximum number of arguments that this operators receieves
    {
        // function body, parenthesis { } after 'return' required
        if (args[0] + args[1] > 1) return { 1 };
        else if (args[0] + args[1] == 0) return { 0 };
        else return { -1 };
    }, 
    10, // operator precedence
    SymCalc::Operators::LEFT_ASSOCIATED, // operator associativity
    true // has a infix notation
));
```

And usage of this operator
``` C++
cout << "0 foo -1 = " << SymCalc::calculate("0 foo ~1")[0] << endl;  // ~ is a unary minus operator
cout << "1 foo -1 = " << SymCalc::calculate("1 foo ~1")[0] << endl;
cout << "1 foo 1  = " << SymCalc::calculate("1 foo 1")[0]  << endl;
```

Output
``` console
0 foo -1 = -1
1 foo -1 = 0
1 foo 1 = 1
```

You also can use another version of **add** function. This version can be used when yours function implementation is too long.

Another implementation of the previouse example:
``` C++
vector<long double> operator_foo(vector<long double> args)
{
	long double sum = args[0] + args[1];
	if (sum > 0)
		return { 1 };
	else if (sum == 0)
		return { 0 };
	else
		return { -1 };
}

SymCalc::Operators::add("foo", 2, &operator_foo, 10, SymCalc::Operators::LEFT_ASSOCIATED, true);

cout << "0 foo -1 = " << SymCalc::calculate("0 foo ~1")[0] << endl;
cout << "1 foo -1 = " << SymCalc::calculate("1 foo ~1")[0] << endl;
cout << "1 foo 1  = " << SymCalc::calculate("1 foo 1")[0]  << endl;

```

Output
```
0 foo -1 = -1
1 foo -1 = 0
1 foo 1  = 1
```

## Update predefined operators
You change predefined operators(+, -, /, \*, etc...) behavior. To do this, you should use **update**  function which declarated in **SymCalc::Operators** namespace.

For example, we change + operator behavior to multiplication:
``` C++
vector<long double> new_plus_operator(vector<long double> args)
{
	return { args[0] * args[1] };
}

SymCalc::Operators::update("+", &new_plus_operator);
cout << "2 + 12 =" << SymCalc::calculate("2 + 12");
```

Output
``` console
2 + 12 = 24
```

## Operator Info
**OperatorInfo** typedef that declared in **SymCalc::Operators:** namespace.

OperatorInfo constains next information about operator:

1. ArgsCount - maximum number of arguments that function can receive
2. Operator pointer to function which executed when **SymCalc::calculate** meets operator
3. Precedence - precedence(priority) of the operator
4. Associativity - type of the operator associativity
5. Infix - is operator writes in infix notation or not 


## Predefined operators
See SymCalc::Operators::operators in documentation


# Constants
This section describes how you can add new constants

## Adding new constant
For adding new constant you should use **add** function that declared in **SymCalc::Consts** namespace.

For example, we declare new constant with name "foo":
``` C++
SymCalc::Consts::add("foo", -100);

cout << "1 + foo = " << SymCalc::calculate("1 + foo")[0] << endl;
```

Output:
``` console
1 + foo = -99
```

