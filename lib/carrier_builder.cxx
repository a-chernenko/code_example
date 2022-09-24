#include "carrier_builder.hxx"
#include "chips/chip_builder.hxx"
#include "subsystems/clock_base.hxx"
#include "subsystems/icr_carrier.hxx"
#include "subsystems/main_stream.hxx"
#include "subsystems/power.hxx"
#include "units/sysmon.hxx"

using namespace insys::nebulaxi;

carrier_builder::carrier_builder(const ::io& io)
    : units_builder {
        io,
        { insys::nebulaxi::build_i2c_chips,
            insys::nebulaxi::build_spi_chips,
            insys::nebulaxi::build_reg_chips }
    }
{
}

void carrier_builder::build_units_chips(const config_tree& units_tree)
{
    units_builder::build(units_tree);
    unit_data data { _io, _storage, {}, {}, {} };
    for (auto& [str, unit_node] : units_tree) {
        ::unit_parser parser { unit_node };
        auto type = parser.get_type();
        data.offset = parser.get_offset();
        data.name = parser.get_name();
        data.info = parser.get_info();
        if (sysmon_impl::is_same_type(type)) {
            add_unit<sysmon_impl, sysmon_parser>(data, sysmon_parser { unit_node });
        }
    }
}

template <typename subsystem_type>
void carrier_builder::subsystem_is_exist(const subsystem_data& data)
{
    if (_subsystems.has_subsystem<std::shared_ptr<typename subsystem_type::interface_type>>(data.name)) {
        throw nebulaxi_error("subsystem " + data.name + " [" + data.info + "] already exist in storage");
    }
}

template <typename subsystem_type, typename subsystem_parser>
std::shared_ptr<typename subsystem_type::interface_type>
carrier_builder::add_subsystem(const std::string& type, const subsystem_data& data, const subsystem_parser& parser)
{
    if (!subsystem_type::is_same_type(type)) {
        return std::shared_ptr<typename subsystem_type::interface_type> {};
    }
    subsystem_is_exist<subsystem_type>(data);
    auto subsystem = create_subsystem<subsystem_type>(data, parser);
    _subsystems.add(subsystem);
    return subsystem;
};

void carrier_builder::build_subsystems(const config_tree& subsystem_tree)
{
    subsystem_data data { _storage, _units, _chips, {}, {} };
    for (auto& [str, subsystem_node] : subsystem_tree) {
        subsystem_parser parser { subsystem_node };
        auto type = parser.get_type();
        data.name = parser.get_name();
        data.info = parser.get_info();
        add_subsystem<icr_carrier_impl>(type, data, icr_carrier_parser { parser() })
            || add_subsystem<clock_base_impl>(type, data, clock_base_parser { parser() })
            || add_subsystem<power_impl>(type, data, power_parser { parser() })
            || add_subsystem<main_stream_impl>(type, data, main_stream_parser { parser() })
            // TODO: добавлять по или
            || false;
    }
}
