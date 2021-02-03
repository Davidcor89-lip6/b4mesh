add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ressources) # Expose b4mesh_core_engine__generate_ressources, that generates some header-files

add_library(${PROJECT_NAME}_coreEngine SHARED
# todo : INTERFACE?, hierarchy
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/application_packet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/blockgraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/transaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/b4mesh_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/consensus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/session.cpp
)
add_dependencies(${PROJECT_NAME}_coreEngine ${PROJECT_NAME}::core_engine::generated)
set_target_properties(${PROJECT_NAME}_coreEngine
    PROPERTIES
        CXX_STANDARD 17 # 20
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)
target_compile_features(${PROJECT_NAME}_coreEngine PRIVATE
    cxx_std_17 # cxx_std_20
)
target_link_libraries(${PROJECT_NAME}_coreEngine
    PRIVATE
        ${PROJECT_NAME}::core_engine::generated
)
target_include_directories(${PROJECT_NAME}_coreEngine
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/includes/impl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/includes/interface
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
        random
        system
)

add_dependencies(${PROJECT_NAME}_coreEngine
    Boost::random
    Boost::system
    DBusCXX
    ${PROJECT_NAME}::utils
)
target_link_libraries(${PROJECT_NAME}_coreEngine
    PRIVATE
        #Boost::math
        Boost::random
        Boost::system
        DBusCXX
        ${PROJECT_NAME}::utils
)
add_library(${PROJECT_NAME}::core_engine ALIAS ${PROJECT_NAME}_coreEngine)

