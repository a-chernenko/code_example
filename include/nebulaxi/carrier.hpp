#pragma once

#include "nebulaxi/chips/chip_storage.hpp"
#include "nebulaxi/data_storage.hpp"
#include "nebulaxi/io/io.hpp"
#include "nebulaxi/subsystems/subsystem_storage.hpp"
#include "nebulaxi/units/unit_storage.hpp"

namespace insys::nebulaxi {

struct carrier_interface {
    virtual void reset() = 0;
    virtual const subsystem_storage& subsystems() const noexcept = 0;
    virtual const unit_storage& units() const noexcept = 0;
    virtual const chip_storage& chips() const noexcept = 0;
    virtual io get_io() const noexcept = 0;
    virtual std::string get_name() const noexcept = 0;
    virtual std::string get_version() const noexcept = 0;
    virtual std::string get_serial() const noexcept = 0;
    virtual ~carrier_interface() noexcept = default;
};

using carrier = std::shared_ptr<carrier_interface>;

class carrier_creator final {
public:
    static carrier create(io_type type = {}, std::size_t index = {});
};

class carrier_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    carrier_error(const std::string&);
    virtual ~carrier_error() noexcept = default;
};

}
