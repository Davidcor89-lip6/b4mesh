add_library(${PROJECT_NAME}_utils INTERFACE)
# set_property(TARGET ${PROJECT_NAME}_utils
#     PROPERTY
#         #CXX_STANDARD 17 # 20
#         #CXX_EXTENSIONS OFF
#         #POSITION_INDEPENDENT_CODE ON
# )
target_compile_features(${PROJECT_NAME}_utils INTERFACE
    cxx_std_17 # cxx_std_20
)
target_include_directories(${PROJECT_NAME}_utils INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/
)

# Dependencies
## Boost
find_package(Boost 1.65.0 REQUIRED
    COMPONENTS
        system
)
add_dependencies(${PROJECT_NAME}_utils INTERFACE
    Boost::system
)
target_link_libraries(${PROJECT_NAME}_utils INTERFACE
    Boost::system
)
add_library(${PROJECT_NAME}::utils ALIAS ${PROJECT_NAME}_utils)

