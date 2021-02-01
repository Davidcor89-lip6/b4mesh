if (USING_GREEN_SDK)
	if (NOT DEFINED CMAKE_PROGRAM_PATH)
		message(FATAL_ERROR "[b4mesh] core_engine/ressources : GreenSDK toolchain detected, but toolchain does not set [CMAKE_PROGRAM_PATH]")
	endif()

	set(dbusxx_xml2cpp_path ${CMAKE_PROGRAM_PATH}/dbusxx_xml2cpp)
	if (NOT (EXISTS ${dbusxx_xml2cpp_path}))
		message(FATAL_ERROR "[b4mesh] core_engine/ressources : GreenSDK toolchain detected, but [${dbusxx_xml2cpp_path}] does not exists")
	endif()
else()
	find_program(dbusxx_xml2cpp_path
		NAMES dbusxx-xml2cpp
		REQUIRED
	)
endif()

set(generated_files_output_dir {CMAKE_INSTALL_PREFIX}/generated/)
if (NOT (EXISTS ${generated_files_output_dir}))
	make_directory(${generated_files_output_dir})
endif()

message(STATUS "[b4mesh] core_engine/ressources : output is generated in [${generated_files_output_dir}]")
add_custom_target(${PROJECT_NAME}_core_engine__generate_ressources
	COMMAND	${dbusxx_xml2cpp_path} ${CMAKE_CURRENT_SOURCE_DIR}/consensus.xml --proxy=${generated_files_output_dir}/consensus.h
	COMMAND	${dbusxx_xml2cpp_path} ${CMAKE_CURRENT_SOURCE_DIR}/state.xml --proxy=${generated_files_output_dir}/state.h
	BYPRODUCTS
		${generated_files_output_dir}/consensus.h
		${generated_files_output_dir}/state.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set(${PROJECT_NAME}_core_engine__generate_ressources_INCLUDES_DIR ${generated_files_output_dir} PARENT_SCOPE)
