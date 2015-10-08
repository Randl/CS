// http://habrahabr.ru/post/247053/

#ifndef EXTERNAL_SORT_FLAT_QUICKSORT_H
#define EXTERNAL_SORT_FLAT_QUICKSORT_H

template<class ForwIt>
void insertion_sort(ForwIt be, ForwIt en) {
	for (ForwIt ii = be + 1; ii != en; ii++) {
		ForwIt jj = ii - 1;
		auto val = *ii;
		while (jj >= be and *jj > val) {
			*(jj + 1) = *jj;
			--jj;
		}
		*(jj + 1) = val;
	}
}

template<class ForwIt>
ForwIt median_of_3(ForwIt it1, ForwIt it2, ForwIt it3) {
	return (*it1 > *it2) ?
	       (*it3 > *it2) ? (*it1 > *it3) ? it3 : it1 : it2 :
	       (*it3 > *it1) ? (*it2 > *it3) ? it3 : it2 : it1;
}

template<class ForwIt>
ForwIt choose_pivot(ForwIt be, ForwIt en) {
	ptrdiff_t s = (en - be) / 8;
	ForwIt mid = be + (en - be) / 2;
	ForwIt best1 = median_of_3(be, be + s, be + 2 * s), best2 = median_of_3(mid - s, mid, mid + s), best3 = median_of_3(
		en - 2 * s, en - s, en);
	return median_of_3(best1, best2, best3);
}

// search for the end of the current block
template<class ForwIt>
ForwIt block_range(ForwIt be, ForwIt en) {
	ForwIt it = be;
	while (it != en) {
		if (*be < *it)
			return it;
		++it;
	}
	return it;
}

// warning: after the partition outer pivot may point to random element
template<class ForwIt>
std::pair<ForwIt, ForwIt> partition_range(ForwIt be, ForwIt en, ForwIt pivot) {
	std::pair<ForwIt, ForwIt> re;
	ForwIt j = be;
	for (ForwIt i = be; i != en; ++i)
		if (*i < *pivot) {
			if (pivot == i) pivot = j; else if (pivot == j) pivot = i;
			std::swap(*j, *i);
			++j;
		}
	re.first = j;
	for (ForwIt i = j; i != en; ++i)
		if (*pivot == *i) {
			if (pivot == i) pivot = j; else if (pivot == j) pivot = i;
			std::swap(*j, *i);
			++j;
		}
	re.second = j;
	return re;
}
// makes largest element the first
template<class ForwIt>
void blockify(ForwIt be, ForwIt en) {
	for (ForwIt it = be; it != en; ++it)
		if (*be < *it)
			std::swap(*be, *it);
}

template<class ForwIt>
void flat_quick_sort(ForwIt be, ForwIt en) {
	ForwIt tmp = en; // the current end of block
	while (be != en) {
		if (std::is_sorted(be, tmp)) {
			be = tmp;
			tmp = block_range(be, en);
			continue;
		}
		if (tmp - be < 32)
			insertion_sort(be, tmp);
		else {
			ForwIt pivot = choose_pivot(be, tmp);
			std::pair<ForwIt, ForwIt> range = partition_range(be, tmp, pivot);
			blockify(range.second, tmp);
			tmp = range.first;
		}
	}
}

#endif //EXTERNAL_SORT_FLAT_QUICKSORT_H
