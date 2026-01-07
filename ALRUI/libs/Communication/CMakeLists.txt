file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(Comms ${SOURCES} ${HEADERS})
target_link_libraries(Comms PUBLIC ${GLOBAL_LIBS})
target_include_directories(Comms PUBLIC include)



# add_subdirectory(test)