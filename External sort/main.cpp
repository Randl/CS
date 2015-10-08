#include <iostream>
#include <chrono>

#include <sys/stat.h>
#include "ext_sort.h"
#include "ext_sort2.h"

extern "C" {
#include "external.h"
}

using namespace std;

void generate_bin(string filename, int64_t size) {
	std::fstream data(filename, std::ios::out | std::ios::binary | std::ios::trunc);
	std::default_random_engine generator;
	std::uniform_int_distribution<int64_t> num(std::numeric_limits<int64_t>::min(),
	                                           std::numeric_limits<int64_t>::max());

	generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	for (int64_t i = 0; i < size; ++i) {
		int64_t x = num(generator);
		data.write((char *) &x, sizeof(int64_t));
	}

}

int64_t test(const char *filename, size_t buf, size_t filesize) {
	FILE *f;
	char *dirpath;
	size_t bufsize;
	char dirname[] = "sort";
	struct stat sb;

	bufsize = buf;

	f = fopen(filename, "rb");

	// Create temp dir
	mkdir(dirname);

	// Do stuff
	auto begin = std::chrono::high_resolution_clock::now();
	external_merge_sort(f, filesize, dirname, bufsize);
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
}


int main() {
	auto total_begin = std::chrono::steady_clock::now();
	const int64_t tries = 10;
	const int size = 750000000, count = size / sizeof(int64_t);
	string filename = "data.bin";
	std::vector<int64_t> results;
	for (int i = 0; i < tries; ++i) {
		generate_bin(filename, count);
		//std::vector<int64_t> *v = new std::vector<int64_t>(count);
		std::fstream data(filename, std::ios::in | std::ios::binary);

		/*auto begin = std::chrono::high_resolution_clock::now();
		ext_sort2<int64_t>(filename, 75000000);//134217728);
		auto end = std::chrono::high_resolution_clock::now();
		results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count());*/
		results.push_back(test(filename.c_str(), 7500000, size));
		std::cout << i + 1 << "\n";
		/*for (int64_t i = 0; i < size; ++i)
			data.write((char *) &v[i], sizeof(int64_t));

		if (is_sorted(v->begin(), v->end()))
			cout << "Good job!\n";
		else
			cout << ":(\n";*/
	}
	std::sort(results.begin(), results.end());
	int64_t m_result = results[results.size() / 2];
	double av_result = 0;
	for (int i = 0; i < tries; ++i) {
		av_result += (double) results[i] / tries;
		cout << (double) results[i] / 1000000000 << " s" << " ";
	}
	auto total_end = std::chrono::steady_clock::now();
	std::cout << "Total time: " <<
	std::chrono::duration_cast<std::chrono::nanoseconds>(total_end - total_begin).count() << " ns = " <<
	(double) (total_end - total_begin).count() / 1000000000 << " s" << std::endl;
	std::cout << "Median: " << m_result << " ns = " << (double) m_result / 1000000000 << " s" << std::endl;
	std::cout << "Average: " << av_result << " ns = " << av_result / 1000000000 << " s" << std::endl;
	return 0;
}

