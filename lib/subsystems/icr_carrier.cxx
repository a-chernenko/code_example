#include <array>
#include <sstream>

#include <boost/property_tree/json_parser.hpp>

#include "chips/_93aa66.hxx"
#include "subsystems/icr_carrier.hxx"
#include "subsystems/subsystem_base.hxx"

using namespace std::chrono_literals;
using namespace std::string_literals;

using namespace insys::nebulaxi;

icr_carrier_error::icr_carrier_error(const std::string& message)
    : subsystem_error(message, "[icr_carrier_error]: ")
{
}

struct icr_carrier_memory_not_found : icr_carrier_error {

    icr_carrier_memory_not_found()
        : icr_carrier_error("memory chip not found")
    {
    }
};

struct icr_carrier_impl::private_data {
    ::_93aa66b _93aa66b {};
    std::string name {};
    std::string serial {};
    std::string type {};
    std::string version {};
};

icr_carrier_impl::icr_carrier_impl(const subsystem_data& data, const icr_carrier_parser& parser)
    : subsystem_base(data)
    , d_ptr { std::make_shared<private_data>() }
{
    auto chips_tree = parser.get_chips();
    for (auto& [str, chip] : chips_tree) {
        auto chip_name = chip.get_value<std::string>();
        for (auto& chip : chips()) {
            auto chip_type = chip.second->get_type();
            if (chip.second->get_name() == chip_name) {
                if (_93aa66b_impl::is_same_type(chip_type) && chips().has_chip<_93aa66b>(chip_name)) {
                    d_ptr->_93aa66b = chips().get<_93aa66b>(chip_name);
                }
            }
        }
    }
    if (!d_ptr->_93aa66b) {
        throw icr_carrier_error("_93aa66b chip not found");
    }
    // чтение и разбор JSON данных
    // TODO: убрать из конструктора в отдельную функцию
    // TODO: при неправильном EEPROM должна быть возможность прописать правильный
    // TODO: ICR подсистема должна быть в любом случае
    std::stringstream icr_config {};
    config_parser::tree_type ptree {};
    for (auto ch : get_raw_data()) {
        if (ch == std::byte(0))
            break;
        icr_config << char(ch);
    }
    try {
        boost::property_tree::read_json(icr_config, ptree);
        d_ptr->name = ptree.get<std::string>("name", "undefined");
        d_ptr->version = ptree.get<std::string>("version", "undefined");
        d_ptr->type = ptree.get<std::string>("type", "undefined");
        d_ptr->serial = ptree.get<std::string>("sn", "undefined");
    } catch (const std::exception& e) {
        // TODO: не перехватывать, когда вынесем в отдельную функцию
        printf("[icr] |%s|\n", e.what());
    }
}

std::string icr_carrier_impl::get_carrier_name() const noexcept { return d_ptr->name; };
std::string icr_carrier_impl::get_carrier_serial() const noexcept { return d_ptr->serial; };
std::string icr_carrier_impl::get_carrier_type() const noexcept { return d_ptr->type; };
std::string icr_carrier_impl::get_carrier_version() const noexcept { return d_ptr->version; };

icr_raw_data icr_carrier_impl::get_raw_data() const
{
    if (d_ptr->_93aa66b) {
        return get_raw_data_chip<_93aa66b>(d_ptr->_93aa66b);
    } else {
        throw icr_carrier_memory_not_found();
    }
}

void icr_carrier_impl::set_raw_data(const icr_raw_data& data)
{
    if (d_ptr->_93aa66b) {
        set_raw_data_chip<_93aa66b>(d_ptr->_93aa66b, data);
    } else {
        throw icr_carrier_memory_not_found();
    }
}

template <typename chip_type>
icr_raw_data icr_carrier_impl::get_raw_data_chip(chip_type& chip) const
{
    using vtype = typename chip_type::element_type::value_type;
    constexpr auto bytes_in_word = sizeof(vtype); // количество байтов в одном слове микросхемы
    auto chip_size_in_word = chip->size(); // размер микросхемы в словах (8, 16, 32 бита)
    icr_raw_data result {};

    for (vtype addr {}; addr < chip_size_in_word; ++addr) {
        auto value = chip->read(addr);
        for (size_t byte_num = 0; byte_num < bytes_in_word; ++byte_num)
            result.push_back(std::byte(value >> (byte_num << 3)));
    }
    return result;
}

template <typename chip_type>
void icr_carrier_impl::set_raw_data_chip(chip_type& chip, const icr_raw_data& data)
{
    using vtype = typename chip_type::element_type::value_type;
    constexpr auto bytes_in_word = sizeof(vtype); // количество байтов в одном слове микросхемы
    auto chip_size_in_word = chip->size(); // размер микросхемы в словах (8, 16, 32 бита)
    auto chip_size_in_bytes = chip_size_in_word * bytes_in_word; // размер микросхемы в байтах

    if (data.size() > chip_size_in_bytes)
        throw icr_carrier_error("the size of the data is larger than the size of the ICR");

    icr_raw_data new_data { chip_size_in_bytes };
    if (data.size() < chip_size_in_bytes)
        std::copy(data.cbegin(), data.cend(), new_data.begin());

    for (vtype addr {}; addr < chip_size_in_word; ++addr) {
        vtype value {};
        for (size_t byte_num = 0; byte_num < bytes_in_word; ++byte_num)
            value |= size_t(new_data[addr * bytes_in_word + byte_num]) << (byte_num << 3);
        chip->write(addr, value);
    }
}
