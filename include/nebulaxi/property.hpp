#pragma once

namespace insys {

template <typename class_type, typename value_type,
    value_type (class_type::*getter)(),
    void (class_type::*setter)(const value_type&)>
struct property final {
    class_type& instance;

    property() = delete;
    property(class_type& instance)
        : instance(instance)
    {
    }
    property& operator=(const property& other)
    {
        return *this = (other.instance.*getter)();
    }
    property(const property&) = default;
    property& operator=(property&&) noexcept = delete;
    property(property&&) noexcept = delete;
    ~property() noexcept = default;
    operator value_type() const
    {
        static_assert(getter != nullptr, "write only property");
        return (instance.*getter)();
    }
    property& operator=(const value_type& value)
    {
        static_assert(setter != nullptr, "read only property");
        (instance.*setter)(value);
        return *this;
    }
    template <typename class_type_another, typename value_type_another,
        value_type_another (class_type_another::*getter_another)(),
        void (class_type_another::*setter_another)(const value_type_another&)>
    property& operator=(const property<class_type_another, value_type_another,
        getter_another, setter_another>& other)
    {
        return *this = (other.instance.*getter_another)();
    }
};

}
