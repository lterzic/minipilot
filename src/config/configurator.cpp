#include "config/configurator.hpp"
#include "common/chrono.hpp"
#include "common/pb.hpp"
#include "logging/logging.hpp"
#include <pblink/config/host.nanopb.h>
#include <pb_encode.h>
#include <pb_decode.h>

namespace mp {

configurator::configurator(
    emblib::io::istream_base<pb_byte_t>& istorage,
    emblib::io::ostream_base<pb_byte_t>& ostorage,
    emblib::io::istream_base<pb_byte_t>& ihost,
    emblib::io::ostream_base<pb_byte_t>& ohost
) :
    m_istorage(istorage),
    m_ostorage(ostorage),
    m_ihost(ihost),
    m_ohost(ohost)
{
    if (!load_from_storage()) {
        log_info("No saved config found, loading default");
        m_config = get_default_configuration();
    } else {
        log_info("Found saved config");
    }
    
    update_from_host();
}

configurator::configurator(
    emblib::io::istream& istorage,
    emblib::io::ostream& ostorage,
    emblib::io::istream& ihost,
    emblib::io::ostream& ohost
) :
    configurator(
        reinterpret_cast<emblib::io::istream_base<pb_byte_t>&>(istorage),
        reinterpret_cast<emblib::io::ostream_base<pb_byte_t>&>(ostorage),
        reinterpret_cast<emblib::io::istream_base<pb_byte_t>&>(ihost),
        reinterpret_cast<emblib::io::ostream_base<pb_byte_t>&>(ohost)
    )
{}

bool configurator::load_from_storage() noexcept
{
    pb_byte_t buffer[PBLINK_CONFIG_CONFIG_SIZE];
    ssize_t status = m_istorage.read(buffer, sizeof(buffer), milliseconds_t(10));
    
    if (status <= 0) {
        log_error("Failed to read from configuration storage device");
        return false;
    }

    pb_istream_t buf_istream = pb_istream_from_buffer(buffer, status);
    return pb_decode(&buf_istream, PBLINK_CONFIG_CONFIG_FIELDS, &m_config);
}

void configurator::update_from_host() noexcept
{
    // Only one buffer is used at a time
    union buffer_u {
        pb_byte_t request[PBLINK_CONFIG_CONFIG_HOST_REQUEST_SIZE];
        pb_byte_t ack[PBLINK_CONFIG_CONFIG_HOST_ACK_SIZE];
        pb_byte_t update[PBLINK_CONFIG_CONFIG_HOST_UPDATE_SIZE];
        pb_byte_t save[PBLINK_CONFIG_CONFIG_SIZE];
    } buffer;

    // Request update from host
    constexpr auto host_timeout = milliseconds_t(3000);

    pb_ostream_t request_ostream = pb_ostream_from_buffer(buffer.request, sizeof(buffer.request));
    pblink::config::config_host_request_s request {
        .has_request_timeout = true,
        .request_timeout = pb_from(host_timeout),
        .has_current_config = true,
        .current_config = m_config
    };

    if (!pb_encode(&request_ostream, PBLINK_CONFIG_CONFIG_HOST_REQUEST_FIELDS, &request)) {
        log_error("Failed to encode request");
        return;
    }
    
    m_ohost.write(buffer.request, request_ostream.bytes_written, milliseconds_t(10));
    if (m_ihost.read(buffer.ack, sizeof(buffer.ack), host_timeout) < 0) {
        log_info("No host response");
        return;
    }

    // Host has responeded
    pblink::config::config_host_update_s update;
    while (true) {
        ssize_t read_status = m_ihost.read(buffer.update, sizeof(buffer.update), milliseconds_t(-1));
        if (read_status <= 0) {
            log_error("Host read update failed");
            return;
        }
        
        pb_istream_t update_istream = pb_istream_from_buffer(buffer.update, read_status);
        if (!pb_decode(&update_istream, PBLINK_CONFIG_CONFIG_HOST_UPDATE_FIELDS, &update)) {
            log_error("Failed to decode update");
            return;
        }

        if (update.has_config) {
            m_config = update.config;
        }

        if (update.complete) {
            break;
        }
    }

    pb_ostream_t save_stream = pb_ostream_from_buffer(buffer.save, sizeof(buffer.save));
    if (!pb_encode(&save_stream, PBLINK_CONFIG_CONFIG_FIELDS, &update.config)) {
        log_error("Failed to encode new config");
        return;
    }

    if (m_ostorage.write(buffer.save, save_stream.bytes_written, milliseconds_t(100)) < 0) {
        log_error("Failed to save new config");
    } else {
        log_info("Saved new config");
    }
}

config_s configurator::get_default_configuration() noexcept
{
    return {0};
}

}