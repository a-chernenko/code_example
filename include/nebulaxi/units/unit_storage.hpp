#pragma once

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <typeindex>
#include <vector>

#include "nebulaxi/nebulaxi_types.hpp"
#include "nebulaxi/units/unit.hpp"
#include "nebulaxi/utility.hpp"

namespace insys::nebulaxi {

///
/// \brief Исключения хранилища юнитов
///
///
class unit_storage_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    unit_storage_error(const std::string&);
    virtual ~unit_storage_error() noexcept = default;
};

namespace detail {
    ///
    /// \brief Данные хранилища юнитов
    ///
    ///
    struct unit_storage_private_data {
        /// \brief Карта для хранения юнитов
        ///
        /// \tparam std::type_index Идентификатор юнита
        /// \tparam unit Юнит
        std::multimap<std::type_index, unit> map {};
    };

}

///
/// \brief Список имен всех юнитов в хранилище.
///
///
using units_names = std::vector<std::string>;

/// Хранилище юнитов
class unit_storage final {
    std::shared_ptr<detail::unit_storage_private_data> d_ptr {
        std::make_shared<detail::unit_storage_private_data>()
    };
    ///
    /// \brief Приведение типа от родительского юнита к унаследованному.
    ///
    /// \tparam derrived_type Тип унаследованной юнита.
    /// \tparam base_type Тип родительской подситемы.
    /// \param base Универсальная ссылка на родительский юнит.
    /// \return auto Умный указатель на унаследованный юнит.
    ///
    template <typename derrived_type, typename base_type = unit>
    auto unit_downcast(base_type&& base) const
    {
        return std::dynamic_pointer_cast<typename derrived_type::element_type>(base);
    }

public:
    /// Конструктор по умолчанию.
    unit_storage() = default;
    /// Конструктор копирования.
    unit_storage(const unit_storage&) = default;
    /// Конструктор перемещения.
    unit_storage(unit_storage&&) = default;
    /// Оператор копирования через присваивание.
    unit_storage& operator=(const unit_storage&) = default;
    /// Оператор перемещения через присваивание.
    unit_storage& operator=(unit_storage&&) noexcept = default;
    /// Деструктор.
    ~unit_storage() noexcept = default;
    ///
    /// \brief Добавление юнита в хранилище.
    ///
    /// \tparam unit_type Тип добавляемого юнита.
    /// \param[in] unit Добавляемый юнит.
    ///
    template <typename unit_type>
    void add(unit_type&& unit)
    {
        d_ptr->map.emplace(std::type_index(typeid(unit_type)), std::forward<unit_type>(unit));
    }
    ///
    /// \brief Получение юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \return Юнит.
    ///
    template <typename unit_type>
    unit_type get() const
    {
        auto it_unit = d_ptr->map.find(std::type_index(typeid(unit_type)));
        if (it_unit == d_ptr->map.end()) {
            throw unit_storage_error("no unit found [" + std::string(typeid(unit_type).name()) + "]");
        }
        return unit_downcast<unit_type>(it_unit->second);
    }
    ///
    /// \brief Получение юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param unit_offset Смещение юнита
    /// \return Юнит.
    ///
    template <typename unit_type>
    unit_type get(std::size_t unit_offset) const
    {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_offset() == unit_offset) {
                return unit_downcast<unit_type>(elem->second);
            }
        }
        std::stringstream hex_str;
        hex_str << std::showbase << std::hex << unit_offset;
        throw unit_storage_error("no unit found [" + hex_str.str() + "]");
    }
    ///
    /// \brief Получение юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param name Имя юнита.

    /// \return Юнит.
    ///
    template <typename unit_type>
    unit_type get(const std::string& name) const
    {
        std::string _name = to_lowercase_string(name);
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == _name) {
                return unit_downcast<unit_type>(elem->second);
            }
        }
        throw unit_storage_error("no unit found [" + _name + "]");
    }
    ///
    /// \brief Получение юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param[out] unit Юнит.
    /// \return \retval true если юнит найден \retval false если не найден.
    ///
    template <typename unit_type>
    bool get(unit_type& unit) const noexcept
    try {
        auto it_unit = d_ptr->map.find(std::type_index(typeid(unit_type)));
        if (it_unit == d_ptr->map.end()) {
            return false;
        }
        unit = unit_downcast<unit_type>(it_unit->second);
        return true;
    } catch (...) {
        return false;
    }
    ///
    /// \brief Получение юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param[out] unit Юнит.
    /// \param unit_offset Смещение юнита.
    /// \return \retval true если юнит найден \retval false если не найден.
    ///
    template <typename unit_type>
    bool get(unit_type& unit, std::size_t unit_offset) const noexcept
    try {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_offset() == unit_offset) {
                unit = unit_downcast<unit_type>(elem->second);
                return true;
            }
        }
        return false;
    } catch (...) {
        return false;
    }
    ///
    /// \brief Получение юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param[out] unit Юнит.
    /// \param name Имя юнита.
    /// \return \retval true если юнит найден \retval false если не найден.
    ///
    template <typename unit_type>
    bool get(unit_type& unit, const std::string& name) const noexcept
    try {
        std::string _name = to_lowercase_string(name);
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == _name) {
                unit = unit_downcast<unit_type>(elem->second);
                return true;
            }
        }
        return false;
    } catch (...) {
        return false;
    }
    ///
    /// \brief Удаление юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    ///
    template <typename unit_type>
    void remove()
    {
        auto it_unit = d_ptr->map.find(std::type_index(typeid(unit_type)));
        if (it_unit == d_ptr->map.end()) {
            throw unit_storage_error("no unit found [" + std::string(typeid(unit_type).name()) + "]");
        }
        d_ptr->map.erase(it_unit);
    }
    ///
    /// \brief Удаление юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param unit_offset Смещение юнита.
    ///
    template <typename unit_type>
    void remove(std::size_t unit_offset)
    {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_offset() == unit_offset) {
                d_ptr->map.erase(elem);
                break;
            }
        }
        std::stringstream hex_str;
        hex_str << std::showbase << std::hex << unit_offset;
        throw unit_storage_error("no unit found [" + hex_str.str() + "]");
    }
    ///
    /// \brief Удаление юнита из хранилища.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param name Имя юнита.
    ///
    template <typename unit_type>
    void remove(const std::string& name)
    {
        std::string _name = to_lowercase_string(name);
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == _name) {
                d_ptr->map.erase(elem);
                break;
            }
        }
        throw unit_storage_error("no unit found [" + _name + "]");
    }
    ///
    /// \brief Проверка наличия юнита в хранилище.
    ///
    /// \tparam unit_type Тип юнита.
    /// \return \retval true если юнит найден \retval false если не найден.
    ///
    template <typename unit_type>
    bool has_unit() const noexcept
    {
        auto it_unit = d_ptr->map.find(std::type_index(typeid(unit_type)));
        if (it_unit == d_ptr->map.end()) {
            return false;
        }
        return true;
    }
    ///
    /// \brief Проверка наличия юнита в хранилище.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param unit_offset Смещение юнита.
    /// \return \retval true если юнит найден \retval false если не найден.
    ///
    template <typename unit_type>
    bool has_unit(std::size_t unit_offset) const noexcept
    {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_offset() == unit_offset) {
                return true;
            }
        }
        return false;
    }
    ///
    /// \brief Проверка наличия юнита в хранилище.
    ///
    /// \tparam unit_type Тип юнита.
    /// \param name Имя юнита.
    /// \return \retval true если юнит найден \retval false если не найден.
    ///
    template <typename unit_type>
    bool has_unit(const std::string& name) const noexcept
    {
        std::string _name = to_lowercase_string(name);
        auto range = d_ptr->map.equal_range(std::type_index(typeid(unit_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == _name) {
                return true;
            }
        }
        return false;
    }
    ///
    /// \brief Получение числа юнитов в хранилище.
    ///
    /// \return \retval std::size_t число юнитов в хранилище.
    ///
    auto size() const noexcept
    {
        return d_ptr->map.size();
    }
    ///
    /// \brief Получение состояния хранилища.
    ///
    /// \return \retval true хранилище пустое, \retval false хранилище не пустое.
    ///
    auto empty() const noexcept { return d_ptr->map.empty(); }
    ///
    /// \brief Очистка хранилища юнитов.
    ///
    ///
    void clear() noexcept { d_ptr->map.clear(); }
    auto begin() noexcept { return d_ptr->map.begin(); }
    auto end() noexcept { return d_ptr->map.end(); }
    auto begin() const noexcept { return d_ptr->map.begin(); }
    auto end() const noexcept { return d_ptr->map.end(); }
    auto cbegin() const noexcept { return d_ptr->map.cbegin(); }
    auto cend() const noexcept { return d_ptr->map.cend(); }
    void merge(const unit_storage& other) noexcept { d_ptr->map.merge(other.d_ptr->map); }
};

}
