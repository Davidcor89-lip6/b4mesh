
if (NOT ${BUILD_TESTING})
    return()
endif()

if (NOT TARGET ${PROJECT_NAME}::binary)
    message(FATAL_ERROR "[${PROJECT_NAME}/tests] All tests depends on [${PROJECT_NAME}::binary] output")
endif()

find_program(sshpass_EXE NAME sshpass REQUIRED)

add_test(
	NAME ${PROJECT_NAME}_test__1
	COMMAND ${sshpass_EXE} -p 'b4meshroot' scp $<TARGET_FILE:${PROJECT_NAME}_binary> default@10.181.178.217:/var/persistent-data/b4mesh # execute test
	COMMAND ${sshpass_EXE} -p 'b4meshroot' scp default@10.181.178.217:/var/persistent-data/b4mesh/blockgraph* Results # recover results
)

add_test(
	NAME ${PROJECT_NAME}_test__all
	# execute tests
	COMMAND ${sshpass_EXE} -p 'b4meshroot' scp $<TARGET_FILE:${PROJECT_NAME}_binary> default@10.181.178.217:/var/persistent-data/b4mesh
	COMMAND ${sshpass_EXE} -p 'b4meshroot' scp $<TARGET_FILE:${PROJECT_NAME}_binary> default@10.181.172.130:/var/persistent-data/b4mesh
	COMMAND ${sshpass_EXE} -p 'b4meshroot' scp $<TARGET_FILE:${PROJECT_NAME}_binary> default@10.154.134.26:/var/persistent-data/b4mesh
	# recover results
	COMMAND  ${sshpass_EXE} -p 'b4meshroot' scp default@10.181.178.217:/var/persistent-data/b4mesh/blockgraph* Results
	COMMAND  ${sshpass_EXE} -p 'b4meshroot' scp default@10.181.172.130:/var/persistent-data/b4mesh/blockgraph* Results
	COMMAND  ${sshpass_EXE} -p 'b4meshroot' scp default@10.154.134.26:/var/persistent-data/b4mesh/blockgraph* Results
	COMMAND  ${sshpass_EXE} -p 'b4meshroot' scp default@10.181.178.217:/var/persistent-data/b4mesh/mempool* Results
	COMMAND  ${sshpass_EXE} -p 'b4meshroot' scp default@10.181.172.130:/var/persistent-data/b4mesh/mempool* Results
	COMMAND  ${sshpass_EXE} -p 'b4meshroot' scp default@10.154.134.26:/var/persistent-data/b4mesh/mempool* Results
)
