#ifndef AS_UTILS_HPP
#define AS_UTILS_HPP

#include <godot_cpp/variant/variant.hpp>
using namespace godot;

template <typename Ty, typename Fn>
static bool any(Ty array, Fn fn) {
    for (int i = 0; i < array.size(); i++) {
        if (fn(array[i])) return true;
    }
    return false;
};

template <typename Ty, typename Fn>
static bool all(TypedArray<Ty> array, Fn fn) {
    for (int i = 0; i < array.size(); i++) {
        if (!fn(array[i])) return false;
    }
    return true;
};

template <typename Ty, typename Fn>
static void for_each(TypedArray<Ty> array, Fn fn) {
    for (int i = 0; i < array.size(); i++) {
        fn(array[i]);
    }
};

template <typename Fn>
static void for_each(Array array, Fn fn) {
    for (int i = 0; i < array.size(); i++) {
        fn(array[i]);
    }
};

template <typename Ty, typename Fn>
static void for_each_i(TypedArray<Ty> array, Fn fn) {
    for (int i = 0; i < array.size(); i++) {
        fn(array[i], i);
    }
};

template <class Fn, class First, class... Rest>
static void do_for(Fn fn, First first, Rest... rest) {
    fn(first);
    do_for(fn, rest...);
}

template <class Fn>
static void do_for(Fn fn) {
    /* Parameter pack is empty. */
}

template <class... Args>
static PackedStringArray string_array(Args... args) {
    PackedStringArray arr;
    do_for([&](auto arg) { arr.append((String)arg); }, args...);
    return arr;
}

template <class... Args>
static Array variant_array(Args... args) {
    Array arr;
    do_for([&](auto arg) { arr.append(arg); }, args...);
    return arr;
}

template <typename T, typename Key, typename Value>
static T dict_append_all(T appendable, Key key, Value val) {
    appendable[key] = val;
    return appendable;
}

template <typename T, typename Key, typename Value, typename... Args>
static T dict_append_all(T appendable, Key key, Value val, Args... args) {
    appendable[key] = val;
    return dict_append_all(appendable, args...);
}

template <typename T>
static T dict_append_all(T appendable) {
    return appendable;
}

template <class... Args>
static Dictionary dictionary(Args... args) {
    return dict_append_all(Dictionary(), args...);
}

static void try_connect(Object *obj, String signal_name, Callable callable) {
    if (obj != nullptr && !obj->is_connected(signal_name, callable))
        obj->connect(signal_name, callable);
};

static const Color contrast(Color color) {
    if (color.get_luminance() < 0.5) return Color(1, 1, 1);
    else return Color(0, 0, 0);
}

#endif