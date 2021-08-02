#include "print.h"
#include <iostream>

using namespace std;
using namespace zxy;

int main()
{
	TinySTL::Tuple<int, int> t(1, 2);
	cout << t;
}
