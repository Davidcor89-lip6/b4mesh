# Install script for directory: /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  MESSAGE("Deploying b4mesh::live_visualizer (nginx endpoints)...")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  include(/home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/../cmake_modules/remote_deploy.cmake)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  send_to_remote_machines(
                DESTINATIONS "10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210"
                PATH
                    /tmp
                FILES
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/nginx/http_live_visualizer.conf
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_root_move_file(
        DESTINATION_IP      10.181.178.217 
        SOURCE_PATH         /tmp/http_live_visualizer.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_root_move_file(
        DESTINATION_IP      10.181.172.130 
        SOURCE_PATH         /tmp/http_live_visualizer.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_root_move_file(
        DESTINATION_IP      10.154.134.26 
        SOURCE_PATH         /tmp/http_live_visualizer.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_root_move_file(
        DESTINATION_IP      10.154.134.170 
        SOURCE_PATH         /tmp/http_live_visualizer.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_root_move_file(
        DESTINATION_IP      10.181.178.210 
        SOURCE_PATH         /tmp/http_live_visualizer.conf
        DESTINATION_PATH    /etc/nginx/qolyester.d/
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  MESSAGE("Deploying b4mesh::live_visualizer (sources)...")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  include(/home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/../cmake_modules/remote_deploy.cmake)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  send_to_remote_machines(
                DESTINATIONS "10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210"
                PATH
                    /var/persistent-data/b4mesh/live_visualizer
                FILES
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/index.html
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/style.css
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/favicon.ico
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/configuration.js
                DIRECTORIES
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/scripts
                    /home/alexandre/b4mesh-demonstration_07_06_2021/live_visualizer/img_web
                SSHPASS_USER    b4meshroot
                SSH_USER        default
            )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  MESSAGE("Fixing remote directories ACLs for NGinx ...")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  initialize_remote_FS(
                DESTINATIONS "10.181.178.217;10.181.172.130;10.154.134.26;10.154.134.170;10.181.178.210"
                PATH
                    /var/persistent-data/b4mesh
                SSHPASS_USER    b4meshroot
                SSH_USER        default
                ROOT_PASSWORD   b4meshroot
            )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_restart_nginx(
        DESTINATION_IP      10.181.178.217 
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_restart_nginx(
        DESTINATION_IP      10.181.172.130 
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_restart_nginx(
        DESTINATION_IP      10.154.134.26 
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_restart_nginx(
        DESTINATION_IP      10.154.134.170 
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  remote_restart_nginx(
        DESTINATION_IP      10.181.178.210 
        SSHPASS_USER        b4meshroot
        SSH_USER            default
        ROOT_PASSWORD       b4meshroot
    )
endif()

