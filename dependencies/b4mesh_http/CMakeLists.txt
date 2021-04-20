cmake_minimum_required (VERSION 3.8)

project ("b4mesh_http")

# required for GreenIT-SDK integration
# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(FetchContent)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system random)
#find_package(Boost REQUIRED COMPONENTS system date_time thread chrono regex random)
add_library(boost_beast INTERFACE)
target_include_directories(boost_beast INTERFACE
#	installation non-cloisonnee
#   donc oblige d'integrer tout GREEN_SDK_INCLUDEDIR
#   pour maintenir les pathes : boost/beast/<filename>.hpp
	#${GREEN_SDK_INCLUDEDIR}
	${Boost_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/includes/b4mesh_http
)
target_link_libraries(${PROJECT_NAME} INTERFACE
	Threads::Threads
	Boost::system # Boost::asio requirement
	boost_beast
)
add_library(b4mesh::http ALIAS ${PROJECT_NAME})

set(b4mesh_http_BuildTests OFF CACHE INTERNAL "")
message(STATUS "[b4mesh::http] Build tests is [${b4mesh_http_BuildTests}]")

if (${b4mesh_http_BuildTests})
	enable_testing()
	add_subdirectory(tests)
endif()