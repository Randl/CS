/* This is realization of Hwang-Lin algorithm for merging two arrays of size m, n described in
 * A SIMPLE ALGORITHM FOR MERGING TWO DISJOINT LINEARLY ORDERED SETS by F. K. HWANG AND S. LIN
 * This realization uses buffer of size m that stays unchanged.
 */

template<typename Iter>
void hwang_lin_buf(Iter first1, Iter first2, Iter last, Iter buf_start) {
	ptrdiff_t m = first2 - first1, n = last - first2, buff_size = m;
	std::swap_ranges(first1, first1 + m, buf_start);
	std::rotate(first1, first2, last);// B first1 - first1 + n, A buf_start - buf_start + m
	Iter a = buf_start, b = first1, buffer = first1 + n, buffer_end = buffer + buff_size;
	while (m > 0 && n > 0) {
		if (n > m) {
			size_t alpha = (size_t) floor(log2((double) n / m)), x = n - pow(2, alpha);
			if (*(a + m - 1) < *(b + x)) {
				std::rotate(b + x, buffer, buffer_end);//pull out the set of all elements in B >= bx
				buffer = b + x; //new buffer position
				buffer_end = buffer + buff_size;
				n = x;
			} else {
				Iter place = binary_search(b + x, b + n - 1, a + m - 1);
				if (place != b + n) {
					std::rotate(place, buffer, buffer_end);  //pull out the set of all elements in B >= am
					buffer = place;
					buffer_end = buffer + buff_size;
				}
				std::swap(*(a + m - 1), *(buffer_end - 1)); //insert am
				--buff_size;
				buffer_end = buffer + buff_size;
				--m;
				n = place - b;
			}
		} else {
			size_t alpha = (size_t) floor(log2((double) m / n)), x = m - std::pow(2, alpha);
			if (*(b + n - 1) < *(a + x)) {
				std::swap_ranges(a + x, a + m, buffer_end - m + x);//pull out the set of all elements in A >= ax
				buff_size -= m - x;
				buffer_end = buffer + buff_size;
				m = x;
			} else {
				Iter place = binary_search(a + x, a + m - 1, b + n - 1);
				ptrdiff_t c_size = a + m - place;
				std::swap_ranges(place, a + m, buffer_end - c_size); //pull out the set of all elements in A >= bn
				buff_size -= c_size;
				buffer_end = buffer + buff_size;
				std::rotate(b + n - 1, buffer, buffer_end); //insert bn
				--buffer;
				buffer_end = buffer + buff_size;
				--n;
				m -= c_size;
			}
		}

	}
	if (m > 0)
		std::swap_ranges(a, a + m, buffer);
}