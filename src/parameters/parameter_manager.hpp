#pragma once

#include <emblib/io/istream.hpp>
#include <emblib/io/ostream.hpp>

namespace mp {

class parameter_manager {
public:
    // TODO: Replace with protobuf generated type
    struct parameters_s {};

    parameter_manager(
        emblib::io::istream& istorage,
        emblib::io::ostream& ostorage
    );

    /**
     * Get parameters reference
     */
    const parameters_s& get_parameters() const noexcept
    {
        return m_parameters;
    }

private:
    /**
     * Try to load parameters from storage,
     * if successful return true
     */
    bool load_from_storage();

    /**
     * Fill parameter values with defaults
     */
    void load_default();

private:
    emblib::io::istream& m_istorage;
    emblib::io::ostream& m_ostorage;

    parameters_s m_parameters;
};

}