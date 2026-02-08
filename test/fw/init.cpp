#include <iostream>

class init {
public:
    init() {
        std::cout << "Firmware starting..." << std::endl;
    };

    ~init() {
        std::cout << "Firmware exiting..." << std::endl;
    }
};

init g_init;