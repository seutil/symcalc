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
}
