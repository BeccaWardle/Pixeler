cmake_minimum_required(VERSION 3.7..3.22)
project( pixeler VERSION 0.1
    DESCRIPTION "Take input image and pixelate with different techniques"
    LANGUAGES CXX)

#find_package(OpenCV PATHS /usr/local/lib/cmake NO_DEFAULT_PATH)
find_package( OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(pixeler main.cpp include/image.h src/image.cpp)

target_include_directories( pixeler PUBLIC include )
#add_executable( MassBody src/MassBody.cpp )
#target_link_libraries( MassBody ${OpenCV_LIBS})


target_link_libraries( pixeler ${OpenCV_LIBS})
