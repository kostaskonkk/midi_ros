cmake_minimum_required(VERSION 2.8.3)
project(midi_ros)

## Compile as C++11, supported in ROS Kinetic and newer

add_compile_options(-std=c++11)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__LINUX_ALSA__ -lasound -lpthread")


find_package(ALSA)
if (NOT ALSA_FOUND)
	message(FATAL_ERROR "ALSA API requested but no ALSA dev libraries found")
endif()

# pthread
find_package(Threads REQUIRED
  CMAKE_THREAD_PREFER_PTHREAD
  THREADS_PREFER_PTHREAD_FLAG)
  list(APPEND LINKLIBS Threads::Threads)
find_package(catkin REQUIRED COMPONENTS
  message_generation
  roscpp
)

# Generate messages in the 'msg' folder
add_message_files(
  FILES
  midi.msg
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs  # Or other packages containing msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES midi_ros
#  CATKIN_DEPENDS message_generation roscpp
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)
add_executable(midi_input src/input.cpp src/RtMidi.cpp)

target_link_libraries(midi_input ${catkin_LIBRARIES})
target_link_libraries(midi_input ${LINKLIBS})
target_link_libraries(midi_input ${ALSA_LIBRARY})
