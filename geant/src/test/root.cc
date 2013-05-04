#include <iostream>
#include "TRandom.h"

using namespace std;

bool p_quiet = false;

void test() {
	TRandom *rnd = new TRandom(time(0));
	Double_t x = rnd->Rndm();
	cout << "x = " << x << endl;
}

int main() {
	test();
	return 0;
}
