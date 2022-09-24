#pragma once

#include "nebulaxi/units/sysmon.hpp"

#include "units/unit_base.hxx"

namespace insys::nebulaxi {

struct sysmon_convert {
    double multiplier { 1. };
    uint8_t power {};
    double offset {};
    uint8_t justify {};
};

class sysmon_parser final : public unit_parser {

public:
    using unit_parser::unit_parser;
    auto get_nominals() const
    {
        sysmon_nominals nominals {};
        auto& nominals_node = m_ptree.get_child("nominals");
        nominals.temp_min = nominals_node.get<double>("temp_min");
        nominals.temp_max = nominals_node.get<double>("temp_max");
        nominals.vcc_aux = nominals_node.get<double>("vcc_aux");
        nominals.vcc_int = nominals_node.get<double>("vcc_int");
        nominals.vcc_bram = nominals_node.get<double>("vcc_bram");
        nominals.vref_n = nominals_node.get<double>("vref_n");
        nominals.vref_p = nominals_node.get<double>("vref_p");
        return nominals;
    }
    auto get_voltage_convert() const
    {
        sysmon_convert convert {};
        auto& convert_node = m_ptree.get_child("converts").get_child("voltage");
        convert.justify = convert_node.get<uint8_t>("justify");
        convert.offset = convert_node.get<double>("offset");
        convert.power = convert_node.get<uint8_t>("power");
        convert.multiplier = convert_node.get<double>("mult");
        return convert;
    }
    auto get_temperature_convert() const
    {
        sysmon_convert convert {};
        auto& convert_node = m_ptree.get_child("converts").get_child("temp");
        convert.justify = convert_node.get<uint8_t>("justify");
        convert.offset = convert_node.get<double>("offset");
        convert.power = convert_node.get<uint8_t>("power");
        convert.multiplier = convert_node.get<double>("mult");
        return convert;
    }
};

class sysmon_impl final : public sysmon_interface, public unit_base<sysmon_impl> {
    struct private_data;
    std::shared_ptr<private_data> d_ptr {};

public:
    using interface_type = sysmon_interface;
    inline static const char* type { NEBULAXI_TYPE_TO_STR(sysmon) };
    inline static constexpr is_u_type type_id { is_u_type::U_THIRD_PARTY };

    sysmon_impl(const unit_data&, const sysmon_parser&);

private:
    void reset() final;
    sysmon_nominals get_nominals() const final;
    sysmon_value get_temperature() const final;
    sysmon_value get_vcc_int() const final;
    sysmon_value get_vcc_aux() const final;
    sysmon_value get_vcc_bram() const final;
    double get_vref_p() const final;
    double get_vref_n() const final;

    double convert(sysmon_convert&, uint32_t) const noexcept;
    uint32_t reg_read(std::size_t) const;
    void reg_write(std::size_t, uint32_t) const;
};

}
