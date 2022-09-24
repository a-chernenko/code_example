#pragma once

#include <memory>
#include <string>

#include "nebulaxi/data_storage.hpp"
#include "nebulaxi/io/io.hpp"
#include "nebulaxi/nebulaxi_types.hpp"
#include "nebulaxi/utility.hpp"

#include "config_parser.hxx"
#include "is_unit_id.hxx"
#include "logger.hxx"

namespace insys::nebulaxi {

struct unit_data {
    logger::log_type log {};
    insys::nebulaxi::io io {};
    data_storage storage {};
    std::size_t offset {};
    std::string name {};
    std::string info {};
    unit_data(insys::nebulaxi::io io, data_storage storage, std::size_t offset, std::string name, std::string info = {})
        : io { std::move(io) }
        , storage { std::move(storage) }
        , offset { offset }
        , name { std::move(name) }
        , info { std::move(info) }
    {
    }
};

template <typename unit_derrived>
class unit_base : public virtual unit_interface {

    std::shared_ptr<unit_data> d_ptr {};

protected:
    using base = unit_base<unit_derrived>;

    unit_base(const unit_data& data)
        : d_ptr { std::make_shared<unit_data>(data) }
    {
        std::stringstream offset_hex {};
        offset_hex << std::showbase << std::hex << d_ptr->offset;
        auto logger_name = d_ptr->name + ":" + offset_hex.str();
        d_ptr->log = logger::create_log(logger_name);
        if constexpr (unit_derrived::type_id != is_u_type::NOT_SUPPORTED) {
            is_unit_reg_id unit_id { d_ptr->io->reg_read(d_ptr->offset + is_unit_reg_id::get_offset()) };
            if (!is_unit_id_valid<unit_derrived::type_id>(unit_id)) {
                auto error_message = "unit " + std::string { unit_derrived::type } + " not found [" + offset_hex.str() + "]";
                throw unit_error(error_message);
            }
        }
        d_ptr->log->debug("unit created");
    }
    virtual ~unit_base() noexcept
    {
        d_ptr->log->debug("unit destroyed");
        logger::drop_log(d_ptr->log);
    }

    logger::log_type::element_type& log() const { return *d_ptr->log; }
    io_interface& io_control() const { return *d_ptr->io; }
    data_storage& storage() const noexcept { return d_ptr->storage; }
    std::size_t get_offset() const noexcept final { return d_ptr->offset; }
    std::string get_type() const noexcept final { return unit_derrived::type; }
    std::string get_name() const noexcept final { return d_ptr->name; }
    std::string get_info() const noexcept final { return d_ptr->info; }

    template <typename axi_reg_type>
    axi_reg_type reg_read() const
    {
        return axi_reg_type { reg_read(axi_reg_type::offset) };
    }
    template <typename axi_reg_type>
    void reg_write(const axi_reg_type& reg) const
    {
        reg_write(axi_reg_type::offset, reg.value);
    }
    uint32_t reg_read(std::size_t offset) const
    {
        return io_control().reg_read(get_offset() + offset);
    }
    void reg_write(std::size_t offset, uint32_t value) const
    {
        io_control().reg_write(get_offset() + offset, value);
    }

    template <typename axi_field_type>
    uint32_t field_read() const
    {
        return axi_field_type { reg_read(axi_field_type::offset) }.get_field();
    }
    template <typename axi_field_type>
    void field_write(uint32_t value) const
    {
        auto reg_value = axi_field_type { reg_read(axi_field_type::offset) };
        reg_write(axi_field_type::offset, reg_value.set_field(value));
    }

public:
    static bool is_same_type(const std::string& type) noexcept
    {
        return (unit_derrived::type == type);
    }
};

template <typename unit_type>
std::shared_ptr<typename unit_type::interface_type> create_unit(const unit_data& data)
{
    return std::make_shared<unit_type>(data);
}

template <typename unit_type, typename unit_parser>
std::shared_ptr<typename unit_type::interface_type> create_unit(const unit_data& data, const unit_parser& parser)
{
    return std::make_shared<unit_type>(data, parser);
}

class unit_parser : public config_base_parser {

public:
    using config_base_parser::config_base_parser;
    virtual ~unit_parser() noexcept = default;

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
    auto get_offset() const
    {
        return std::strtol(m_ptree.get<std::string>("offset").c_str(), nullptr, 16);
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
