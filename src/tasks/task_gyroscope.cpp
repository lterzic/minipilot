#include "task_gyroscope.hpp"

namespace mp {

bool
task_gyroscope::init(emblib::sensor<vector<rps_t, 3>>& sensor) noexcept
{
    // TODO: Get the average of multiple readings as drift
    // to speed up kalman bias convergence
    return true;
}

vector<rps_t, 3>
task_gyroscope::process(const vector<rps_t, 3>& raw_data) const noexcept
{
    // TODO: Add (conditional) band-pass / high-pass filtering
    return m_transform.matmul(raw_data);
}

}