#include "common.h"

int hw::random::uniform(const int min, const int max) {
	std::random_device rd;
	std::mt19937_64 gen(rd());

	std::uniform_int_distribution<int> d(min, max);
	return d(gen);
}

float hw::random::uniform(const float min, const float max) {
	std::random_device rd;
	std::mt19937_64 gen(rd());

	std::uniform_real_distribution<float> d(min, max);
	return d(gen);
}

bool hw::random::binary(const float p) {
	std::random_device rd;
	std::mt19937_64 gen(rd());

	std::bernoulli_distribution d(p);
	return d(gen);
}
