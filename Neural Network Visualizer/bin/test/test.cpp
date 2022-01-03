#include <iostream>
#include <functional>
#include <vector>
#include <random>
#include <network.h>

using namespace std;

int main() {
	vector<int> shape = {2, 16, 8, 10};
	Network n(shape);
	n.forward({0.5, 0.25});
	vector<MatrixL<float>> activations = n.getActivations();
	for (int i = 0; i < n.shape().length(); i++) {
		cout << activations[i] << endl;
	}
	cin.get();
	return 0;
}