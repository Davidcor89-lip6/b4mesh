if (USING_GREEN_SDK)
	if ((NOT DEFINED GREEN_SDK_IMPORTED) OR (NOT ${GREEN_SDK_IMPORTED}))
		include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain/import.cmake)
	endif()
endif()

find_program(dbusxx_xml2cpp_path
	NAMES dbusxx-xml2cpp
	REQUIRED
)
message(STATUS "[b4mesh] core_engine/ressources : detecting [dbusxx-xml2cpp] at [${dbusxx_xml2cpp_path}]")

set(generated_files_OUTPUTDIR ${CMAKE_CURRENT_BINARY_DIR}/generation_output)
set(generated_files_INCLUDEDIR ${generated_files_OUTPUTDIR}/includes)
set(generated_files_INCLUDEDIR_coreEngine ${generated_files_INCLUDEDIR}/b4mesh/core_engine/generated)

if (NOT (EXISTS ${generated_files_INCLUDEDIR_coreEngine}))
	make_directory(${generated_files_INCLUDEDIR_coreEngine})
endif()

message(STATUS "[b4mesh] core_engine/ressources : output is generated in [${generated_files_OUTPUTDIR}]")
add_custom_target(${PROJECT_NAME}_core_engine__generate_ressources
	COMMAND	${dbusxx_xml2cpp_path} ${CMAKE_CURRENT_SOURCE_DIR}/consensus.xml --proxy=${generated_files_INCLUDEDIR_coreEngine}/consensus.hpp
	COMMAND	${dbusxx_xml2cpp_path} ${CMAKE_CURRENT_SOURCE_DIR}/state.xml --proxy=${generated_files_INCLUDEDIR_coreEngine}/state.hpp
	BYPRODUCTS
		${generated_files_INCLUDEDIR_coreEngine}/consensus.hpp
		${generated_files_INCLUDEDIR_coreEngine}/state.hpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_library(${PROJECT_NAME}_core_engine__generated INTERFACE)
add_dependencies(
	${PROJECT_NAME}_core_engine__generated
	${PROJECT_NAME}_core_engine__generate_ressources
)
target_include_directories(${PROJECT_NAME}_core_engine__generated
	INTERFACE
		${generated_files_INCLUDEDIR}/
)

# $<TARGET_PROPERTY:${PROJECT_NAME}_core_engine__generated_ressources,INCLUDE_DIRECTORIES>
