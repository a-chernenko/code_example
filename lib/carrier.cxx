#include "nebulaxi/subsystems/icr_carrier.hpp"

#include "carrier.hxx"
#include "carrier_builder.hxx"
#include "io/io.hxx"

using namespace std::string_literals;

using namespace insys::nebulaxi;

carrier_error::carrier_error(const std::string& message)
    : nebulaxi_error(message, "[carrier_error]: ")
{
}

struct carrier_impl::private_data {
    logger::log_type log {};
    ::io io {};
    data_storage storage {};
    subsystem_storage subsystems {};
    unit_storage units {};
    chip_storage chips {};
    std::string name { "unknown" };
    std::string version { "unknown" };
    std::string serial { "unknown" };
};

carrier_impl::carrier_impl(io_type type, std::size_t index)
    : d_ptr { std::make_shared<private_data>() }
{
    auto logger_name = "carrier:" + std::to_string(index);
    d_ptr->log = logger::create_log(logger_name);
    d_ptr->io = io_impl::create(type, index);
    auto location = d_ptr->io->get_location();
    ::config_parser config_parser {};
    auto device_id = d_ptr->io->get_board_info().device_id;
    if (auto filename = config_parser::get_carrier_filename(device_id);
        config_parser::is_file_exist(filename)) {
        config_parser = ::config_parser { filename };
        d_ptr->log->debug("parsing configuration file: {}", filename.string());
    } else {
        d_ptr->log->warn("configuration file not found");
        d_ptr->log->debug("carrier created");
        return;
    }
    ::carrier_parser carrier_parser { config_parser.get_carrier() };
    ::carrier_builder carrier_builder(d_ptr->io);
    carrier_builder.build_units_chips(carrier_parser.get_units());
    carrier_builder.build_subsystems(carrier_parser.get_subsystems());
    d_ptr->subsystems = carrier_builder.get_subsystems();
    auto icr_carrier = d_ptr->subsystems.get<::icr_carrier>();
    auto version = icr_carrier->get_carrier_version();
    if (!version.empty()) {
        d_ptr->version = version;
    }
    auto serial = icr_carrier->get_carrier_serial();
    if (!serial.empty()) {
        d_ptr->serial = serial;
    }
    d_ptr->chips = carrier_builder.get_chips();
    d_ptr->units = carrier_builder.get_units();
    d_ptr->storage = carrier_builder.get_storage();
    d_ptr->name = carrier_parser.get_name();
    d_ptr->log->debug("carrier created");
}
carrier_impl::~carrier_impl() noexcept
{
    d_ptr->log->debug("carrier destroyed");
    logger::drop_log(d_ptr->log);
}

void carrier_impl::reset()
{
    for (auto& subsystem : d_ptr->subsystems) {
        subsystem.second->reset();
    }
    d_ptr->log->debug("carrier reset");
}

const subsystem_storage& carrier_impl::subsystems() const noexcept
{
    return d_ptr->subsystems;
}

const unit_storage& carrier_impl::units() const noexcept
{
    return d_ptr->units;
}

const chip_storage& carrier_impl::chips() const noexcept
{
    return d_ptr->chips;
}

data_storage& carrier_impl::storage() const noexcept
{
    return d_ptr->storage;
}

::io carrier_impl::get_io() const noexcept
{
    return d_ptr->io;
}

std::string carrier_impl::get_name() const noexcept
{
    return d_ptr->name;
}

std::string carrier_impl::get_version() const noexcept
{
    return d_ptr->version;
}

std::string carrier_impl::get_serial() const noexcept
{
    return d_ptr->serial;
}

carrier carrier_creator::create(io_type type, std::size_t index)
{
    return std::make_shared<carrier_impl>(type, index);
}
