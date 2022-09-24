#pragma once

#include <cstddef>
#include <vector>

#include "nebulaxi/subsystems/subsystem.hpp"

namespace insys::nebulaxi {

using icr_raw_data = std::vector<std::byte>;

struct icr_carrier_interface : virtual subsystem_interface {
    virtual icr_raw_data get_raw_data() const = 0;
    virtual void set_raw_data(const icr_raw_data&) = 0;

    virtual std::string get_carrier_name() const noexcept = 0;
    virtual std::string get_carrier_serial() const noexcept = 0;
    virtual std::string get_carrier_type() const noexcept = 0;
    virtual std::string get_carrier_version() const noexcept = 0;

    virtual ~icr_carrier_interface() noexcept = default;
};

using icr_carrier = std::shared_ptr<icr_carrier_interface>;

class icr_carrier_error : public subsystem_error {

public:
    icr_carrier_error(const std::string&);
    virtual ~icr_carrier_error() noexcept = default;
};

}
