cmake_minimum_required(VERSION 3.12)

# Project : B4mesh

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake) # modules, dependencies

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Default build type")
endif()

project(b4mesh
    VERSION 0.1.2
    LANGUAGES CXX
    DESCRIPTION "B4Mesh main projet"
    #HOMEPAGE_URL https://github.com/Davidcor89-lip6/b4mesh # WTF mix-up with CMAKE_C_COMPILER on linux ???
)
enable_testing()

## Documentation
### Dependencies diagram generation

find_program(DOT_EXE NAME dot)
if (NOT ${DOT_EXE})
    set(DependenciesDiagramNameNoExt ${CMAKE_CURRENT_SOURCE_DIR}/doc/build/dependencies_diagram)
    add_custom_target(${PROJECT_NAME}_generates_cmake_dependencies_diagram ALL
        COMMAND ${CMAKE_COMMAND} "--graphviz=${DependenciesDiagramNameNoExt}.dot" .
        COMMAND dot -Tpng ${DependenciesDiagramNameNoExt}.dot -o ${DependenciesDiagramNameNoExt}.png
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
else()
    message(STATUS "[${PROJECT_NAME}] : Info : program [dot] not found. Install dot to enable [${PROJECT_NAME}_generates_cmake_dependencies_diagram] target")
endif()

## Build

message(STATUS "[${PROJECT_NAME}] : toolchain is [${CMAKE_TOOLCHAIN_FILE}]")
message(STATUS "[${PROJECT_NAME}] : CMAKE_BUILD_TYPE is [${CMAKE_BUILD_TYPE}]")

include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain/lazy_detection.cmake)
GreenSDK_Toolchain_lazyDetection()
if (${USING_GREEN_SDK})
    include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain/import.cmake)
else ()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/toolchain)
    return() # only target allowed is toolchain generation
endif()

### Build : dependencies

include(dependencies/libdbus-cpp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/b4mesh_http)

set(CMAKE_CXX_STANDARD 17) # 20
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(Boost_USE_DEBUG_LIBS OFF)
# set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sources)

## Tests

# todo : option(b4mesh_BUILD_TESTS) => test() + gtest dependencies

## Deployement

# todo : package() ... ?

### Binary

# todo ...
# - install() ... include(GNUInstallDirs)

### Live visualizer

set(remote_machines_IP "NOT_DEFINED" CACHE PATH "deployement : list of ;-separated remotes IP")
if (remote_machines_IP MATCHES "NOT_DEFINED")
    message(FATAL_ERROR "`remote_machines_IP` is a mandatory parameter")
endif()
message(STATUS "${PROJECT_NAME} : Remote machines IPs set to :")
foreach(IP IN LISTS remote_machines_IP)
    message(STATUS "- ${IP}")
endforeach()

set(remote_install_dir "NOT_DEFINED" CACHE PATH "deployement : remote install dir")
if (remote_install_dir MATCHES "NOT_DEFINED")
    message(FATAL_ERROR "`remote_install_dir` is a mandatory parameter")
endif()
include(${CMAKE_SOURCE_DIR}/cmake_modules/utility.cmake)
unquote_string(${remote_install_dir} remote_install_dir)

message(STATUS "${PROJECT_NAME} : remote_install_dir set to : ${remote_install_dir}")

include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)

## remote FS initialization
# install(CODE "MESSAGE(\"Initializing remote directories...\")")
install(CODE "include(${CMAKE_SOURCE_DIR}/cmake_modules/remote_deploy.cmake)")
# install(CODE "initialize_remote_FS(
#                 DESTINATIONS \"${remote_machines_IP}\"
#                 PATH
#                     ${remote_install_dir}
#                 SSHPASS_USER    b4meshroot
#                 SSH_USER        default
#                 ROOT_PASSWORD   b4meshroot
#             )"
# )

## nginx
# - /add_transaction
install(CODE "MESSAGE(\"Deploying b4mesh (nginx endpoints)...\")")
install(CODE "send_to_remote_machines(
                DESTINATIONS \"${remote_machines_IP}\"
                PATH
                    /tmp
                FILES
                    ${CMAKE_SOURCE_DIR}/nginx/add_transaction.conf
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )"
)
foreach(IP IN LISTS remote_machines_IP)
    install(CODE "remote_root_move_file(
        DESTINATION_IP      ${IP} 
        SOURCE_PATH         /tmp/add_transaction.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )")
endforeach()

add_subdirectory(live_visualizer)

