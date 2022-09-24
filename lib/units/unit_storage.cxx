#include "nebulaxi/units/unit_storage.hpp"

using namespace insys::nebulaxi;

unit_storage_error::unit_storage_error(const std::string& message)
    : nebulaxi_error(message, "[unit_storage_error]: ")
{
}
