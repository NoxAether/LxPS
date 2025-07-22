#ifndef SERIALMONITOR_H_
#define SERIALMONITOR_H_

#include "SerialPort.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <libudev.h>
#include <set>

class SerialMonitor {
  public:
    SerialMonitor(boost::asio::io_context &io_context,
                  SerialPort &serial_helper, MainMenu &menu_ref);

    ~SerialMonitor();

    auto scanInitialPorts() -> void;

  private:
    boost::asio::io_context &io_context_;
    SerialPort &serial_helper_;
    udev *udev_;
    udev_monitor *udev_monitor_;
    boost::asio::posix::stream_descriptor udev_descriptor_;
    std::set<std::string> known_ports_;
    char dummy_byte_[1];
    MainMenu &menu_ref_;

    auto startReceiveUdevEvents() -> void;

    auto handleUdevEvent() -> void;
};

#endif
