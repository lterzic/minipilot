#include "task_accelerometer.hpp"

namespace mp {

bool
task_accelerometer::init(emblib::sensor<vector<mpss_t, 3>>& sensor) noexcept
{
    // TODO: Get bias from the saved settings or if not available
    // warn the user and and ask to read multiple samples to calculate bias
    m_bias.fill(mpss_t(0));
    return true;
}

vector<mpss_t, 3>
task_accelerometer::process(const vector<mpss_t, 3>& raw_data) const noexcept
{
    // TODO: Add (optional) low-pass filtering
    
    // TODO: Update emblib so that casts don'd do lazy eval
    return m_transform.matmul(raw_data - m_bias);
}

}