//
// Created by Evgenii on 28.09.2015.
//
#include <fstream>
#include "filesize.h"


int64_t file_size(const std::string filename) {
	std::ifstream file(filename.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!file.is_open()) {
		return -1;
	}

	file.seekg(0, std::ios::end);
	int size = file.tellg();
	file.close();

	return size;
}
