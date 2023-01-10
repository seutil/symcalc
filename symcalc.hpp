#pragma once

#include <map>
#include <stack>
#include <cmath>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

/**
 * @file
 */

/**
 * Used within SymCalc::Operators::add function to adding new operators.\n
 * For example of usage see documentation main page
 */
#define DECLARE_OPERATOR(n, c, b, p, a, i) 	{ n, \
											  std::make_tuple<ArgsCount, Operator, Precedence, Associativity, bool> \
										      (c, \
 	 	 	 	 	 	 	 	 	 	 	  ([](std::vector<long double> args) -> std::vector<long double> {b}), \
											  p, \
											  a, \
											  i) \
										    }

/**
 * @namespace SymCalc
 *
 * Name space that contains function, typedefs and enumerators types for the symbolic calculations.
 * @see SymCalc::Operators
 * @see SymCalc::Consts
 */
namespace SymCalc
{
	/**
	 * \brief
	 * Function that associates with operator
	 * @param args Arguments that passed to operator
	 * @return Result of calculations
	 */
	typedef std::vector<long double> (*Operator)(std::vector<long double> args);

	/**
	 * \brief
	 * contains function and typedef for operators(functions):
	 * 		- predefined operations(function) that used for formula calculating
	 * 		- types for operator(function) declaration
	 * 		- function for extracting information about operator(function)
	 * 		- function Operators::add() to add new operators(functions) or replace predefined
	 */
	namespace Operators
	{
		/**
		 * \brief
		 * Precendence of the operator.\n
		 * For example see <a href="http://www.eecs.northwestern.edu/~wkliao/op-prec.htm" target="_blank">Operators precedence</a>
		 */
		typedef size_t Precedence;

		/**
		 * \brief
		 * Typedef for number of function arguments.\n
		 * Used in SymCalc::Operators::OperatorInfo typedef
		 */
		typedef size_t ArgsCount;

		/**
		 * \brief
		 * Associativity of the operator(function)\n
		 * For example see <a href="http://www.eecs.northwestern.edu/~wkliao/op-prec.htm" target="_blank">Associativity of operators</a>
		 */
		typedef enum { LEFT_ASSOCIATED, RIGHT_ASSOCIATED } Associativity;

		/**
		 * Typedef which used to describe operator(function).\n
		 * The last boolean value indicates that operator writes on infix form or not.
		 *
		 * @see #DECLARE_OPERATOR
		 */
		typedef std::tuple<ArgsCount, Operator, Precedence, Associativity, bool> OperatorInfo;

		/**
		 * \brief
		 * Map of Operators::OperatorInfo
		 */
		typedef std::map<std::string, OperatorInfo> OperatorsMap;

