#include "host.hpp"
#include "storage.hpp"
#include <mp/config/configurator.hpp>

int main()
{
    auto host = mp::test::get_host_dev();
    auto storage = mp::test::get_storage_dev();
    auto configurator = mp::configurator(storage, storage, host, host);
    return 0;
}