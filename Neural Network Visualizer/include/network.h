#ifndef NNV_NETWORK_H
#define NNV_NETWORK_H

#include <matrixl.h>
#include <random>
#include <functional>
#include <vector>
#include <chrono>
#include <cmath>
#include <iostream>

class Network {
public:
	Network(std::vector<int> shape);
	void forward(std::vector<float> input);
	void forward(float *input);
	const std::vector<MatrixL<float>> &getActivations() const;
	void setActivations(const std::vector<MatrixL<float>> &activations);
	const MatrixL<int>& shape() const { return shape_; }
private:
	MatrixL<int> shape_;
	std::vector<MatrixL<float>> weights, biases, zs, activations;

	MatrixL<float> *h_func(MatrixL<float> *m) const;
	MatrixL<float> *o_func(MatrixL<float> *m) const;
	float clip(float n) const;
};

#endif //NNV_NETWORK_H