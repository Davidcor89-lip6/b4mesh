add_executable(${PROJECT_NAME}_binary
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp
)
set_property(TARGET ${PROJECT_NAME}_binary
    PROPERTY
        CXX_STANDARD 17 # 20
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)
target_compile_features(${PROJECT_NAME}_binary
    cxx_std_17 # cxx_std_20
)

# Dependencies
## Boost
# set(Boost_USE_DEBUG_LIBS OFF)
# set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.65.0 REQUIRED
    COMPONENTS
        asio
        # program_options ! instead of getopts
) 
## Threads
set(CMAKE_THREAD_PREFER_PTHREAD true)
set(THREADS_PREFER_PTHREAD_FLAG true)
find_package(Threads REQUIRED)

add_dependencies(${PROJECT_NAME}_binary
    ${PROJECT_NAME}::core_engine
    Boost::asio
    Threads::Threads
)
target_link_libraries(${PROJECT_NAME}_binary
    PRIVATE
        ${PROJECT_NAME}::core_engine
        Boost::asio
        Threads::Threads
)
add_executable(${PROJECT_NAME}::binary ALIAS ${PROJECT_NAME}_binary)

# ressources
#   consensus.xml
#   state.xml
