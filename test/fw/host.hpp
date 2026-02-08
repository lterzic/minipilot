#pragma once

#include <cstdint>
#include <emblib/io/net/udp_stream.hpp>

namespace mp::test {

inline emblib::io::net::udp_stream get_host_dev()
{
    // IP and port should match with the link.py
    const char* HOST_IP = "127.0.0.1";
    constexpr uint16_t HOST_PORT = 26200;
    constexpr uint16_t FW_PORT = 26201;

    return emblib::io::net::udp_stream(HOST_IP, HOST_PORT, FW_PORT);
}

}

