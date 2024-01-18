#ifndef AS_UTILS_HPP
#define AS_UTILS_HPP

template <typename Ty, typename Fn>
bool any(TypedArray<Ty> array, Fn fn) {
	for (int i = 0; i < array.size(); i++) {
		if (fn(array[i]))
			return true;
	}
	return false;
};

template <typename Ty, typename Fn>
bool all(TypedArray<Ty> array, Fn fn) {
	for (int i = 0; i < array.size(); i++) {
		if (!fn(array[i]))
			return false;
	}
	return true;
};

template <typename Ty, typename Fn>
void for_each(TypedArray<Ty> array, Fn fn) {
	for (int i = 0; i < array.size(); i++) {
		fn(array[i]);
	}
};

template <typename Ty, typename Fn>
void for_each_i(TypedArray<Ty> array, Fn fn) {
	for (int i = 0; i < array.size(); i++) {
		fn(array[i], i);
	}
};

template <class Fn, class First, class... Rest>
void do_for(Fn fn, First first, Rest... rest) {
	fn(first);
	do_for(fn, rest...);
}

template <class Fn>
void do_for(Fn fn) {
	/* Parameter pack is empty. */
}

template <class... Args>
Array variant_array(Args... args) {
	Array arr;
	do_for([&](auto arg) { arr.append(arg); }, args...);
	return arr;
}

#endif