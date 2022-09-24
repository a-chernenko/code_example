#include <thread>

#include "sysmon.hxx"

using namespace std::chrono_literals;

using namespace insys::nebulaxi;

sysmon_error::sysmon_error(const std::string& message)
    : unit_error(message, "[sysmon_error]: ")
{
}

///
/// \brief Карта регистров юнита.
///
///
struct reg_offset {
    inline static const std::size_t SW_RESET = 0x000, ///< Сброс.
        SYSMON_RESET = 0x010, ///< Еще один сброс :)
        TEMP_VALUE = 0x400, ///< Температура кристалла.
        TEMP_MAX = 0x480, ///< Максимальная температура.
        TEMP_MIN = 0x490, ///< Минимальная температура.
        VCC_INT_VALUE = 0x404, ///< Напряжение питания ядра.
        VCC_INT_MAX = 0x484, ///< Максимальное напряжение питания ядра.
        VCC_INT_MIN = 0x494, ///< Минимальное напряжение питания ядра.
        VCC_AUX_VALUE = 0x408, ///< Напряжение питания ПЛИС.
        VCC_AUX_MAX = 0x488, ///< Максимальное напряжение питания ПЛИС.
        VCC_AUX_MIN = 0x498, ///< Минимальное напряжение питания ПЛИС.
        VREF_P_VALUE = 0x410, ///< Внешнее опорное напряжение (плюс).
        VREF_N_VALUE = 0x414, ///< Внешнее опорное напряжение (минус).
        VCC_BRAM_VALUE = 0x418, ///< Напряжение питания блока памяти.
        VCC_BRAM_MAX = 0x48C, ///< Максимальное напряжение питания блока памяти.
        VCC_BRAM_MIN = 0x49C; ///< Минимальное напряжение питания блока памяти.
};

///
/// \brief Данные подсистемы системного монитора.
///
///
struct sysmon_impl::private_data {

    sysmon_nominals nominals; ///< Номинальные значения для данного типа ПЛИС.
    struct {
        sysmon_convert temperature {}; ///< Параметры конвертации температуры.
        sysmon_convert voltage {}; ///< Параметры конвертации напряжения.
    } convert {}; ///< Параметры конвертации.
};

sysmon_impl::sysmon_impl(const unit_data& data, const sysmon_parser& parser)
    : unit_base(data)
    , d_ptr { std::make_shared<private_data>() }
{
    d_ptr->nominals = parser.get_nominals();
    d_ptr->convert.temperature = parser.get_temperature_convert();
    d_ptr->convert.voltage = parser.get_voltage_convert();
}

void sysmon_impl::reset()
{
    reg_write(reg_offset::SW_RESET, 0x0A);
    std::this_thread::sleep_for(1ms);
    reg_write(reg_offset::SW_RESET, 0x00);
}

double sysmon_impl::convert(sysmon_convert& convert, uint32_t value) const noexcept
{
    value >>= convert.justify;
    auto divider = (1 << convert.power);
    auto result = value * convert.multiplier / divider - convert.offset;
    return result;
}

sysmon_nominals sysmon_impl::get_nominals() const
{
    return d_ptr->nominals;
}

sysmon_value sysmon_impl::get_temperature() const
{
    sysmon_value temperature {};
    temperature.value = convert(d_ptr->convert.temperature, reg_read(reg_offset::TEMP_VALUE));
    temperature.max = convert(d_ptr->convert.temperature, reg_read(reg_offset::TEMP_MAX));
    temperature.min = convert(d_ptr->convert.temperature, reg_read(reg_offset::TEMP_MIN));
    return temperature;
}

sysmon_value sysmon_impl::get_vcc_int() const
{
    sysmon_value vcc_int {};
    vcc_int.value = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_INT_VALUE));
    vcc_int.max = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_INT_MAX));
    vcc_int.min = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_INT_MIN));
    return vcc_int;
}

sysmon_value sysmon_impl::get_vcc_aux() const
{
    sysmon_value vcc_aux {};
    vcc_aux.value = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_AUX_VALUE));
    vcc_aux.max = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_AUX_MAX));
    vcc_aux.min = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_AUX_MIN));
    return vcc_aux;
}

sysmon_value sysmon_impl::get_vcc_bram() const
{
    sysmon_value vcc_bram {};
    vcc_bram.value = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_BRAM_VALUE));
    vcc_bram.max = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_BRAM_MAX));
    vcc_bram.min = convert(d_ptr->convert.voltage, reg_read(reg_offset::VCC_BRAM_MIN));
    return vcc_bram;
}

double sysmon_impl::get_vref_p() const
{
    return convert(d_ptr->convert.voltage, reg_read(reg_offset::VREF_P_VALUE));
}

double sysmon_impl::get_vref_n() const
{
    return convert(d_ptr->convert.voltage, reg_read(reg_offset::VREF_N_VALUE));
}

uint32_t sysmon_impl::reg_read(std::size_t offset) const
{
    return io_control().reg_read(get_offset() + offset);
}

void sysmon_impl::reg_write(std::size_t offset, uint32_t value) const
{
    io_control().reg_write(get_offset() + offset, value);
}
