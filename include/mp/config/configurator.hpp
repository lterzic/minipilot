#pragma once

#include "config/config.hpp"
#include <emblib/io/istream.hpp>
#include <emblib/io/ostream.hpp>

namespace mp {

/**
 * Configurator responsible for loading the config from storage,
 * and communicating with a host machine if available.
 */
class configurator {
public:
    configurator(
        emblib::io::istream_base<pb_byte_t>& istorage,
        emblib::io::ostream_base<pb_byte_t>& ostorage,
        emblib::io::istream_base<pb_byte_t>& ihost,
        emblib::io::ostream_base<pb_byte_t>& ohost
    );

    configurator(
        emblib::io::istream& istorage,
        emblib::io::ostream& ostorage,
        emblib::io::istream& ihost,
        emblib::io::ostream& ohost
    );

    /**
     * Get configuration
     */
    const config_s& get_config() const noexcept
    {
        return m_config;
    }

private:
    /**
     * Try to load configuration from storage,
     * if successful return true
     */
    bool load_from_storage() noexcept;

    /**
     * Request configuration update from the host.
     */
    void update_from_host() noexcept;

    /**
     * Default configuration
     */
    static config_s get_default_configuration() noexcept;

private:
    emblib::io::istream_base<pb_byte_t>& m_istorage;
    emblib::io::ostream_base<pb_byte_t>& m_ostorage;
    emblib::io::istream_base<pb_byte_t>& m_ihost;
    emblib::io::ostream_base<pb_byte_t>& m_ohost;

    config_s m_config;
};

}