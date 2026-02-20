#pragma once

#include <cstdint>
#include <emblib/io/std/fstream.hpp>

namespace mp::test {

inline emblib::io::std::fstream get_storage_dev()
{
    return emblib::io::std::fstream("./config.bin");
}

}