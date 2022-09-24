#include "nebulaxi/subsystems/subsystem.hpp"

using namespace insys::nebulaxi;

subsystem_error::subsystem_error(const std::string& message)
    : nebulaxi_error(message, "[subsystem_error]: ")
{
}
