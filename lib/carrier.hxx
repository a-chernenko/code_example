#pragma once

#include "nebulaxi/carrier.hpp"

#include "config_parser.hxx"

namespace insys::nebulaxi {

class carrier_impl final : public carrier_interface {
    struct private_data;
    std::shared_ptr<private_data> d_ptr {};
    friend class mezzanine_impl;

public:
    carrier_impl(io_type type = io_type::simulate, std::size_t index = {});
    ~carrier_impl() noexcept;
private:
    void reset() final;
    std::string get_name() const noexcept final;
    std::string get_version() const noexcept final;
    std::string get_serial() const noexcept final;
    io get_io() const noexcept final;
    const subsystem_storage& subsystems() const noexcept final;
    const unit_storage& units() const noexcept final;
    const chip_storage& chips() const noexcept final;

    data_storage& storage() const noexcept;
};

class carrier_parser final : public config_base_parser {

public:
    using config_base_parser::config_base_parser;
    auto get_name() const
    {
        return m_ptree.get<std::string>("name");
    }
    auto get_subsystems() const
    {
        return m_ptree.get_child("subsystems");
    }
    auto get_units() const
    {
        return m_ptree.get_child("units");
    }
};

}
