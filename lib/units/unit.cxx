#include "nebulaxi/units/unit.hpp"

using namespace insys::nebulaxi;

unit_error::unit_error(const std::string& message)
    : nebulaxi_error(message, "[unit_error]: ")
{
}
