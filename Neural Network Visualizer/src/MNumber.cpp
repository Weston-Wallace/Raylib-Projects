#include "MNumber.h"

MNumbers::MNumbers(std::string path, Rectangle numberBase) {
	file.open(path.c_str(), std::ios::binary);
	index = 0;
	numBase = numberBase;
    empty = true;
}

MNumbers::~MNumbers() {
	file.close();
}

void MNumbers::load(int index) {
	this->index = index;
    loadNumber(index);
    for (int i = 0; i < 784; i++) {
        colors[i] = (Color) { values[i], values[i], values[i], 255 };
    }
    int indx = 0;
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++, indx++) {
            rects[indx].width = numBase.width / 28;
            rects[indx].height = numBase.height / 28;
            rects[indx].x = numBase.x + (rects[i].width * j);
            rects[indx].y = numBase.y + (rects[i].height * i);
        }
    }
    empty = false;
}

void MNumbers::loadNext() {
	if (index < 59999) {
		load(index + 1);
	}
	else {
		throw std::out_of_range("index can not go past 59999");
	}
    empty = false;
}

void MNumbers::loadPrev() {
	if (index > 0) {
		load(index - 1);
	}
	else {
		throw std::out_of_range("index can not be less than 0");
	}
    empty = false;
}

void MNumbers::loadNumber(int index) {
    index = 16 + index * 784;
    uint8_t byte;

    file.seekg(index);
    for (int i = 0; i < 784; i++) {
        file.read((char*) &byte, 1);
        values[i] = (int)byte;
    }
}