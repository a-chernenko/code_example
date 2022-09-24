#pragma once

#include "nebulaxi/subsystems/icr_carrier.hpp"

#include "subsystems/subsystem_base.hxx"

namespace insys::nebulaxi {

class icr_carrier_parser final : public subsystem_parser {

public:
    using subsystem_parser::subsystem_parser;
};

class icr_carrier_impl final : public icr_carrier_interface,
                               public subsystem_base<icr_carrier_impl> {
    struct private_data;
    std::shared_ptr<private_data> d_ptr {};

public:
    using interface_type = icr_carrier_interface;
    inline static const char* type { NEBULAXI_TYPE_TO_STR(icr_carrier) };

    icr_carrier_impl(const subsystem_data&, const icr_carrier_parser&);

private:
    std::string get_carrier_name() const noexcept final;
    std::string get_carrier_serial() const noexcept final;
    std::string get_carrier_type() const noexcept final;
    std::string get_carrier_version() const noexcept final;

    icr_raw_data get_raw_data() const final;
    void set_raw_data(const icr_raw_data&) final;

    template <typename chip_type>
    icr_raw_data get_raw_data_chip(chip_type& chip) const;
    template <typename chip_type>
    void set_raw_data_chip(chip_type& chip, const icr_raw_data& data);
};

}
