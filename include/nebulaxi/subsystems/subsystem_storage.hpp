#pragma once

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <typeindex>
#include <vector>

#include "nebulaxi/nebulaxi_types.hpp"
#include "nebulaxi/subsystems/subsystem.hpp"

namespace insys::nebulaxi {

///
/// \brief Исключения хранилища подсистем
///
///
class subsystem_storage_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    subsystem_storage_error(const std::string&);
    virtual ~subsystem_storage_error() noexcept = default;
};

namespace detail {
    ///
    /// \brief Данные хранилища подсистем
    ///
    ///
    struct subsystem_storage_private_data {
        /// \brief Карта для хранения подсистем
        ///
        /// \tparam std::type_index Идентификатор подсистемы
        /// \tparam subsystem Подсистема
        std::multimap<std::type_index, subsystem> map {};
    };

}

///
/// \brief Список имен всех подсистем в хранилище.
///
///
using subsystems_names = std::vector<std::string>;

/// Хранилище подсистем
class subsystem_storage final {
    std::shared_ptr<detail::subsystem_storage_private_data> d_ptr {
        std::make_shared<detail::subsystem_storage_private_data>()
    };

    ///
    /// \brief Приведение типа от родительской подсистемы к унаследованной.
    ///
    /// \tparam derrived_type Тип унаследованной подсистемы.
    /// \tparam base_type Тип родительской подситемы.
    /// \param base Универсальная ссылка на родительскую подсистему.
    /// \return auto Умный указатель на унаследованную подсистему.
    ///
    template <typename derrived_type, typename base_type = subsystem>
    auto subsystem_downcast(base_type&& base) const
    {
        return std::dynamic_pointer_cast<typename derrived_type::element_type>(base);
    }

public:
    /// Конструктор по умолчанию.
    subsystem_storage() = default;
    /// Конструктор копирования.
    subsystem_storage(const subsystem_storage&) = default;
    /// Конструктор перемещения.
    subsystem_storage(subsystem_storage&&) = default;
    /// Оператор копирования через присваивание.
    subsystem_storage& operator=(const subsystem_storage&) = default;
    /// Оператор перемещения через присваивание.
    subsystem_storage& operator=(subsystem_storage&&) noexcept = default;
    /// Деструктор.
    ~subsystem_storage() noexcept = default;
    ///
    /// \brief Добавление подсистемы в хранилище.
    ///
    /// \tparam subsystem_type Тип добавляемой подсистемы.
    /// \param[in] subsystem Добавляемая подсистема.
    ///
    template <typename subsystem_type>
    void add(subsystem_type&& subsystem)
    {
        d_ptr->map.emplace(std::type_index(typeid(subsystem_type)), std::forward<subsystem_type>(subsystem));
    }
    ///
    /// \brief Получение подсистемы из хранилища.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \return Подсистема.
    ///
    template <typename subsystem_type>
    subsystem_type get() const
    {
        auto it_subsystem = d_ptr->map.find(std::type_index(typeid(subsystem_type)));
        if (it_subsystem == d_ptr->map.end()) {
            throw subsystem_storage_error("no subsystem found [" + std::string(typeid(subsystem_type).name()) + "]");
        }
        return subsystem_downcast<subsystem_type>(it_subsystem->second);
    }
    ///
    /// \brief Получение подсистемы с базовым интерфейсом из хранилища.
    ///
    /// \tparam base_subsystem_type Базовый тип подсистемы.
    /// \return Подсистема.
    ///
    template <typename base_subsystem_type>
    base_subsystem_type get_base() const
    {
        for (auto& [type_index, subsystem] : d_ptr->map) {
            if (subsystem_downcast<base_subsystem_type>(subsystem)) {
                return subsystem_downcast<base_subsystem_type>(subsystem);
            }
        }
        throw subsystem_storage_error("no subsystem found [" + std::string(typeid(base_subsystem_type).name()) + "]");
    }
    ///
    /// \brief Получение подсистемы из хранилища.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \param name Имя подсистемы

