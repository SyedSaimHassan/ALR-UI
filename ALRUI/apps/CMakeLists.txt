add_executable(Main
    Main.cpp     
 
)

target_link_libraries(Main Qt5::Widgets Qt5::Core Qt5::Gui GUI Comms)