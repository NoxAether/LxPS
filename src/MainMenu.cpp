#include "../headers/MainMenu.hpp"
#include <iostream> // For fallback std::cerr/std::cout

// Constructor implementation
MainMenu::MainMenu() : nc_(nullptr), status_plane_(nullptr), status_line_(0) {
    nc_ = notcurses_init(nullptr, nullptr); // Initialize Notcurses
    if (nc_ == nullptr) {
        std::cerr << "Failed to initialize Notcurses. Falling back to standard "
                     "output."
                  << std::endl;
    } else {
        status_plane_ = notcurses_stdplane(nc_); // Get the default plane
        if (status_plane_) {
            // Set default colors for the status plane: green text on black
            // background Step 1: Create individual 32-bit channels
            uint32_t fg_channel = 0;
            uint32_t bg_channel = 0;

            // Step 2: Set RGB values for individual channels (using singular
            // ncchannel_set_rgb8)
            ncchannel_set_rgb8(&fg_channel, 0, 255, 0); // Foreground green
            ncchannel_set_rgb8(&bg_channel, 0, 0, 0);   // Background black

            // Step 3: Combine individual channels into a 64-bit channel pair
            uint64_t default_channels =
                ncchannels_combine(fg_channel, bg_channel);

            // Step 4: Apply the combined channel pair to the plane
            ncplane_set_base(status_plane_, "", 0, default_channels);

            // Determine the last line for status messages
            unsigned int rows, cols;
            ncplane_dim_yx(status_plane_, &rows, &cols);
            status_line_ = rows - 1; // Set status line to the last row

            displayMessage("Notcurses initialized. Ready.");
        } else {
            // Fallback if standard plane cannot be obtained
            std::cerr << "Failed to get standard Notcurses plane. Falling back "
                         "to standard output."
                      << std::endl;
            notcurses_stop(nc_); // Clean up Notcurses if initialization failed
            nc_ = nullptr;
        }
    }
}

// Destructor implementation
MainMenu::~MainMenu() {
    if (nc_) {
        notcurses_stop(nc_); // Shut down Notcurses
        nc_ = nullptr;       // Reset pointer
    }
}

// displayMessage method implementation
void MainMenu::displayMessage(const std::string &msg, bool is_error) {
    if (status_plane_) {
        ncplane_erase(status_plane_); // Clear the current status line

        // Step 1: Create individual 32-bit channels
        uint32_t fg_channel = 0;
        uint32_t bg_channel = 0;
        uint64_t combined_channels = 0;

        if (is_error) {
            // Step 2: Set RGB values for individual channels (using singular
            // ncchannel_set_rgb8)
            ncchannel_set_rgb8(&fg_channel, 255, 0, 0); // Red text for errors
            ncchannel_set_rgb8(&bg_channel, 0, 0, 0);   // Black background
        } else {
            // Step 2: Set RGB values for individual channels (using singular
            // ncchannel_set_rgb8)
            ncchannel_set_rgb8(&fg_channel, 0, 255,
                               0); // Green text for normal messages
            ncchannel_set_rgb8(&bg_channel, 0, 0, 0); // Black background
        }

        // Step 3: Combine individual channels into a 64-bit channel pair
        combined_channels = ncchannels_combine(fg_channel, bg_channel);

        // Step 4: Apply the combined channel pair to the plane
        ncplane_set_channels(status_plane_, combined_channels);

        // Put the string aligned to the left on the status line
        ncplane_putstr_aligned(status_plane_, status_line_, NCALIGN_LEFT,
                               msg.c_str());
        notcurses_render(nc_); // Render the changes to the terminal
    } else {
        // Fallback to standard output if Notcurses is not initialized
        if (is_error) {
            std::cerr << msg << std::endl;
        } else {
            std::cout << msg << std::endl;
        }
    }
}

// displayError method implementation
void MainMenu::displayError(const std::string &msg) {
    displayMessage(msg,
                   true); // Call the main display function with is_error = true
}
