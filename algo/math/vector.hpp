#pragma once

#include <emblib/math/vector.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

namespace mp {

template <typename scalar_type, size_t SIZE>
using vector = emblib::math::vector<scalar_type, SIZE>;

using vector3f = vector<float, 3>;
using vector4f = vector<float, 4>;

}

namespace pybind11::detail {

template <
    typename Scalar,
    size_t Dim,
    typename Base
>
struct type_caster<emblib::math::vector<Scalar, Dim, Base> > {
    using wrapper_type = emblib::math::vector<Scalar, Dim, Base>;

    // this gives the python-side name; you can make it nicer if you want
    PYBIND11_TYPE_CASTER(wrapper_type, _("Vector"));

    bool load(handle src, bool convert) {
        /// Reuse the Eigen caster
        type_caster<Base> eigen_caster;
        if (!eigen_caster.load(src, convert))
            return false;

        // If Eigen conversion succeeded, stick it into our wrapper
        value = wrapper_type(cast_op<Base&>(eigen_caster));
        return true;
    }

    // cast const lvalue
    static handle cast(const wrapper_type &src,
                       return_value_policy policy,
                       handle parent)
    {
        // delegate to Eigen caster on the underlying matrix
        return type_caster<Base>::cast(src.get_base(), policy, parent);
    }

    // cast rvalue (optional but nice to have)
    static handle cast(wrapper_type &&src,
                       return_value_policy policy,
                       handle parent)
    {
        return type_caster<Base>::cast(src.get_base(), policy, parent);
    }
};

}