if (NOT DEFINED FileAppend_DESTINATION)
	message(FATAL_ERROR "[FileAppend.cmake] : missing FileAppend_DESTINATION parameter")
endif()
if (NOT DEFINED FileAppend_DATA)
	message(FATAL_ERROR "[FileAppend.cmake] : missing FileAppend_DATA parameter")
endif()

message(STATUS "[FileAppend.cmake] Appenning [${FileAppend_DATA}] to [${FileAppend_DESTINATION}]")

file(APPEND ${FileAppend_DESTINATION} ${FileAppend_DATA})