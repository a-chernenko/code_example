#pragma once

#include <any>
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <utility>

#include "nebulaxi/nebulaxi_error.hpp"

namespace insys::nebulaxi {

/// Ошибки при работе с хранилищем данных
class data_storage_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    data_storage_error(const std::string&);
    virtual ~data_storage_error() noexcept = default;
};

namespace detail {
    ///
    /// \brief Данные хранилища
    ///
    ///
    struct data_storage_private_data {
        /// \brief Карта для хранения данных
        ///
        /// \tparam std::type_index Идентификатор данных
        /// \tparam std::any Хранитель данных
        std::map<std::string, std::pair<std::type_index, std::any>> map {};
    };
}

#define DATA_STORAGE_ADD_VALUE(STORAGE, PARAMETR) STORAGE.add(#PARAMETR, PARAMETR)
#define DATA_STORAGE_HAS_VALUE(STORAGE, PARAMETR) STORAGE.has_data<decltype(PARAMETR)>(#PARAMETR)
#define DATA_STORAGE_GET_VALUE(STORAGE, PARAMETR) PARAMETR = STORAGE.get<decltype(PARAMETR)>(#PARAMETR)
#define DATA_STORAGE_REMOVE_VALUE(STORAGE, PARAMETR) STORAGE.remove<decltype(PARAMETR)>(#PARAMETR)
#define DATA_STORAGE_VALUE_TO_STRING(PARAMETR) (#PARAMETR)

///
/// \brief Хранилище данных
/// \details Данный класс реализует хранилище данных, можно хранить любой тип данных. Поиск значения осуществляется по ключу.
///
class data_storage final {
    std::shared_ptr<detail::data_storage_private_data> d_ptr { std::make_shared<detail::data_storage_private_data>() };

public:
    data_storage() = default; ///< Конструктор по умолчанию.
    data_storage(const data_storage&) = default; ///< Конструктор копирования.
    data_storage(data_storage&&) = default; ///< Конструктор перемещения.
    data_storage& operator=(const data_storage&) = default; ///< Оператор копирования через присваивание.
    data_storage& operator=(data_storage&&) noexcept = default; ///< Оператор перемещения через присваивание.
    ~data_storage() noexcept = default; ///< Деструктор хранилища данных.

    ///
    /// \brief
    ///
    /// \tparam data_type
    /// \param name
    /// \param data
    /// \return true
    /// \return false
    ///
    template <typename data_type>
    bool add(const std::string& name, const data_type& data)
    {
        return d_ptr->map.emplace(name, std::make_pair(std::type_index(typeid(data_type)), data)).second;
    }
    template <typename data_type>
    data_type get(const std::string& name) const
    {
        auto it_data = d_ptr->map.find(name);
        if (it_data != d_ptr->map.end() && std::type_index(typeid(data_type)) == it_data->second.first) {
            try {
                return std::any_cast<data_type>(it_data->second.second);
            } catch (const std::bad_any_cast& e) {
            }
        }
        throw data_storage_error { "no data found" };
    }
    template <typename data_type>
    data_type& get_ref(const std::string& name) const
    {
        auto it_data = d_ptr->map.find(name);
        if (it_data != d_ptr->map.end() && std::type_index(typeid(data_type)) == it_data->second.first) {
            try {
                return std::any_cast<data_type&>(it_data->second.second);
            } catch (const std::bad_any_cast& e) {
            }
        }
        throw data_storage_error { "no data found" };
    }
    template <typename data_type>
    bool get(const std::string& name, data_type& data) const noexcept
    try {
        auto it_data = d_ptr->map.find(name);
        if (it_data != d_ptr->map.end() && std::type_index(typeid(data_type)) == it_data->second.first) {
            try {
                data = std::any_cast<data_type>(it_data->second.second);
                return true;
            } catch (const std::bad_any_cast& e) {
            }
        }
        return false;
    } catch (...) {
        return false;
    }
    template <typename data_type>
    void remove(const std::string& name)
    {
        auto it_data = d_ptr->map.find(name);
        if (it_data != d_ptr->map.end() && std::type_index(typeid(data_type)) == it_data->second.first) {
            try {
                static_cast<void>(std::any_cast<data_type>(it_data->second.second));
                d_ptr->map.erase(it_data);
                return;
            } catch (const std::bad_any_cast& e) {
            }
        }
        throw data_storage_error { "no data found" };
    }
    template <typename data_type>
    bool has_data(const std::string& name) const
    {
        auto it_data = d_ptr->map.find(name);
        if (it_data != d_ptr->map.end() && std::type_index(typeid(data_type)) == it_data->second.first) {
            try {
                static_cast<void>(std::any_cast<data_type>(it_data->second.second));
                return true;
            } catch (const std::bad_any_cast& e) {
            }
        }
        return false;
    }
    auto size() const noexcept { return d_ptr->map.size(); }
    auto empty() const noexcept { return d_ptr->map.empty(); }
    void clear() noexcept { d_ptr->map.clear(); }
    auto begin() noexcept { return d_ptr->map.begin(); }
    auto end() noexcept { return d_ptr->map.end(); }
    auto begin() const noexcept { return d_ptr->map.begin(); }
    auto end() const noexcept { return d_ptr->map.end(); }
    auto cbegin() const noexcept { return d_ptr->map.cbegin(); }
    auto cend() const noexcept { return d_ptr->map.cend(); }
    void merge(const data_storage& other) noexcept { d_ptr->map.merge(other.d_ptr->map); }
};
}
