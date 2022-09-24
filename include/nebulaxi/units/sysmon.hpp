#pragma once

#include "nebulaxi/units/unit.hpp"

namespace insys::nebulaxi {

///
/// \brief Тип параметров системного монитора.
///
///
struct sysmon_value {
    double min; ///< Минимальное значение.
    double max; ///< Максимальное значение.
    double value; ///< Текущее значение.
};

///
/// \brief Номинальные значения.
///
///
struct sysmon_nominals {
    double temp_min; ///< Минимально допустимая температура.
    double temp_max; ///< Максимально допустимая температура.
    double vcc_int; ///< Номинальное напряжение питания ядра.
    double vcc_aux; ///< Номинальное напряжение питания ПЛИС.
    double vcc_bram; ///< Номинальное напряжение питания блока памяти.
    double vref_p; ///< Опорное напряжение "+".
    double vref_n; ///< Опорное напряжение "-".
};

///
/// \brief Интерфейс подсистемы системного монитора.
///
///
struct sysmon_interface : virtual unit_interface {
    ///
    /// \brief Сброс.
    ///
    ///
    virtual void reset() = 0;
    ///
    /// \brief Запрос номиналов.
    ///
    /// \return Номинальные значения для данного типа ПЛИС.
    ///
    virtual sysmon_nominals get_nominals() const = 0;
    ///
    /// \brief Запрос температуры кристалла.
    ///
    /// \return Температура кристалла.
    ///
    virtual sysmon_value get_temperature() const = 0;
    ///
    /// \brief Запрос напряжения питания ядра.
    ///
    /// \return Напряжение питания ядра.
    ///
    virtual sysmon_value get_vcc_int() const = 0;
    ///
    /// \brief Запрос напряжения питания ПЛИС.
    ///
    /// \return Напряжение питания ПЛИС.
    ///
    virtual sysmon_value get_vcc_aux() const = 0;
    ///
    /// \brief Запрос напряжения питания блока памяти.
    ///
    /// \return Напряжение питания блока памяти.
    ///
    virtual sysmon_value get_vcc_bram() const = 0;
    ///
    /// \brief Запрос опорного напряжения "+".
    ///
    /// \return Опорное напряжение.
    ///
    virtual double get_vref_p() const = 0;
    ///
    /// \brief Запрос опорного напряжения "-".
    ///
    /// \return Опорное напряжение.
    ///
    virtual double get_vref_n() const = 0;

    ///
    /// \brief Деструктор подсистемы системный монитор.
    ///
    ///
    virtual ~sysmon_interface() noexcept = default;
};

///
/// \brief Умный указатель на интерфейс подсистемы системный монитор.
///
///
using sysmon = std::shared_ptr<sysmon_interface>;

class sysmon_error : public unit_error {

public:
    sysmon_error(const std::string&);
    virtual ~sysmon_error() noexcept = default;
};

}
