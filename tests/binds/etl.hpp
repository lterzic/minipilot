#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <etl/optional.h>
#include <etl/utility.h>

namespace pybind11::detail {
namespace py = pybind11;

// Works for any optional-like type that has has_value(), value(), reset(), emplace()
template <typename T>
struct type_caster<etl::optional<T>> : public optional_caster<etl::optional<T>> {};

// Treat etl::pair like a 2-tuple. Requires .first and .second (ETL has them)
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