    /// \return Подсистема.
    ///
    template <typename subsystem_type>
    subsystem_type get(const std::string& name) const
    {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(subsystem_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == name) {
                return subsystem_downcast<subsystem_type>(elem->second);
            }
        }
        throw subsystem_storage_error("no subsystem found [" + name + "]");
    }
    ///
    /// \brief Получение подсистемы с базовым интерфейсом из хранилища.
    ///
    /// \tparam base_subsystem_type Базовый тип подсистемы.
    /// \param name Имя подсистемы

    /// \return Подсистема.
    ///
    template <typename base_subsystem_type>
    base_subsystem_type get_base(const std::string& name) const
    {
        for (auto& [type_index, subsystem] : d_ptr->map) {
            if (subsystem_downcast<base_subsystem_type>(subsystem)
                && subsystem->get_name() == name) {
                return subsystem_downcast<base_subsystem_type>(subsystem);
            }
        }
        throw subsystem_storage_error("no subsystem found [" + name + "]");
    }
    ///
    /// \brief Получение подсистемы из хранилища.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \param[out] subsystem Подсистема.
    /// \return \retval true если подсистема найдена \retval false если не найдена.
    ///
    template <typename subsystem_type>
    bool get(subsystem_type& subsystem) const noexcept
    try {
        auto it_subsystem = d_ptr->map.find(std::type_index(typeid(subsystem_type)));
        if (it_subsystem == d_ptr->map.end()) {
            return false;
        }
        subsystem = subsystem_downcast<subsystem_type>(it_subsystem->second);
        return true;
    } catch (...) {
        return false;
    }
    ///
    /// \brief Получение подсистемы из хранилища.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \param[out] subsystem Подсистема.
    /// \param name Имя подсистемы
    /// \return \retval true если подсистема найдена \retval false если не найдена.
    ///
    template <typename subsystem_type>
    bool get(subsystem_type& subsystem, const std::string& name) const noexcept
    try {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(subsystem_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == name) {
                subsystem = subsystem_downcast<subsystem_type>(elem->second);
                return true;
            }
        }
        return false;
    } catch (...) {
        return false;
    }
    ///
    /// \brief Удаление подсистемы из хранилища.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    ///
    template <typename subsystem_type>
    void remove()
    {
        auto it_subsystem = d_ptr->map.find(std::type_index(typeid(subsystem_type)));
        if (it_subsystem == d_ptr->map.end()) {
            throw subsystem_storage_error("no subsystem found");
        }
        d_ptr->map.erase(it_subsystem);
    }
    ///
    /// \brief Удаление подсистемы из хранилища.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \param name Имя подсистемы
    ///
    template <typename subsystem_type>
    void remove(const std::string& name)
    {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(subsystem_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == name) {
                d_ptr->map.erase(elem);
                break;
            }
        }
        throw subsystem_storage_error("no subsystem found [" + name + "]");
    }
    ///
    /// \brief Проверка наличия подсистемы в хранилище.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \return \retval true если подсистема найдена \retval false если не найдена.
    ///
    template <typename subsystem_type>
    bool has_subsystem() const noexcept
    {
        auto it_subsystem = d_ptr->map.find(std::type_index(typeid(subsystem_type)));
        if (it_subsystem == d_ptr->map.end()) {
            return false;
        }
        return true;
    }
    ///
    /// \brief Проверка наличия подсистемы с базовым интерфейсом в хранилище.
    ///
    /// \tparam base_subsystem_type Базовый тип подсистемы.
    /// \return \retval true если подсистема найдена \retval false если не найдена.
    ///
    template <typename base_subsystem_type>
    bool has_base() const noexcept
    {
        for (auto& [type_index, subsystem] : d_ptr->map) {
            if (subsystem_downcast<base_subsystem_type>(subsystem)) {
                return true;
            }
        }
        return false;
    }
    ///
    /// \brief Проверка наличия подсистемы в хранилище.
    ///
    /// \tparam subsystem_type Тип подсистемы.
    /// \param name Имя подсистемы
    /// \return \retval true если подсистема найдена \retval false если не найдена.
    ///
    template <typename subsystem_type>
    bool has_subsystem(const std::string& name) const noexcept
    {
        auto range = d_ptr->map.equal_range(std::type_index(typeid(subsystem_type)));
        for (auto elem = range.first; elem != range.second; ++elem) {
            if (elem->second->get_name() == name) {
                return true;
            }
        }
        return false;
    }
    ///
    /// \brief Проверка наличия подсистемы с базовым интерфейсом в хранилище.
    ///
    /// \tparam base_subsystem_type Базовый тип подсистемы.
    /// \param name Имя подсистемы
    /// \return \retval true если подсистема найдена \retval false если не найдена.
    ///
    template <typename base_subsystem_type>
    bool has_base(const std::string& name) const noexcept
    {
        for (auto& [type_index, subsystem] : d_ptr->map) {
            if (subsystem_downcast<base_subsystem_type>(subsystem)
                && subsystem->get_name() == name) {
                return true;
            }
        }
        return false;
    }
    ///
    /// \brief Получение числа подсистем в хранилище.
    ///
    /// \return \retval std::size_t число подсистем в хранилище.
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
    /// \brief Очистка хранилища подсистем.
    ///
    ///
    void clear() noexcept { d_ptr->map.clear(); }
    auto begin() noexcept { return d_ptr->map.begin(); }
    auto end() noexcept { return d_ptr->map.end(); }
    auto begin() const noexcept { return d_ptr->map.begin(); }
    auto end() const noexcept { return d_ptr->map.end(); }
    auto cbegin() const noexcept { return d_ptr->map.cbegin(); }
    auto cend() const noexcept { return d_ptr->map.cend(); }
    void merge(const subsystem_storage& other) noexcept { d_ptr->map.merge(other.d_ptr->map); }
};

}
