#pragma once

#include <memory>
#include <string>

#include "nebulaxi/nebulaxi_error.hpp"

namespace insys::nebulaxi {

///
/// \brief Родительский класс для всех производных подсистем.
///
///
struct subsystem_interface {
    virtual void reset() = 0;
    ///
    /// \brief Получение имени подсистемы.
    ///
    /// \return std::string Имя подсистемы.
    ///
    virtual std::string get_name() const noexcept = 0;
    ///
    /// \brief Получение типа подсистемы.
    ///
    /// \return std::string Тип подсистемы.
    ///
    virtual std::string get_type() const noexcept = 0;
    ///
    /// \brief Получение расширенного описания подсистемы.
    ///
    /// \return std::string Описание подсистемы.
    ///
    virtual std::string get_info() const noexcept = 0;
    ///
    /// \brief Деструктор подсистемы.
    ///
    ///
    virtual ~subsystem_interface() noexcept = default;
};

/// Умный указатель на интерфейс подсистемы.
using subsystem = std::shared_ptr<subsystem_interface>;

///
/// \brief Приведение типа от родительской подсистемы к унаследованной.
///
/// \tparam derrived_type Тип унаследованной подсистемы.
/// \tparam base_type Тип родительской подситемы.
/// \param base Универсальная ссылка на родительскую подсистему.
/// \return auto Умный указатель на унаследованную подсистему.
///
template <typename derrived_type, typename base_type = subsystem>
auto subsystem_cast(base_type&& base)
{
    return std::dynamic_pointer_cast<typename derrived_type::element_type>(base);
}

///
/// \brief Ошибки подсистемы.
/// \details Производные подсистемы могут унаследовать этот тип.
///
class subsystem_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    subsystem_error(const std::string&);
    virtual ~subsystem_error() noexcept = default;
};

}
