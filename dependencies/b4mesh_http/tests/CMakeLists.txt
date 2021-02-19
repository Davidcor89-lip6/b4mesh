# Test, benchmark, example binary
#
# Expose the following endpoints :
#	/benchmark			which has an empty behavior
#	/error				which return a server error
#	/add_transaction	which emulates a request with payload

add_executable(${PROJECT_NAME}_example_binary
	${CMAKE_CURRENT_SOURCE_DIR}/sources/source.cpp
)
set_target_properties(${PROJECT_NAME}_example_binary
	PROPERTIES
		CXX_STANDARD 17
)
add_dependencies(${PROJECT_NAME}_example_binary ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_example_binary
	PRIVATE
		${PROJECT_NAME}
)

# Tests
find_program(AB_PATH ab REQUIRED) # apache-benchmark
add_test(
	NAME test_$<TARGET_NAME:${PROJECT_NAME}_example_binary>_error
	COMMAND $<TARGET_FILE:${PROJECT_NAME}_example_binary> 10 &
	(
		sleep 2; curl --request GET  0.0.0.0:4242/error
	)
)