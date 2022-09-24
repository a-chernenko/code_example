#include "nebulaxi/resource_manager.hpp"

#include "io/io.hxx"
#include "logger.hxx"

using namespace insys::nebulaxi;

resource_manager_error::resource_manager_error(const std::string& message)
    : nebulaxi_error(message, "[resource_manager_error]: ")
{
}

struct resource_manager::private_data {
    logger::log_type log { logger::create_log("resource_manager") };
    const std::size_t max_mezzanine_index { 5 };
    std::size_t max_board_index { 64 };
    board_list boards_list {};
};

std::shared_ptr<resource_manager::private_data> resource_manager::d_ptr {
    std::make_shared<resource_manager::private_data>()
};

void resource_manager::set_max_boards(std::size_t max_boards) noexcept
{
    d_ptr->max_board_index = max_boards;
}

std::size_t resource_manager::get_max_boards() noexcept
{
    return d_ptr->max_board_index;
}

board_info_list resource_manager::get_boards_info()
{
    board_info_list info_list {};
    const io_type io_type_array[] { io_type::pcie, io_type::usb, io_type::zynq };
    for (auto io_type : io_type_array) {
        for (std::size_t index {}; index < d_ptr->max_board_index; ++index)
            try {
                auto io = io_impl::create(io_type, index);
                board_info info {};
                info.index = io->get_index();
                info.info = io->get_board_info();
                info.location = io->get_location();
                info.type = io->get_io_type();
                info.path = io->get_path();
                info.simulate = io->is_simulate();
                info_list.push_back(info);
            } catch (const io_error& e) {
                // TODO: добавить в лог
                break;
            }
    }
    return info_list;
}

void resource_manager::find_boards()
{
    d_ptr->log = logger::get_log(d_ptr->log->name());
    if (!d_ptr->boards_list.empty()) {
        d_ptr->boards_list.clear();
    }
    const io_type io_type_array[] { io_type::pcie, io_type::usb, io_type::zynq };
    for (auto io_type : io_type_array) {
        for (std::size_t carrier_index {}; carrier_index < d_ptr->max_board_index; ++carrier_index)
            try {
                board board {};
                board.carrier = carrier_creator::create(io_type, carrier_index);
                for (int mezzanine_index {}; mezzanine_index < d_ptr->max_mezzanine_index; ++mezzanine_index)
                    try {
                        auto mezzanine = mezzanine_creator::create(board.carrier, mezzanine_index);
                        board.mezzanines_list.push_back(std::move(mezzanine));
                    } catch (const mezzanine_error& e) {
                        // TODO: добавить в лог
                        break;
                    }
                d_ptr->log->info("Find board name: {}, serial: {}", board.carrier->get_name(), board.carrier->get_serial());
                d_ptr->boards_list.push_back(std::move(board));
            } catch (const io_error& e) {
                // TODO: добавить в лог
                break;
            }
    }
}

board_list resource_manager::get_boards()
{
    if (d_ptr->boards_list.empty()) {
        find_boards();
    }
    return d_ptr->boards_list;
}

board resource_manager::find_by_carrier_serial(const std::string& serial)
{
    auto& boards_list = d_ptr->boards_list;
    if (boards_list.empty()) {
        throw resource_manager_error("empty board list");
    }
    auto it = std::find_if(boards_list.cbegin(), boards_list.cend(),
        [&serial](const board& board) { return board.carrier->get_serial() == serial; });
    if (it == boards_list.cend()) {
        throw resource_manager_error("carrier not found [serial: " + serial + "]");
    }
    return *it;
}

board resource_manager::find_by_carrier_location(const io_locaction& location)
{
    auto& boards_list = d_ptr->boards_list;
    if (boards_list.empty()) {
        throw resource_manager_error("empty board list");
    }
    auto it = std::find_if(boards_list.cbegin(), boards_list.cend(),
        [&location](const board& board) {
            auto real_location = board.carrier->get_io()->get_location();
            return (real_location.bus == location.bus && real_location.slot == location.slot) ? true : false;
        });
    if (it == boards_list.cend()) {
        auto location_string = std::to_string(location.bus) + '.' + std::to_string(location.slot);
        throw resource_manager_error("carrier not found [bus.slot: " + location_string + "]");
    }
    return *it;
}
