#include "symcalc.hpp"

#include <iostream>

int main()
{
	using namespace std;

	cout << SymCalc::calculate("3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3")[0];

	return 0;
}
