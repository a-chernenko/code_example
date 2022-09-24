#pragma once

#include <cinttypes>
#include <cstddef>
#include <string>

/// Пространство имен библиотеки Nebula-XI

namespace insys::nebulaxi {

///
/// \brief Дефолтная битовая маска.
///
///
struct axi_reg_bitmask_default {
    uint32_t bit_0 : 1, bit_1 : 1, bit_2 : 1, bit_3 : 1, bit_4 : 1, bit_5 : 1, bit_6 : 1, bit_7 : 1,
        bit_8 : 1, bit_9 : 1, bit_10 : 1, bit_11 : 1, bit_12 : 1, bit_13 : 1, bit_14 : 1, bit_15 : 1,
        bit_16 : 1, bit_17 : 1, bit_18 : 1, bit_19 : 1, bit_20 : 1, bit_21 : 1, bit_22 : 1, bit_23 : 1,
        bit_24 : 1, bit_25 : 1, bit_26 : 1, bit_27 : 1, bit_28 : 1, bit_29 : 1, bit_30 : 1, bit_31 : 1;
};

///
/// \brief Регистр AXI.
///
/// \tparam bitmask_type тип битовой маски регистра.
/// \tparam address_value значение адреса регистра.
///
template <typename bitmask_type, std::size_t offset_value>
class axi_reg {
    using value_type = bitmask_type;
    union data_type {
        uint32_t value;
        value_type bits;
    } _data {};

public:
    axi_reg() = default;
    axi_reg(const axi_reg& other) { operator=(other); }
    axi_reg& operator=(const axi_reg& other)
    {
        if (this != &other) {
            this->_data.value = other.value;
        }
        return *this;
    }
    axi_reg(axi_reg&&) noexcept = default;
    axi_reg& operator=(axi_reg&&) noexcept = default;
    axi_reg(uint32_t value) { operator=(value); }
    axi_reg& operator=(uint32_t value)
    {
        this->_data.value = value;
        return *this;
    }
    operator uint32_t()
    {
        return _data.value;
    }
    axi_reg(const value_type& bitmask)
        : _data { bitmask }
    {
    }
    virtual ~axi_reg() noexcept = default;
    auto operator->() noexcept { return &_data.bits; }
    auto operator->() const noexcept { return &_data.bits; }
    value_type volatile& bits { _data.bits };
    uint32_t volatile& value { _data.value };
    static constexpr std::size_t offset { offset_value };
};

///
/// \brief Тип обобщенного регистра с битовым представлением.
///
/// \tparam offset_value смещение адреса регистра.
///
template <std::size_t offset_value>
using axi_reg_default = axi_reg<axi_reg_bitmask_default, offset_value>;

///
/// \brief Битовое поле регистра.
///
/// \tparam offset_value значение адреса регистра.
/// \tparam bit_offset_value смещение первого бита поля.
/// \tparam bit_width_value ширина поля в битах.
/// \tparam value_type тип регистра.
///
template <std::size_t offset_value, std::size_t bit_offset_value, std::size_t bit_width_value, typename value_type = std::size_t>
class reg_field {
    value_type _value {};

public:
    using reg_type = value_type;
    static constexpr std::size_t offset = offset_value;
    static constexpr std::size_t bit_offset = bit_offset_value;
    static constexpr std::size_t bit_mask = ((1 << bit_width_value) - 1) << bit_offset_value;

    constexpr reg_field(const reg_field&) noexcept = default;
    constexpr reg_field& operator=(const reg_field&) noexcept = default;
    constexpr reg_field(reg_field&&) noexcept = default;
    constexpr reg_field& operator=(reg_field&&) noexcept = default;
    constexpr reg_field(reg_type value) { operator=(value); }
    constexpr reg_field& operator=(reg_type value)
    {
        _value = value;
        return *this;
    }
    constexpr operator reg_type()
    {
        return _value;
    }
    constexpr reg_type get_field()
    {
        return (_value & reg_field::bit_mask) >> reg_field::bit_offset;
    }
    constexpr reg_field& set_field(reg_type value)
    {
        _value &= ~reg_field::bit_mask;
        _value |= (value << reg_field::bit_offset) & reg_field::bit_mask;
        return *this;
    }
};

///
/// \brief Битовое поле регистра AXI.
///
template <std::size_t offset_value, std::size_t bit_offset_value, std::size_t bit_width_value>
using axi_field = reg_field<offset_value, bit_offset_value, bit_width_value, uint32_t>;

///
/// \brief Битовое поле регистра DRP.
///
template <std::size_t offset_value, std::size_t bit_offset_value, std::size_t bit_width_value>
using drp_field = reg_field<offset_value, bit_offset_value, bit_width_value, uint32_t>;

///
/// \brief Супер ENUM :-)
///
template <typename type>
struct enum_class {
    using value_type = type;
    constexpr enum_class() = default;
    constexpr enum_class(const enum_class&) = default;
    constexpr enum_class(enum_class&&) noexcept = default;
    constexpr enum_class& operator=(const enum_class&) = default;
    constexpr enum_class& operator=(enum_class&&) noexcept = default;
    constexpr enum_class(value_type value)
        : _value { value }
    {
    }
    constexpr enum_class& operator=(value_type value)
    {
        _value = value;
        return *this;
    }
    constexpr operator value_type() const
    {
        return _value;
    }
    constexpr value_type value() const
    {
        return _value;
    }

private:
    value_type _value {};
};

#define NEBULAXI_TYPE_TO_STR(type) #type

}
