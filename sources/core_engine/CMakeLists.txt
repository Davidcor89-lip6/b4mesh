add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ressources)
# Expose b4mesh_core_engine__generate_ressources, that generates some header-files
# Aliased as b4mesh::core_engine::generate_ressources

add_library(${PROJECT_NAME}_coreEngine SHARED
# todo : INTERFACE?, hierarchy -> check if all .cpp files are useless
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/application_packet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/b4mesh_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/blockgraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/consensus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/node.cpp # todo : delete this file !
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sources/transaction.cpp
)
set_target_properties(${PROJECT_NAME}_coreEngine
    PROPERTIES
        CXX_STANDARD 17 # 20
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)
target_compile_features(${PROJECT_NAME}_coreEngine PRIVATE
    cxx_std_17 # cxx_std_20
)
target_include_directories(${PROJECT_NAME}_coreEngine
# todo : restrict headers exposure
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/includes/impl
        ${CMAKE_CURRENT_SOURCE_DIR}/includes/interface
)

# Dependencies
## Boost
find_package(Boost 1.65.0 REQUIRED
    COMPONENTS 
        random
        system
)
## nlohmann_json::nlohmann_json)
include(${CMAKE_SOURCE_DIR}/cmake/modules/dependencies/fetch_nlohmann_json.cmake)
fetch_nlohmann_json()
if (NOT TARGET DBusCXX_asio_integration)
    include(${CMAKE_SOURCE_DIR}/cmake/modules/dependencies/libdbus-cpp-asio-integration.cmake)
endif()

add_dependencies(${PROJECT_NAME}_coreEngine
    Boost::random
    Boost::system
    nlohmann_json::nlohmann_json
    DBusCXX
    #DBusCXX_asio_integration
    ${PROJECT_NAME}::utils
    ${PROJECT_NAME}::core_engine::generated
)
target_link_libraries(${PROJECT_NAME}_coreEngine
    PRIVATE
        Boost::random
        Boost::system
        nlohmann_json::nlohmann_json
        ${PROJECT_NAME}::utils
        ${PROJECT_NAME}::core_engine::generated
    PUBLIC
    # because of useless .cpp files, we need to push extra stuffs into PUBLIC exposure
        DBusCXX
        #DBusCXX_asio_integration
        ${PROJECT_NAME}::core_engine::generated
)
add_library(${PROJECT_NAME}::core_engine ALIAS ${PROJECT_NAME}_coreEngine)

