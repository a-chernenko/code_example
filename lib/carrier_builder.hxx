#pragma once

#include "nebulaxi/subsystems/subsystem_storage.hpp"

#include "subsystems/subsystem_base.hxx"
#include "units/unit_builder.hxx"

namespace insys::nebulaxi {

class carrier_builder final : public units_builder {
    subsystem_storage _subsystems {};

    template <typename subsystem_type>
    void subsystem_is_exist(const subsystem_data&);
    template <typename subsystem_type, typename subsystem_parser>
    std::shared_ptr<typename subsystem_type::interface_type>
    add_subsystem(const std::string&, const subsystem_data&, const subsystem_parser&);

public:
    carrier_builder(const io&);

    void build_units_chips(const config_tree&);
    void build_subsystems(const config_tree&);

    auto get_subsystems() const noexcept { return _subsystems; }
};

}
