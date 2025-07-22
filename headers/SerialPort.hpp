// SerialPort.hpp
#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include "MainMenu.hpp"
#include <boost/asio.hpp>
#include <cstdint>
#include <optional>

class SerialPort {
  public:
    SerialPort(MainMenu &menu_ref);

    auto configureSerialPort(boost::asio::serial_port &serial,
                             const std::string &port_name, uint32_t baud_rate)
        -> int;

    auto readFromSerialPort(boost::asio::serial_port &serial)
        -> std::optional<std::string>;

    auto writeToSerialPort(boost::asio::serial_port &serial,
                           const std::string &message) -> int;

  private:
    MainMenu &menu_ref_;
};

#endif
