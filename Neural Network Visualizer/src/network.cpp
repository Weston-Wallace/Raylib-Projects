#include "network.h"

using namespace std;

Network::Network(std::vector<int> shape) {
	this->shape_ = MatrixL<int>({shape.size()}, shape.data(), shape.size());
	weights = vector<MatrixL<float>>(shape.size() - 1);
	biases = vector<MatrixL<float>>(shape.size() - 1);
	zs = vector<MatrixL<float>>(shape.size() - 1);
	activations = vector<MatrixL<float>>(shape.size());
	default_random_engine gen(chrono::system_clock::now().time_since_epoch().count());
	uniform_real_distribution<float> dist(-1, 1);
	auto getNum = bind(dist, gen);
	for (int i = 0; i < shape.size() - 1; i++) {
		weights[i] = MatrixL<float>({shape[i], shape[i + 1]});
		biases[i] = MatrixL<float>({shape[i + 1]});
		for (int j = 0; j < shape[i] * shape[i + 1]; j++) {
			weights[i].flatAt(j) = getNum();
		}
		for (int j = 0; j < shape[i + 1]; j++) {
			biases[i][j] = getNum();
		}
	}
}

void Network::forward(std::vector<float> input) {
	activations[0] = MatrixL<float>({1, input.size()}, input.data(), input.size());
	for (int i = 0; i < shape_.length() - 1; i++) {
		zs[i] = activations[i].reg_matmul(weights[i]) + MatrixL<float>({1, biases[i].shape()[0]}, &(biases[i].flatAt(0)), biases[i].length());
		if (i < shape_.length() - 2) {
			activations[i + 1] = *h_func(&zs[i]);
		}
		else {
			activations[i + 1] = *o_func(&zs[i]);
		}
	}
}

void Network::forward(float *input) {
	activations[0] = MatrixL<float>({1, shape_.flatAt(0)}, input, shape_.flatAt(0));
	for (int i = 0; i < shape_.length() - 1; i++) {
		zs[i] = activations[i].reg_matmul(weights[i]) + MatrixL<float>({1, biases[i].shape()[0]}, &(biases[i].flatAt(0)), biases[i].length());
		if (i < shape_.length() - 2) {
			activations[i + 1] = *h_func(&zs[i]);
		}
		else {
			activations[i + 1] = *o_func(&zs[i]);
		}
	}
}

const vector<MatrixL<float>> &Network::getActivations() const {
	return activations;
}

void Network::setActivations(const std::vector<MatrixL<float>> &activations) {
	this->activations = activations;
}

MatrixL<float> *Network::h_func(MatrixL<float> *m) const {
	MatrixL<float> *ret = new MatrixL<float>(m->shape());
	for (int i = 0; i < m->length(); i++) {
		ret->flatAt(i) = m->flatAt(i) < 0 ? 0 : m->flatAt(i);
	}
	return ret;
}

MatrixL<float> *Network::o_func(MatrixL<float> *m) const {
	MatrixL<float> *ret = new MatrixL<float>(m->shape());
	for (int i = 0; i < m->length(); i++) {
		ret->flatAt(i) = 1 / 1 + exp(clip(-m->flatAt(i)));
	}
	float sum = ret->sum();
	for (int i = 0 ; i < m->length(); i++) {
		ret->flatAt(i) /= sum;
	}
	return ret;
}

float Network::clip(float n) const {
	if (n >= 0 && n < 1e-14) {
        n = 1e-14;
    }
    else if (n < 0 && n > -1e-14) {
        n = -1e-14;
    }
    else if (n > 20) {
    	n = 20;
    }
    else if (n < -20) {
    	n = -20;
    }
    return n;
}