#include "../headers/SerialPort.hpp"
#include <boost/asio/io_context.hpp> #include <string>
#include <vector>

SerialPort::SerialPort(MainMenu &menu_ref) : menu_ref_(menu_ref) {}

auto SerialPort::configureSerialPort(boost::asio::serial_port &serial,
                                     const std::string &port_name,
                                     uint32_t baud_rate) -> int {

    if (!serial.is_open()) {
        menu_ref_.displayError(
            "Error: configureSerialPort called with a closed serial port for " +
            port_name);
        return -1;
    }

    boost::system::error_code ec;

    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate), ec);
    if (ec) {
        menu_ref_.displayError("Setting baud rate to " +
                               std::to_string(baud_rate) + " for " + port_name +
                               ": Fail. Error: " + ec.message());
        return -1;
    }
    menu_ref_.displayMessage("Setting baud rate to " +
                             std::to_string(baud_rate) + " for " + port_name +
                             ": Success");

    serial.set_option(boost::asio::serial_port_base::character_size(8), ec);
    if (ec) {
        menu_ref_.displayError("Error setting character_size for " + port_name +
                               ": " + ec.message());
        return -1;
    }
    serial.set_option(boost::asio::serial_port_base::flow_control(
                          boost::asio::serial_port_base::flow_control::none),
                      ec);
    if (ec) {
        menu_ref_.displayError("Error setting flow_control for " + port_name +
                               ": " + ec.message());
        return -1;
    }
    serial.set_option(boost::asio::serial_port_base::parity(
                          boost::asio::serial_port_base::parity::none),
                      ec);
    if (ec) {
        menu_ref_.displayError("Error setting parity for " + port_name + ": " +
                               ec.message());
        return -1;
    }
    serial.set_option(boost::asio::serial_port_base::stop_bits(
                          boost::asio::serial_port_base::stop_bits::one),
                      ec);
    if (ec) {
        menu_ref_.displayError("Error setting stop_bits for " + port_name +
                               ": " + ec.message());
        return -1;
    }
    menu_ref_.displayMessage("Other serial options (8N1, no flow) set for " +
                             port_name + ": Success");

    return 0;
}

auto SerialPort::readFromSerialPort(boost::asio::serial_port &serial)
    -> std::optional<std::string> {

    std::vector<char> buffer(100);
    boost::system::error_code ec;

    size_t len = boost::asio::read(serial, boost::asio::buffer(buffer), ec);

    if (ec) {
        menu_ref_.displayError("Read serial port: Fail. Error: " +
                               ec.message());
        return std::nullopt;
    } else {
        menu_ref_.displayMessage("Read serial port: Success. Read " +
                                 std::to_string(len) + " bytes.");
        return std::string(buffer.data(), len);
    }
}

auto SerialPort::writeToSerialPort(boost::asio::serial_port &serial,
                                   const std::string &message) -> int {
    boost::system::error_code ec;

    boost::asio::write(serial, boost::asio::buffer(message), ec);

    if (ec) {
        menu_ref_.displayError("Writing to serial: Fail. Error: " +
                               ec.message());
        return -1;
    }

    menu_ref_.displayMessage("Writing to serial: Success. Wrote " +
                             std::to_string(message.length()) + " bytes.");
    return 0;
}
