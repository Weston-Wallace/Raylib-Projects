#include <iostream>

using namespace std;

int *foo();

int main() {
	int *test;
	test = foo();
	for (int i = 0; i < 10; i++) {
		cout << test[i] << ", ";
	}
	cout << endl;
	delete test;
	cin.get();
	return 0;
}

int *foo() {
	int *bar = new int[10];
	for (int i = 0; i < 10; i++) {
		bar[i] = i;
	}
	return bar;
}