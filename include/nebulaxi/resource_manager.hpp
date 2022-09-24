#pragma once

#include <memory>
#include <string>
#include <vector>

#include "nebulaxi/io/io.hpp"
#include "nebulaxi/mezzanine.hpp"

namespace insys::nebulaxi {

struct board_info {
    io_type type;
    io_locaction location;
    io_info info;
    std::size_t index;
    std::string path;
    bool simulate;
};

struct board {
    insys::nebulaxi::carrier carrier;
    std::vector<mezzanine> mezzanines_list;
};

using board_info_list = std::vector<board_info>;
using board_list = std::vector<board>;

class resource_manager final {
    struct private_data;
    static std::shared_ptr<private_data> d_ptr;

public:
    static void set_max_boards(std::size_t) noexcept;
    static std::size_t get_max_boards() noexcept;

    static board_info_list get_boards_info();
    static void find_boards();
    static board_list get_boards();

    static board find_by_carrier_serial(const std::string&);
    static board find_by_carrier_location(const io_locaction&);
};

class resource_manager_error : public nebulaxi_error {

public:
    using nebulaxi_error::nebulaxi_error;
    resource_manager_error(const std::string&);
    virtual ~resource_manager_error() noexcept = default;
};

}
