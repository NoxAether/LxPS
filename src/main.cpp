#include "../headers/MainMenu.hpp"
#include "../headers/SerialMonitor.hpp"
#include "../headers/SerialPort.hpp"
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <unistd.h>

auto main(void) -> int {
    MainMenu menu;

    if (geteuid() != 0) {
        menu.displayError("Warning: Application not running with root "
                          "privileges. Device access may be limited.");
        menu.displayError("Consider running with 'sudo ./lxPS'");
    }

    char repeatLoop = 'y';

    while (tolower(repeatLoop) == 'y') {
        boost::asio::io_context io_context;

        SerialPort serial_helper(menu);
        SerialMonitor monitor(io_context, serial_helper, menu);
        monitor.scanInitialPorts();
        menu.displayMessage(
            "Application started. Waiting for new serial connections...");
        menu.displayMessage(
            "Plug in a serial device to trigger a connection attempt.");

        io_context.run();
        menu.displayMessage("Application finished this cycle. Go again (Y/N)?");

        std::cin >> repeatLoop;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    menu.displayMessage("Goodbye...");
    return 0;
}
