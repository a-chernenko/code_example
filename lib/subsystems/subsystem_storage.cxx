#include "nebulaxi/subsystems/subsystem_storage.hpp"

using namespace insys::nebulaxi;

subsystem_storage_error::subsystem_storage_error(const std::string& message)
    : nebulaxi_error(message, "[subsystem_storage_error]: ")
{
}
