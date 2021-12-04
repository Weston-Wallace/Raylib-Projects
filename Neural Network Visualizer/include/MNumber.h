#pragma once
#include "matrixl.h"
#include "raylib.h"
#include <Profiler.h>
#include <fstream>
#include <string>
#include <iostream>

class MNumbers {
public:
	MNumbers(std::string path, Rectangle numberBase);
	~MNumbers();
	void loadNext();
	void loadPrev();
	void load(int index);
	Rectangle numBase;
	Color colors[784];
	Rectangle rects[784];
	int values[784];
	std::ifstream file;
	int index;
	bool empty;
	Profiler prof;

private:

	void loadNumber(int index);
	//vector<MatrixL<double>> loadNumbers(ifstream& input, int index, int amount);
};