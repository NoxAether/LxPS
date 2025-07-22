#ifndef MAINMENU_HPP_
#define MAINMENU_HPP_

#include <notcurses/notcurses.h>
#include <string>

struct notcurses;
struct ncplane;

class MainMenu {
public:
  MainMenu();

  ~MainMenu();

  void displayMessage(const std::string &msg, bool is_error = false);

  void displayError(const std::string &msg);

private:
  struct notcurses *nc_;
  struct ncplane *status_plane_;
  int status_line_;
};

#endif
