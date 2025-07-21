cmake_minimum_required_version(VERSION 3.6)
fltk_wrap_ui(Ui CFluidWindow.fl)
add_library(Ui CFluidWindow.cxx ${Ui_FLTK_UI_SRCS})

add_executable(foo display_image.cpp)
add_dependencies(foo Ui)
target_link_libraries(foo ${FLTK_LIBRARIES})
target_link_libraries(foo ${OPENGL_LIBARIES})
target_link_libraries()
