project(PrimeWatch)
add_executable(${PROJECT_NAME} "")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE QML *.qml)

target_sources(${PROJECT_NAME} PRIVATE
	main.cpp
	client.cpp
	client.h
	debughelper.inc
	${QML}
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED 14)

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Concurrent Core Multimedia Network Qml Quick Svg)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent Qt5::Core Qt5::Multimedia Qt5::Network Qt5::Qml Qt5::Quick Qt5::Svg)

# Copy the QML to the build folder
foreach(filepath_full ${QML})
	# Strip source path to get relative path (in)
	string(REPLACE ${CMAKE_SOURCE_DIR}/ "" filepath_relative_in ${filepath_full})
	configure_file(${filepath_relative_in} ${filepath_relative_in} COPYONLY)
endforeach()

# Endianness is the root of all evil
if(WIN32)
  target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
