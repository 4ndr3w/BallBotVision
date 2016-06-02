cmake_minimum_required(VERSION 3.5)
project(BallFinder)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp KinectCV.cpp RobotReceiver.cpp RobotSender.cpp EndianHelper.cpp)
add_executable(BallFinder ${SOURCE_FILES})
add_executable(RecvTest recvtest.cpp RobotReceiver.cpp RobotSender.cpp EndianHelper.cpp)
target_link_libraries(BallFinder opencv_core opencv_highgui opencv_imgproc freenect freenect_sync)
