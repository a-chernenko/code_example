#pragma once

#include <memory>
#include <string>

#include "nebulaxi/units/reg.hpp"
#include "nebulaxi/chips/chip_storage.hpp"
#include "nebulaxi/data_storage.hpp"
#include "nebulaxi/nebulaxi_types.hpp"
#include "nebulaxi/subsystems/subsystem.hpp"
#include "nebulaxi/units/unit_storage.hpp"

#include "config_parser.hxx"
#include "logger.hxx"

namespace insys::nebulaxi {

struct subsystem_data {
    logger::log_type log {};
    chip_storage chips {};
    unit_storage units {};
    data_storage storage {};
    std::string name {};
    std::string info {};
    subsystem_data(data_storage storage, unit_storage units, chip_storage chips, std::string name, std::string info = {})
        : storage { std::move(storage) }
        , units { std::move(units) }
        , chips { std::move(chips) }
        , name { std::move(name) }
        , info { std::move(info) }
    {
    }
};

template <typename subsystem_derrived>
class subsystem_base : public virtual subsystem_interface {
    std::shared_ptr<subsystem_data> d_ptr {};

protected:
    using base = subsystem_base<subsystem_derrived>;

    subsystem_base(const subsystem_data& data)
        : d_ptr { std::make_shared<subsystem_data>(data) }
    {
        auto logger_name = d_ptr->name + ":" + std::string(subsystem_derrived::type);
        d_ptr->log = logger::create_log(logger_name);
        d_ptr->log->debug("subsystem created");
    }
    virtual ~subsystem_base() noexcept
    {
        d_ptr->log->debug("subsystem destroyed");
        logger::drop_log(d_ptr->log);
    }
    logger::log_type::element_type& log() const { return *d_ptr->log; }
    chip_storage& chips() const noexcept { return d_ptr->chips; }
    unit_storage& units() const noexcept { return d_ptr->units; }
    data_storage& storage() const noexcept { return d_ptr->storage; }
    std::string get_type() const noexcept final { return subsystem_derrived::type; }
    std::string get_name() const noexcept final { return d_ptr->name; }
    std::string get_info() const noexcept final { return d_ptr->info; }
    void reset() override {};

    template <typename axi_field_type>
    static uint32_t reg_field_read(const reg_interface &reg)
    {
        return axi_field_type { reg.read(axi_field_type::offset) }.get_field();
    }

    template <typename axi_field_type>
    static void reg_field_write(const reg_interface &reg, uint32_t value)
    {
        auto reg_value = axi_field_type { reg.read(axi_field_type::offset) };
        reg.write(axi_field_type::offset, reg_value.set_field(value));
    }

public:
    static bool is_same_type(const std::string& type) noexcept
    {
        return (subsystem_derrived::type == type);
    }
};

template <typename subsystem_type, typename subsystem_parser>
std::shared_ptr<typename subsystem_type::interface_type> create_subsystem(
    const subsystem_data& data, const subsystem_parser& parser)
{
    return std::make_shared<subsystem_type>(data, parser);
}

class subsystem_parser : public config_base_parser {

public:
    using config_base_parser::config_base_parser;
    virtual ~subsystem_parser() noexcept = default;

    auto get_name() const
    {
        return m_ptree.get<std::string>("name");
    }
    auto get_type() const
    {
        return m_ptree.get<std::string>("type");
    }
    auto get_info() const
    {
        return m_ptree.get_optional<std::string>("info").get_value_or(std::string {});
    }
    auto get_timeout() const
    {
        auto timeout = m_ptree.get_optional<std::size_t>("timeout").get_value_or(1000);
        return std::chrono::milliseconds(timeout);
    }
    auto get_units() const
    {
        return m_ptree.get_child("units");
    }
    auto get_units_optional() const
    {
        return m_ptree.get_child_optional("units");
    }
    auto get_chips() const
    {
        return m_ptree.get_child("chips");
    }
    auto get_chips_optional() const
    {
        return m_ptree.get_child_optional("chips");
    }
};
}
