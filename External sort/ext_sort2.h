/*
 Effective Performance of External Sorting with No Additional Disk Space by Md. Rafiqul Islam, Md. Sumon Sarker†, Sk. Razibul Islam
 A simple algorithm for in-place merging by Jing-Chao Chen
*/

#ifndef EXTERNAL_SORT_EXT_SORT2_H
#define EXTERNAL_SORT_EXT_SORT2_H

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include "filesize.h"
#include "flat_quicksort.h"

template<typename T>
void merge(std::vector<T> &chunk, size_t s, size_t q, size_t r);

template<typename T>
void ext_sort2(const std::string filename, const size_t mem) {
	const size_t memory = mem / 3 * 2;
	const size_t type_size = sizeof(T);
	const uint64_t filesize = file_size(filename), data_size = filesize / type_size;
	std::fstream data(filename, std::ios::in | std::ios::out | std::ios::binary);
	const uint64_t chunk_number = filesize / memory;
	const size_t chunk_size = memory / type_size - (memory / type_size) % 2, chunk_byte_size =
		chunk_size * type_size, half_chunk_byte_size = chunk_byte_size / 2, half_chunk_size = chunk_size / 2;

	std::vector<T> *chunk = new std::vector<T>(chunk_size);
	for (uint64_t i = 0; i < chunk_number; ++i) {
		data.seekg(chunk_byte_size * i);
		data.read((char *) chunk->data(), chunk_byte_size);
		flat_quick_sort(chunk->begin(), chunk->end());
		data.seekp(chunk_byte_size * i);
		data.write((char *) chunk->data(), chunk_byte_size);
	}

	int64_t s = chunk_number, start = 0;
	while (s > 0) {
		data.seekp(half_chunk_byte_size * start);
		data.read((char *) chunk->data(), half_chunk_byte_size);
		for (uint64_t q = 1; q < s; ++q) {
			data.seekg(half_chunk_byte_size * start + chunk_byte_size * q);
			data.read((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
			merge(*chunk, 0, half_chunk_size - 1, chunk_size - 1);
			data.seekp(half_chunk_byte_size * start + chunk_byte_size * q);
			data.write((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
		}
		data.seekp(half_chunk_byte_size * start);
		data.write((char *) chunk->data(), half_chunk_byte_size);

		data.seekp(half_chunk_byte_size * start + chunk_byte_size * s - half_chunk_byte_size);
		data.read((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
		for (int64_t q = s - 2; q >= 0; --q) {
			data.seekg(half_chunk_byte_size * (start + 1) + chunk_byte_size * q);
			data.read((char *) chunk->data(), half_chunk_byte_size);
			merge(*chunk, 0, half_chunk_size - 1, chunk_size - 1);
			data.seekp(half_chunk_byte_size * (start + 1) + chunk_byte_size * q);
			data.write((char *) chunk->data(), half_chunk_byte_size);
		}
		data.seekg(half_chunk_byte_size * start + chunk_byte_size * s - half_chunk_byte_size);
		data.write((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
		--s;
		++start;
		for (uint64_t p = 0; p < s; ++p) {
			data.seekp(half_chunk_byte_size * start + chunk_byte_size * p);
			data.read((char *) chunk->data(), chunk_byte_size);
			merge(*chunk, 0, half_chunk_size - 1, chunk_size - 1);
			data.seekg(half_chunk_byte_size * start + chunk_byte_size * p);
			data.write((char *) chunk->data(), chunk_byte_size);
		}
	}

	delete chunk;
}

template<typename T>
void merge(std::vector<T> &chunk, size_t s, size_t q, size_t r) {
	std::vector<T> *chunk2 = new std::vector<T>(q - s + 1);
	auto it2 = chunk2->begin(), it1 = chunk.begin() + q + 1, it = chunk.begin() + s;
	std::copy(it, it1, it2);
	while (it2 != chunk2->end() && it1 != chunk.begin() + r + 1) {
		if (*it1 > *it2) {
			*it = *it2;
			++it2;
		} else {
			*it = *it1;
			++it1;
		}
		++it;
	}
	if (it1 == chunk.begin() + r + 1)
		std::copy(it2, chunk2->end(), it);
	else
		std::copy(it1, chunk.begin() + r + 1, it);
	delete chunk2;
}
#endif //EXTERNAL_SORT_EXT_SORT2_H
