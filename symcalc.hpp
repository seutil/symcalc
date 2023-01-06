#pragma once

#include <map>
#include <set>
#include <list>
#include <cmath>
#include <stack>
#include <cctype>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>

#define DECLARE_OPERATOR(n, c, b, p, a, i) { n, { c, { ([](std::vector<long double> args) -> long double {b}), { p, { a, i } } } } }

namespace SymCalc
{
	typedef long double (*Operator)(std::vector<long double>);

	namespace Operators
	{
		typedef size_t Precedence;
		typedef size_t ArgsCount;
		typedef enum { LEFT_ASSOCIATED, RIGHT_ASSOCIATED } Associativity;
		typedef std::pair<ArgsCount, std::pair<Operator, std::pair<Precedence, std::pair<Associativity, bool>>>> OperatorInfo;
		typedef std::map<std::string, OperatorInfo> OperatorsMap;

		OperatorsMap operators {
			DECLARE_OPERATOR("+",    2, return args[0] + args[1];, 			 0, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("-",    2, return args[0] - args[1];, 			 0, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("*",    2, return args[0] * args[1];, 			 1, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("/",    2, return args[0] / args[1];, 			 1, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("%",    2, return (int)args[0] % (int)args[1];, 2, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("^", 	 2, return pow(args[0], args[1]);, 		 3, RIGHT_ASSOCIATED, true),
			DECLARE_OPERATOR("sqrt", 1, return pow(args[0], 1/args[1]);, 	 3, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("sin",  1, return sin(args[0]);, 				 4, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("cos",  1, return cos(args[0]);, 				 4, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("tan",  1, return tan(args[0]);, 				 4, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("~",	 1, return -args[0];,					 5, RIGHT_ASSOCIATED, true)
		};

		void add(std::pair<std::string, OperatorInfo> op)
		{
			operators.insert(op);
		}

		bool is_operator(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return true;
			return false;
		}

		bool is_infix(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return iter.second.second.second.second.second;
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		Operator get_operator(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return iter.second.second.first;
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		Precedence get_precedence(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return iter.second.second.second.first;
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		Associativity get_associativity(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return iter.second.second.second.second.first;
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		ArgsCount get_args_count(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return iter.second.first;
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}
	}
	namespace Consts
	{
		std::map<std::string, long double> consts {
			{ "pi", 3.1415926535 },
			{ "e",  2.7182818284 }
		};

		void add(std::string n, long double v)
		{
			consts[n] = v;
		}

		bool is_const(std::string co)
		{
			for (const auto& iter : consts)
				if (iter.first == co)
					return true;
			return false;
		}

		long double get_const(std::string co)
		{
			for (const auto& iter : consts)
				if (iter.first == co)
					return iter.second;
			throw std::invalid_argument("Constant with specified name: \"" + co + "\" is not registered");
		}
	}

	std::vector<std::string> string_to_rpn(const std::string formula)
	{
		using namespace std;

		vector<string> output;
		stack<string> stack;
		size_t formula_length = formula.length();
		for (size_t i = 0; i < formula_length; i++)
			if (formula[i] == ',' || formula[i] == ' ')
				continue;
			else if (isdigit(formula[i]))
			{
				string number = "";
				size_t j = i;
				while (j < formula_length && (formula[j] == '.' || isdigit(formula[j])))
					number += formula[j++];
				output.push_back(number);
				i = --j;
			}
			else if (formula[i] == '(')
				stack.push("(");
			else if (formula[i] == ')')
			{
				if (!stack.size())
					throw length_error("Parenthesis at '" + to_string(i) + "' position was never been opened.");
				while (stack.top() != "(")
				{
					output.push_back(stack.top());
					stack.pop();
					if (!stack.size())
						throw length_error("Parenthesis at '" + to_string(i) + "' position was never been opened.");
				}
				if (stack.top() == "(") stack.pop();

				// check that token at the top of the stack is function
				if (stack.size() && !Operators::is_infix(stack.top()))
				{
					output.push_back(stack.top());
					stack.pop();
				}
			}
			else // when it operator(function) or constant
			{
				string name = formula.substr(i++, 1);
				bool is_punct = ispunct(name[0]);

				while (i < formula_length && (is_punct ? (ispunct(formula[i]) && formula[i] != '(' && formula[i] != ')') : isalpha(formula[i])))
					name += formula[i++];
				i--;

				// operator processing
				if (Operators::is_operator(name))
				{
					if (!stack.size() || stack.top() == "(")
					{
						stack.push(name);
						continue;
					}

					Operators::Precedence op1_precedence = Operators::get_precedence(name);
					Operators::Precedence op2_precedence = Operators::get_precedence(stack.top());
					bool is_op1_left_assocated = Operators::get_associativity(name) == Operators::LEFT_ASSOCIATED;
					while (op2_precedence > op1_precedence ||
						  (op1_precedence == op2_precedence && is_op1_left_assocated))
					{
						output.push_back(stack.top());
						stack.pop();
						if (stack.size())
							op2_precedence = Operators::get_precedence(stack.top());
						else
							break;
					}
					stack.push(name);
				}
				else if (Consts::is_const(name))
					output.push_back(to_string(Consts::get_const(name)));
				else
					throw invalid_argument("Symbol \"" + name + "\" is not operator or constant");
			}

		while (stack.size())
		{
			output.push_back(stack.top());
			stack.pop();
		}

		return output;
	}

	long double calculate_rpn(std::vector<std::string> rpn)
	{
		using namespace std;

		stack<long double> stack;
		for (const string& s : rpn)
			if (!Operators::is_operator(s))
				stack.push(stod(s));
			else
			{
				vector<long double> args;
				Operators::ArgsCount max_args_count = Operators::get_args_count(s);
				while (max_args_count-- && stack.size())
				{
					args.push_back(stack.top());
					stack.pop();
				}

				// reverse arguments
				size_t args_length = args.size();
				for (size_t i = 0; i < args_length/2; i++)
				{
					long double temp = args[i];
					args[i] = args[args_length-i-1];
					args[args_length-i-1] = temp;
				}

				stack.push(Operators::get_operator(s)(args));
			}
		return stack.top();
	}

	long double calculate(std::string formula)
	{
		return calculate_rpn(string_to_rpn(formula));
	}
}
