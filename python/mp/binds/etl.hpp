#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <etl/array.h>
#include <etl/optional.h>
#include <etl/utility.h>
#include <etl/variant.h>

namespace pybind11::detail {
namespace py = pybind11;

// Type casting for etl::optional
template <typename T>
struct type_caster<etl::optional<T>> : public optional_caster<etl::optional<T>> {};

// Type casting for etl::variant
template <typename ...T>
struct type_caster<etl::variant<T...>> : public variant_caster<etl::variant<T...>> {};

// Type casting for etl::array
template <typename T, size_t SIZE>
struct type_caster<etl::array<T, SIZE>> : public array_caster<etl::array<T, SIZE>, T, false, SIZE> {};

// Type casting for etl::pair
template <class A, class B>
struct type_caster<etl::pair<A,B>> {
    using Pair = etl::pair<A,B>;
    using CA = make_caster<A>;
    using CB = make_caster<B>;

    PYBIND11_TYPE_CASTER(Pair,
        _("Tuple[") + CA::name + _(", ") + CB::name + _("]"));

    // Python -> C++
    bool load(py::handle src, bool convert) {
        if (!py::isinstance<py::sequence>(src)) return false;
        py::sequence s = py::reinterpret_borrow<py::sequence>(src);
        if (py::len(s) != 2) return false;

        CA a_cast;
        CB b_cast;
        if (!a_cast.load(s[0], convert)) return false;
        if (!b_cast.load(s[1], convert)) return false;

        value = Pair{ cast_op<A>(a_cast), cast_op<B>(b_cast) };
        return true;
    }

    // C++ -> Python
    static py::handle cast(const Pair& p,
                           py::return_value_policy policy,
                           py::handle parent) {
        py::object a = py::cast(p.first,  policy, parent);
        py::object b = py::cast(p.second, policy, parent);
        return py::make_tuple(a, b).release();
    }
};

}
