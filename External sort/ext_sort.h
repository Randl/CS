/*
 Effective Performance of External Sorting with No Additional Disk Space by Md. Rafiqul Islam, Md. Sumon Sarker†, Sk. Razibul Islam
 A simple algorithm for in-place merging by Jing-Chao Chen
*/

#ifndef EXTERNAL_SORT_EXT_SORT_H
#define EXTERNAL_SORT_EXT_SORT_H

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include "filesize.h"
#include "flat_quicksort.h"

template<typename Iter>
void in_place_merge(Iter x0, Iter y0, Iter yn, int64_t k = -1, bool rec = false);

template<typename T>
void ext_sort(const std::string filename, const size_t memory) {

	const size_t type_size = sizeof(T);
	const uint64_t filesize = file_size(filename);
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
		for (int64_t q = 1; q < s; ++q) {
			data.seekg(half_chunk_byte_size * start + chunk_byte_size * q);
			data.read((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
			in_place_merge(chunk->begin(), chunk->begin() + half_chunk_size - 1, chunk->begin() + chunk_size);
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
			in_place_merge(chunk->begin(), chunk->begin() + half_chunk_size - 1, chunk->begin() + chunk_size);
			data.seekp(half_chunk_byte_size * (start + 1) + chunk_byte_size * q);
			data.write((char *) chunk->data(), half_chunk_byte_size);
		}
		data.seekg(half_chunk_byte_size * start + chunk_byte_size * s - half_chunk_byte_size);
		data.write((char *) (chunk->data() + half_chunk_size), half_chunk_byte_size);
		--s;
		++start;
		for (int64_t p = 0; p < s; ++p) {
			data.seekp(half_chunk_byte_size * start + chunk_byte_size * p);
			data.read((char *) chunk->data(), chunk_byte_size);
			in_place_merge(chunk->begin(), chunk->begin() + half_chunk_size - 1, chunk->begin() + chunk_size);
			data.seekg(half_chunk_byte_size * start + chunk_byte_size * p);
			data.write((char *) chunk->data(), chunk_byte_size);
		}
	}

	delete chunk;
}

template<typename Iter>
void merge_B_and_Y(Iter z, Iter y, Iter yn) {
	for (; z < y && y < yn; ++z) {
		Iter j = std::min_element(z, y);
		if (*j <= *y)
			std::swap(*z, *j);
		else {
			std::swap(*z, *y);
			++y;
		}
	}
	if (z < y)
		flat_quick_sort(z, yn);
}

template<typename Iter>
Iter find_next_X_block(Iter x0, Iter z, Iter y, size_t k, size_t f, Iter b1,
                       Iter b2, auto max) {
	auto min1 = max, min2 = max;
	Iter m = x0 + (ptrdiff_t) floor((z - x0 - f) / k) * k + f, x = x0;
	if (m <= z)
		m += k;

	for (auto i = m; i + k <= y; i += k) {
		if (i != b1 && i != b2) {
			Iter j = (i < b1 && b1 < i + k) ? m - 1 : i + k - 1;
			if (*i <= min1 && *j <= min2) {
				x = i;
				min1 = *i;
				min2 = *j;
			}
		}
	}
	return x;
}

template<typename Iter>
void in_place_merge(Iter x0, Iter y0, Iter yn, int64_t k, bool rec) {
	if (k == -1)
		k = (int64_t) sqrt(yn - x0);
	size_t f = (y0 - x0) % k;
	Iter x = (f == 0) ? y0 - 2 * k : y0 - k - f;
	auto t = *x, max = *std::max_element(x0, yn);
	*x = *x0;
	Iter z = x0, y = y0, b1 = x + 1, b2 = y0 - k;
	int i = 0;
	while (y - z > 2 * k) {
		++i;
		if (*x <= *y || y >= yn) {
			*z = *x;
			*x = *b1;
			++x;
			if ((x - x0) % k == f) if (z < x - k)
				b2 = x - k;
			x = find_next_X_block(x0, z, y, k, f, b1, b2, max);
		} else {
			*z = *y;
			*y = *b1;
			++y;
			if ((y - y0) % k == 0)
				b2 = y - k;
		}
		++z;
		*b1 = *z;
		if (z == x)
			x = b1;
		if (z == b2)
			b2 = yn + 1;
		++b1;
		if ((b1 - x0) % k == f)
			b1 = (b2 == yn + 1) ? b1 - k : b2;
	}
	*z = t;
	if (rec)
		merge_B_and_Y(z, y, yn);
	else  {
		flat_quick_sort(z, y);
		in_place_merge(z,y,yn,(int64_t)sqrt(k),true);
	}
}

#endif //EXTERNAL_SORT_EXT_SORT_H
