#pragma once

#include <tuple>

#include "nebulaxi/chips/chip_storage.hpp"
#include "nebulaxi/units/i2c.hpp"
#include "nebulaxi/units/reg.hpp"
#include "nebulaxi/units/spi.hpp"
#include "nebulaxi/units/unit_storage.hpp"

#include "units/unit_base.hxx"

namespace insys::nebulaxi {

class units_builder {

public:
    virtual std::tuple<chip_storage, unit_storage, data_storage>
    build(const config_tree&);

    auto get_units() const noexcept { return _units; }
    auto get_chips() const noexcept { return _chips; }
    auto get_storage() const noexcept { return _storage; }

protected:
    template <typename unit_type>
    using chips_builder_abstract = chip_storage (*)(const unit_type&, const config_tree&);

    struct chips_builder {
        chips_builder_abstract<i2c> build_i2c_chips {};
        chips_builder_abstract<spi> build_spi_chips {};
        chips_builder_abstract<reg> build_reg_chips {};
    };

    units_builder(const io&, chips_builder);

    template <typename unit_type>
    void unit_is_exist(const unit_data& data)
    {
        if (_units.has_unit<std::shared_ptr<typename unit_type::interface_type>>(data.name)) {
            throw nebulaxi_error("unit " + data.name + " [" + data.info + "] already exist in storage");
        }
    }

    template <typename unit_type, typename unit_parser>
    auto add_unit(const unit_data& data, const unit_parser& parser)
    {
        unit_is_exist<unit_type>(data);
        auto unit = create_unit<unit_type, unit_parser>(data, parser);
        _units.add(unit);
        return unit;
    }

    template <typename unit_type>
    auto add_unit(const unit_data& data)
    {
        unit_is_exist<unit_type>(data);
        auto unit = create_unit<unit_type>(data);
        _units.add(unit);
        return unit;
    }

    io _io {};
    chips_builder _chips_builder {};
    unit_storage _units {};
    data_storage _storage {};
    chip_storage _chips {};
};

}
