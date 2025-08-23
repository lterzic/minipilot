#include "link/rx.hpp"
#include "vehicles/copter/copter_control.hpp"

namespace mp {

class command_handler : public rx_handler {
public:
    explicit command_handler(copter_control* copter);

    /**
     * Execute the command on the appropriate vehicle type
     */
    void handle(const mp_pb_link_UplinkMessage& msg) noexcept override;

private:
    copter_control* m_copter;
};

}