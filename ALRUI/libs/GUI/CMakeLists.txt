file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(GUI ${SOURCES} ${HEADERS})
target_link_libraries(GUI PUBLIC ${GLOBAL_LIBS})
target_include_directories(GUI PUBLIC include)



# add_subdirectory(test)