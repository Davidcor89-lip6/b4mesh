add_library(${PROJECT_NAME}_utils INTERFACE)
set_property(TARGET ${PROJECT_NAME}_utils
    PROPERTY
        CXX_STANDARD 17 # 20
        CXX_EXTENSIONS OFF
)
target_compile_features(${PROJECT_NAME}_utils
    cxx_std_17 # cxx_std_20
)
target_include_directories(${PROJECT_NAME}_utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/
)

# Dependencies
## Boost
# set(Boost_USE_DEBUG_LIBS OFF)
# set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.65.0 REQUIRED COMPONENTS asio) 

add_dependencies(${PROJECT_NAME}_utils INTERFACE
    Boost::asio
)
target_link_libraries(${PROJECT_NAME}_utils INTERFACE
    Boost::asio
)
add_library(${PROJECT_NAME}::utils ALIAS ${PROJECT_NAME}_utils)