		/**
		 * \brief
		 * Predefined operators.
		 * @see Operators::add
		 */
		OperatorsMap operators {
			DECLARE_OPERATOR("+",    2, return {args[0] + args[1]};, 			 10, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("-",    2, return {args[0] - args[1]};, 			 10, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("*",    2, return {args[0] * args[1]};, 			 11, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("/",    2, return {args[0] / args[1]};, 			 11, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("%",    2, return {std::fmod(args[0], args[1])};,   12, LEFT_ASSOCIATED,  true),
			DECLARE_OPERATOR("^", 	 2, return {pow(args[0], args[1])};, 		 13, RIGHT_ASSOCIATED, true),
			DECLARE_OPERATOR("sqrt", 1, return {pow(args[0], 1/args[1])};, 	 	 13, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("sin",  1, return {sin(args[0])};, 				 14, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("cos",  1, return {cos(args[0])};, 				 14, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("tan",  1, return {tan(args[0])};, 				 14, RIGHT_ASSOCIATED, false),
			DECLARE_OPERATOR("~",	 1, return {-args[0]};,					 	 15, RIGHT_ASSOCIATED, true)
		};

		/**
		 * Checking that operator wiht name \a op exists
		 *
		 * @param op Operator identifier
		 * @return true if operator exists, otherwise return false
		 */
		bool is_operator(std::string op)
		{
			for (const auto& iter : operators)
				if (iter.first == op)
					return true;
			return false;
		}

		/**
		 * Add new operator.
		 * @see DECLARE_OPERATOR
		 */
		void add(std::pair<std::string, OperatorInfo> op)
		{
			operators.insert(op);
		}

		/**
		 * @see add
		 */
		void add(std::string n, ArgsCount c, Operator b, Precedence p, Associativity a, bool i)
		{
			if (is_operator(n))
				return;

			OperatorInfo op_info = std::make_tuple(c, b, p, a, i);
			operators.insert({ n, op_info });
		}

		/**
		 * Update already existed operator
		 * @param op Operator identifier
		 * @param op_function New function that will be associates with operator \a op
		 */
		void update(std::string op, Operator op_function)
		{
			if (!is_operator(op))
				throw std::invalid_argument("Operator \"" + op + "\" is not exists");

			std::get<1>(operators.at(op)) = op_function;
		}

		/**
		 * Checking that operator with identifier \a op has write in infix notation
		 *
		 * @param op Operator identifier
		 * @return true if operator exists and have infix notation, otherwise rising exception
		 */
		bool is_infix(std::string op)
		{
			for (const auto& [k, v] : operators)
				if (k == op)
					return std::get<4>(v);
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		/**
		 * Return function that associates with passed operator identifier
		 *
		 * @param op Operator identifier
		 * @return SymCalc::Operator if operator with identifier \a op exists, otherwise rising exception
		 */
		Operator get_operator(std::string op)
		{
			for (const auto& [k, v] : operators)
				if (k == op)
					return std::get<1>(v);
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		/**
		 * Return operator precedence
		 *
		 * @param op Operator identifier
		 * @return Operators::Precedence if operator with identifier \a op exists, otherwise rising exception
		 */
		Precedence get_precedence(std::string op)
		{
			for (const auto& [k, v] : operators)
				if (k == op)
					return std::get<2>(v);
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		/**
		 * Return operator associativity
		 *
		 * @param op Operator identifier
		 * @return Operators::Associativity if operator with identifier \a op exists, otherwise rising exception
		 */
		Associativity get_associativity(std::string op)
		{
			for (const auto& [k, v] : operators)
				if (k == op)
					return std::get<3>(v);
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}

		/**
		 * Return maximum number of arguments that operator \a op accept
		 *
		 * @param op Operator identifier
		 * @return Operators::ArgsCount if operator with identifier \a op exists, otherwise rising exception
		 */
		ArgsCount get_args_count(std::string op)
		{
			for (const auto& [k, v] : operators)
				if (k == op)
					return std::get<0>(v);
			throw std::invalid_argument("Operator \"" + op + "\" is not exists");
		}
	}

	/**
	 * \brief
	 * Namespace that contains constants and functions for working with constants
	 */
	namespace Consts
	{
		/**
		 * Predefined constants
		 */
		std::map<std::string, long double> consts {
			{ "pi", 3.1415926535 },
			{ "e",  2.7182818284 }
		};

		/**
		 * Add new constant
		 *
		 * @param n Constant identifier
		 * @param v Constant value
		 */
		void add(std::string n, long double v)
		{
			consts[n] = v;
		}

		/**
		 * Checking that constants with passed identifier exists
		 *
		 * @param co Constant identifier
		 * @return true if constant with identifier \a co exists, otherwise return false
		 */
		bool is_const(std::string co)
		{
			for (const auto& iter : consts)
				if (iter.first == co)
					return true;
			return false;
		}

		/**
		 * Return constant associated value
		 *
		 * @param co Constant identifier
		 * @return Constant associated value if constant with identifier \a co exists, otherwise rising exception
		 */
		long double get_const(std::string co)
		{
			for (const auto& iter : consts)
				if (iter.first == co)
					return iter.second;
			throw std::invalid_argument("Constant with specified name: \"" + co + "\" is not registered");
		}
	}

	/**
	 * Transform passed formula in infix notation to postfix(reverse polish) notation.\n
	 * For more information see <a href="https://en.wikipedia.org/wiki/Shunting_yard_algorithm" target="_blank">Shunting yard algorithm</a>
	 *
	 * @param formula Algebraic formula in infix notation
	 * @return vector of strings that represents formula in postfix notation
	 * @see SymCalc::calculate_rpn
	 *
	 */
	std::vector<std::string> string_to_rpn(std::string formula)
	{
		using namespace std;

		vector<string> output;
		stack<string> stack;
		size_t formula_length = formula.length();
		for (size_t i = 0; i < formula_length; i++)
			if (formula[i] == ' ' || formula[i] == ',')
				continue;
			else if (isdigit(formula[i]))
			{
				string number = formula.substr(i++, 1);

				// Number consists only of digits or '.'(decimal separator) characters
				while (i < formula_length && (formula[i] == '.' || isdigit(formula[i])))
					number += formula[i++];
				output.push_back(number);
				i--; // used in order not to skip next character
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
				stack.pop(); // remove left parenthesis from stack

				/* If operator writes not on infix form than parenthesis
				 * refer to the function(such as sqrt, cos, sin, etc.)
				 */
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

				/* If symbol that we read is a punctuation character
				 * than operator or constant name consists only of punctuation characters.
				 * We should except '(' and ')' character because this is special characters.
				 *
				 * Otherwise, operator or constant name consist only of alphabet characters,
				 */
				while (i < formula_length && (is_punct ? (ispunct(formula[i]) && formula[i] != '(' && formula[i] != ')') : isalpha(formula[i])))
					name += formula[i++];
				i--; // used in order not to skip next character

				if (Operators::is_operator(name))
				{
					if (i == formula_length)
						throw invalid_argument("Operator \"" + name + "\" expects operands");

					if (!stack.size() || stack.top() == "(")
					{
						stack.push(name);
						continue;
					}

					/* Shunting yard algorithm.
					 * For more information see https://en.wikipedia.org/wiki/Shunting_yard_algorithm
					 *  or main page in the documentation.
					 */
					Operators::Precedence op1_precedence = Operators::get_precedence(name);
					Operators::Precedence op2_precedence = Operators::get_precedence(stack.top());
					bool is_op1_left_assocated = Operators::get_associativity(name) == Operators::LEFT_ASSOCIATED;
					while (!stack.empty() &&
						  (op2_precedence > op1_precedence || (op1_precedence == op2_precedence && is_op1_left_assocated)))
					{
						output.push_back(stack.top());
						stack.pop();
						op2_precedence = Operators::get_precedence(stack.top());
					}
					stack.push(name);
				}
				else if (Consts::is_const(name))
					output.push_back(to_string(Consts::get_const(name)));
				else
					throw invalid_argument("Symbol \"" + name + "\" is not operator or constant");
			}

		/* Last step of shunting yard algorithm.
		 * For more information see https://en.wikipedia.org/wiki/Shunting_yard_algorithm
		 *  or main page in the documentation.
		 */
		while (stack.size())
		{
			output.push_back(stack.top());
			stack.pop();
		}

		return output;
	}

	/**
	 * Calculate passed vector of strings that represents formula in postfix notation
	 *
	 * @param rpn Formula in postfix notation
	 * @return Result of formula calculations
	 */
	std::vector<long double> calculate_rpn(std::vector<std::string> rpn)
	{
		using namespace std;

		vector<long double> output;
		for (const string& s : rpn)
			if (!Operators::is_operator(s))
				output.push_back(stod(s));
			else
			{
				/* Extracting @args_count passed arguments for operator(function).
				 *
				 * Note:
				 * 	Number of operator(function) arguments are varies.
				 * 	That means that in function can be passed 1, 2, ..., max_args_count arguments.
				 */
				vector<long double> args;
				Operators::ArgsCount args_count = Operators::get_args_count(s);
				while (args_count-- && output.size())
				{
					args.push_back(output[output.size()-1]);
					output.pop_back();
				}

				/* Flipping extracted arguments vector, because order of passed arguments
				 * to operator(function) in RPN notation has been reversed.
				 */
				size_t args_length = args.size();
				for (size_t i = 0; i < args_length/2; i++)
				{
					long double temp = args[i];
					args[i] = args[args_length-i-1];
					args[args_length-i-1] = temp;
				}

				vector<long double> operator_output = Operators::get_operator(s)(args);
				while (operator_output.size())
				{
					output.push_back(operator_output[operator_output.size()-1]);
					operator_output.pop_back();
				}
			}

		/* Reversing output vector */
		size_t output_length = output.size();
		for (size_t i = 0; i < output.size() / 2; i++)
		{
			long double temp = output[i];
			output[i] = output[output_length-i];
			output[output_length-i] = temp;
		}
		return output;
	}

	/**
	 * Calculate passed algebraic formula.\n
	 *
	 * Alias for consecutive calls to the SymCalc::string_to_rpn and SymCalc::calculate_rpn functions
	 *
	 * @param formula Algebraic formula in infix notation
	 * @return Result of formula calculations
	 * @see
	 *
	 */
	std::vector<long double> calculate(std::string formula)
	{
		return calculate_rpn(string_to_rpn(formula));
	}
}
