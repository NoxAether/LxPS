// SerialMonitor.cpp
#include "../headers/SerialMonitor.hpp"

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/serial_port.hpp>
#include <filesystem>
#include <libudev.h>

extern void display_message_fmt(const std::string &format_str, ...);
extern void display_error_fmt(const std::string &format_str, ...);

// UTILITY FUNCTION
auto tryOpenPort(boost::asio::io_context &io, const std::string &port_name)
    -> bool {
    boost::asio::serial_port serial(io);
    boost::system::error_code ec;
    serial.open(port_name, ec);

    if (!ec) {
        display_message_fmt("tryOpenPort: Successfully opened and closed {0}",
                            port_name.c_str());
        serial.close(ec);
        return true;
    } else {
        display_error_fmt("tryOpenPort: Failed to open {0}: {1}",
                          port_name.c_str(), ec.message().c_str());
        return false;
    }
}

SerialMonitor::SerialMonitor(boost::asio::io_context &io_context,
                             SerialPort &serial_helper,
                             MainMenu &menu_ref) // <-- Add this parameter
    : io_context_(io_context), serial_helper_(serial_helper),
      menu_ref_(menu_ref), // <-- Initialize the new member
      udev_(udev_new()), udev_monitor_(nullptr), udev_descriptor_(io_context) {

    if (!udev_) {
        display_error_fmt("Failed to create udev context.");
        return;
    }

    udev_monitor_ = udev_monitor_new_from_netlink(udev_, "udev");

    if (!udev_monitor_) {
        display_error_fmt("Failed to create udev monitor.");
        udev_unref(udev_);
        udev_ = nullptr;
        return;
    }

    udev_monitor_filter_add_match_subsystem_devtype(udev_monitor_, "tty", NULL);
    udev_monitor_enable_receiving(udev_monitor_);

    int fd = udev_monitor_get_fd(udev_monitor_);

    if (fd < 0) {
        display_error_fmt("Failed to get udev monitor file descriptor.");
        udev_monitor_unref(udev_monitor_);
        udev_unref(udev_);
        udev_monitor_ = nullptr;
        udev_ = nullptr;
        return;
    }

    udev_descriptor_.assign(fd);
    startReceiveUdevEvents();

    display_message_fmt(
        "Udev monitor started, listening for new serial devices.");
}

SerialMonitor::~SerialMonitor() {
    if (udev_descriptor_.is_open()) {
        udev_descriptor_.cancel();
        udev_descriptor_.close();
    }

    if (udev_monitor_) {
        udev_monitor_unref(udev_monitor_);
    }
    if (udev_) {
        udev_unref(udev_);
    }
}

auto SerialMonitor::scanInitialPorts() -> void {
    std::vector<std::string> common_prefixes = {"/dev/ttyS", "/dev/ttyUSB",
                                                "/dev/ttyACM"};
    for (const auto &prefix : common_prefixes) {
        for (int i = 0; i < 32; i++) {
            std::string port_path = prefix + std::to_string(i);
            if (std::filesystem::exists(port_path)) {
                display_message_fmt("Initial scan: Found filesystem entry {0}",
                                    port_path.c_str());
                if (tryOpenPort(io_context_, port_path)) {
                    known_ports_.insert(port_path);
                    display_message_fmt("Ports found with initial scan: {0} "
                                        "(successfully opened)",
                                        port_path.c_str());
                } else {
                    display_message_fmt("Ports found with initial scan: {0} "
                                        "(failed to open for testing)",
                                        port_path.c_str());
                }
            }
        }
    }
}

// Private functions
auto SerialMonitor::startReceiveUdevEvents() -> void {
    static boost::asio::mutable_buffer dummy_buffer(dummy_byte_, 1);

    udev_descriptor_.async_read_some(
        dummy_buffer, [this](const boost::system::error_code &ec,
                             std::size_t /*Bytes transferred*/) {
            if (!ec) {
                // If there is an event, handle that event.
                handleUdevEvent();
                // Continue listening for new events
                startReceiveUdevEvents();
            }

            else if (ec !=
                     boost::asio::error::
                         operation_aborted) { // Handle errors more, ignore op
                                              // aborted (FD close)
                display_error_fmt("Error reading udev events: {0}",
                                  ec.message().c_str());
            }
        });
}

auto SerialMonitor::handleUdevEvent() -> void {
    struct udev_device *dev = udev_monitor_receive_device(udev_monitor_);
    if (dev) {
        const char *action = udev_device_get_action(dev);
        const char *devnode = udev_device_get_devnode(dev);
        const char *subsystem = udev_device_get_subsystem(dev);

        if (devnode && subsystem) {
            display_message_fmt(
                "UDEV raw event: Action={0}, Devnode={1}, Subsystem={2}",
                (action ? action : "NULL"), (devnode ? devnode : "NULL"),
                (subsystem ? subsystem : "NULL"));

            if (std::string(subsystem) == "tty") {
                display_message_fmt(
                    "EVENT (tty subsystem): Action={0}, Device={1}", action,
                    devnode);

                if (std::string(action) == "add") {
                    if (known_ports_.find(devnode) == known_ports_.end()) {
                        display_message_fmt(
                            "Attempting connection to new device: {0}",
                            devnode);

                        boost::asio::serial_port new_serial(io_context_);
                        boost::system::error_code open_ec;
                        new_serial.open(devnode, open_ec);

                        if (open_ec) {
                            display_error_fmt(
                                "Failed to open new device {0}: {1}", devnode,
                                open_ec.message().c_str());
                        } else {
                            display_message_fmt("Device {0} opened. Now "
                                                "attempting configuration.",
                                                devnode);

                            if (serial_helper_.configureSerialPort(
                                    new_serial, devnode, 9600)) {
                                display_error_fmt(
                                    "Failed to configure new device: {0}",
                                    devnode);
                                new_serial.close();
                            } else {
                                display_message_fmt("Successfully connected to "
                                                    "new device: {0}",
                                                    devnode);
                                known_ports_.insert(devnode);
                                new_serial.close();
                            }
                        }
                    } else {
                        display_message_fmt("Device {0} already known.",
                                            devnode);
                    }
                } else if (std::string(action) == "remove") {
                    display_message_fmt("Serial device removed: {0}", devnode);
                    known_ports_.erase(devnode);
                }
            }
        }
        udev_device_unref(dev);
    }
}
