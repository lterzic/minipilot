#pragma once

#include "logger.hpp"

#define log_debug(...) mp::logger::get_instance().log(mp::log_level_e::DEBUG, __VA_ARGS__)
#define log_info(...) mp::logger::get_instance().log(mp::log_level_e::INFO, __VA_ARGS__)
#define log_warning(...) mp::logger::get_instance().log(mp::log_level_e::WARNING, __VA_ARGS__)
#define log_error(...) mp::logger::get_instance().log(mp::log_level_e::ERROR, __VA_ARGS__)