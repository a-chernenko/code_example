#include "units/unit_builder.hxx"
#include "units/axis_fifo.hxx"
#include "units/i2c.hxx"
#if !defined(__x86_64__) && !defined(_M_X64)
#include "units/i2c_ps.hxx"
#endif
#include "units/jesd204_phy.hxx"
#include "units/jesd204c.hxx"
#include "units/reg.hxx"
#include "units/spi.hxx"

using namespace insys::nebulaxi;

units_builder::units_builder(const ::io& io, chips_builder chips_builder)
    : _io { io }
    , _chips_builder { chips_builder }
{
    if (!chips_builder.build_reg_chips) {
        throw nebulaxi_error("reg chips buidler is not set");
    }
    if (!chips_builder.build_spi_chips) {
        throw nebulaxi_error("spi chips buidler is not set");
    }
    if (!chips_builder.build_i2c_chips) {
        throw nebulaxi_error("i2c chips buidler is not set");
    }
}

std::tuple<chip_storage, unit_storage, data_storage>
units_builder::build(const config_tree& units_tree)
{
    unit_data data { _io, _storage, {}, {}, {} };
    for (auto& [str, unit_node] : units_tree) {
        unit_parser parser { unit_node };
        auto type = parser.get_type();
        data.offset = parser.get_offset();
        data.name = parser.get_name();
        data.info = parser.get_info();
        if (reg_impl::is_same_type(type)) {
            auto unit = add_unit<reg_impl>(data);
            if (auto chips_tree = parser.get_chips_optional(); chips_tree.has_value()) {
                _chips.merge(_chips_builder.build_reg_chips(unit, chips_tree.value()));
            }
        } else if (i2c_impl::is_same_type(type)) {
            auto unit = add_unit<i2c_impl>(data);
            if (auto chips_tree = parser.get_chips_optional(); chips_tree.has_value()) {
                _chips.merge(_chips_builder.build_i2c_chips(unit, chips_tree.value()));
            }
        }
#if !defined(__x86_64__) && !defined(_M_X64)
        else if (i2c_ps_impl::is_same_type(type)) {
            auto unit = add_unit<i2c_ps_impl>(data);
            if (auto chips_tree = parser.get_chips_optional(); chips_tree.has_value()) {
                _chips.merge(_chips_builder.build_i2c_chips(unit, chips_tree.value()));
            }
        }
#endif
        else if (spi_impl::is_same_type(type)) {
            auto unit = add_unit<spi_impl>(data);
            if (auto chips_tree = parser.get_chips_optional(); chips_tree.has_value()) {
                _chips.merge(_chips_builder.build_spi_chips(unit, chips_tree.value()));
            }
        } else if (axis_fifo_impl::is_same_type(type)) {
            add_unit<axis_fifo_impl>(data);
        } else if (jesd204_phy_impl::is_same_type(type)) {
            add_unit<jesd204_phy_impl>(data);
        } else if (jesd204c_impl::is_same_type(type)) {
            add_unit<jesd204c_impl>(data);
        }
    }
    return std::make_tuple(_chips, _units, _storage);
}
