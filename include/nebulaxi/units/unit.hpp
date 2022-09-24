#pragma once

#include <memory>
#include <string>

#include "nebulaxi/nebulaxi_error.hpp"

namespace insys::nebulaxi {

///
/// \brief Родительский класс для всех производных юнитов.
///
///
struct unit_interface {
    ///
    /// \brief Получение имени юнита.
    ///
    /// \return std::string Имя юнита.
    ///
    virtual std::string get_name() const noexcept = 0;
    ///
    /// \brief Получение типа юнита.
    ///
    /// \return std::string Тип юнита.
    ///
    virtual std::string get_type() const noexcept = 0;
    ///
    /// \brief Получение смещения юнита.
    ///
    /// \return std::size_t Смещение юнита.
    ///
    virtual std::size_t get_offset() const noexcept = 0;
    ///
    /// \brief Получение информации о юните.
    ///
    /// \return std::string Информация о юните.
    ///
    virtual std::string get_info() const noexcept = 0;
    ///
    /// \brief Деструктор юнита.
    ///
    ///
    virtual ~unit_interface() noexcept = default;
};

/// Умный указатель на интерфейс юнита.
using unit = std::shared_ptr<unit_interface>;

///
/// \brief Ошибки юнита.
/// \details Производные юнита могут унаследовать этот тип.
///
class unit_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    unit_error(const std::string& message);
    virtual ~unit_error() noexcept = default;
};

}
