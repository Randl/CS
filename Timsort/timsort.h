#ifndef TIMSORT_H
#define TIMSORT_H

#include <stack>
#include <cstdint>

int8_t minRunSize;
const int8_t minGallop = 7;

template<class T>
struct run {
    T *start;
    uint64_t length;
};

template<typename T>
void timsort(T *, uint64_t);
template<typename T>
bool shouldCollapse(std::stack <run<T>> *);
template<typename T>
int8_t toMinSize(T *, T *);
template<typename T>
bool divide(T *, uint64_t, std::stack <run<T>> *);
int8_t setminRunSize(uint64_t);
template<typename T>
void reverse(T *, uint64_t);
template<typename T>
void merge(run<T>, run<T>);
template<typename T>
void insertionSort(T *, int8_t);
template<typename T>
T *binarySearch(T *, T, T *);

template<typename T>
void timsort(T *array, uint64_t size) {
	minRunSize = setminRunSize(size);
	std::stack <run<T>> runs;
	while (divide(array, size, &runs))
		while (shouldCollapse(&runs));

	while (runs.size() > 1) {
		run<T> temp = runs.top();
		runs.pop();
		merge(runs.top(), temp);
	}
}

template<typename T>
bool shouldCollapse(std::stack <run<T>> *runs) {
	if (runs->size() < 3)
		return 0;

	run<T> t1 = runs->top();
	runs->pop();
	run<T> t2 = runs->top();
	runs->pop();
	run<T> t3 = runs->top();
	if (t1.length > t2.length + t3.length && t2.length > t3.length) {
		runs->push(t2);
		runs->push(t1);
		return 0;
	}
	if (t1.length > t3.length) {
		runs->pop();
		merge(t3, t2);
		runs->push(t3);
		runs->push(t1);
	}
	else {
		merge(t2, t1);
		runs->push(t2);
	}
}

template<typename T>
bool divide(T *array, uint64_t size, std::stack <run<T>> *runs) {
	static T *dividedUpTo = array;
	if (dividedUpTo >= array + size)
		return false;
	T *startsFrom = dividedUpTo;
	uint64_t length;
	T *x = dividedUpTo + 1;
	if (*dividedUpTo > *x) {
		while (x < array + size - 1 && *dividedUpTo > *x) {
			++dividedUpTo;
			++x;
		}
		length = dividedUpTo - startsFrom;
		reverse(startsFrom, length);
	}
	else {
		while (x < array + size - 1 && *dividedUpTo <= *x) {
			++dividedUpTo;
			++x;
		}
		length = dividedUpTo - startsFrom;
	}
	if (length < minRunSize) {
		length = toMinSize(startsFrom, array + size);
		dividedUpTo = startsFrom + length;
	}
	run<T> tmp;
	tmp.start = startsFrom;
	tmp.length = length;
	runs->push(tmp);

	return true;
}

template<typename T>
int8_t toMinSize(T *starts, T *ends) {
	int8_t length;
	if (ends - starts <= minRunSize)
		length = ends - starts;
	else
		length = minRunSize;
	insertionSort(starts, length);
	return length;
}

int8_t setminRunSize(uint64_t size) {
	int8_t r = 0;                     //will change to 1 if any of bits is 1
	while (size >= 64) {
		r |= size & 1;
		size >>= 1;
	}
	return size + r;
}

template<typename T>
void reverse(T *array, uint64_t length) {
	T *start = array;
	T *end = array + length - 1;

	while (start < end) {
		T x = *start;
		*start = *end;
		*end = x;
		++start;
		--end;
	}
}

template<typename T>
void merge(run<T> a1, run<T> a2) {
	/* Current index into each of the two arrays we're writing 
	from. */
	run<T> tmp;
	const uint64_t l1 = a1.length, l2 = a2.length;
	tmp.start = new T[l1];
	tmp.length = l1;
	memcpy(tmp.start, a1.start, sizeof(T) * l1);  //copying lesser(first) array to temp

	T *p1 = tmp.start, *p2 = a2.start, *end1 = tmp.start + l1, *end2 = a2.start + l2;
	// The address to which we write the next element in the merge
	T *nextMergeElement = a1.start;
	//Galloping counters
	bool leftAdd = true;
	int8_t count = 0;

	/* Iterate over the two arrays, writing the least element at the
	current position to merge_to. When the two are equal we prefer
	the left one, because if we're merging left, right we want to
	ensure stability.
	Of course this doesn't matter for integers, but it's the thought
	that counts. */
	while (p1 < end1 && p2 < end2) {
		if (*p1 <= *p2) {
			*nextMergeElement = *p1;
			++p1;
			if (leftAdd)
				++count;
			else
				count = 1;
			leftAdd = true;
			if (count > minGallop) {
				T *upTo = binarySearch(p1, *p2, end1 + 1);
				memcpy(nextMergeElement, p1, sizeof(T) * (upTo - p1));
				nextMergeElement += upTo - p1 - 1;
				p1 = upTo - 1;
			}
		}
		else {
			*nextMergeElement = *p2;
			++p2;
			if (!leftAdd)
				++count;
			else
				count = 1;
			leftAdd = false;
			if (count > minGallop) {
				T *upTo = binarySearch(p2, *p1, end2 + 1);
				memcpy(nextMergeElement, p2, sizeof(T) * (upTo - p2));
				nextMergeElement += upTo - p2 - 1;
				p2 = upTo - 1;
			}
		}
		++nextMergeElement;
	}
	// If we stopped int8_t before the end of one of the arrays
	// we now copy the rest over.
	memcpy(nextMergeElement, p1, sizeof(T) * (end1 - p1));
	delete[] tmp.start;
}

template<typename T>
void insertionSort(T *xs, int8_t length) {
	if (length <= 1)
		return;
	for (int8_t i = 1; i < length; ++i) {
		// The array before i is sorted. Now insert xs[i] into it
		T x = xs[i];
		int8_t j = i - 1;

		/* Move j down until it's either at the beginning or on
		 something <= x, and everything to the right of it has 
		 been moved up one. */
		while (j >= 0 && xs[j] > x) {
			xs[j + 1] = xs[j];
			--j;
		}
		xs[j + 1] = x;
	}
}

template<typename T>
T *binarySearch(T *start, T x, T *end) {
	if (*start > x) {
		return start + 1;
	}
	else if (*(end - 1) < x) {
		return end;
	}

	T *first = start, *last = end, *mid;

	while (first < last) {
		mid = first + (last - first) / 2;

		if (x < *mid) {
			last = mid;
		}
		else {
			first = mid + 1;
		}
	}

	return last + 1;
}

#endif