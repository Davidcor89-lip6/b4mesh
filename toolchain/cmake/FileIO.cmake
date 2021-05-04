# This CMake Script is direct-callable, using `FileIO_DIRECT_CALL:BOOL` variable

function(FileIO)

	set(options)
	set(oneValueArgs PATH)
	set(multiValueArgs TO_ADD TO_REMOVE_PATTERNS)
	cmake_parse_arguments(FILEIO
		"${options}"
		"${oneValueArgs}"
		"${multiValueArgs}"
		${ARGN}
	)
	message(STATUS "[FileIO] Scanning [${FILEIO_PATH}] ...")
	file(STRINGS ${FILEIO_PATH} LINES)

	message(STATUS "[FileIO] REMOVING lines matching any of [${FILEIO_TO_REMOVE_PATTERNS}] ...")
	foreach(PATTERN IN LISTS FILEIO_TO_REMOVE_PATTERNS)
		message(STATUS "[FileIO] - [${PATTERN}]")
	endforeach()

	file(WRITE ${FILEIO_PATH} "")
	foreach(LINE IN LISTS LINES)
		set(keep_line true)
		foreach(PATTERN IN LISTS FILEIO_TO_REMOVE_PATTERNS)
			string(REGEX MATCH ${PATTERN} match_result "${LINE}")
			if (match_result)
				set(keep_line false)
				break()
			endif()
		endforeach()
		if (keep_line)
			file(APPEND ${FILEIO_PATH} "${LINE}\n")
		endif()
	endforeach()

	message(STATUS "[FileIO] ADDING [${FILEIO_TO_ADD}] lines ...")
	foreach (LINE IN LISTS FILEIO_TO_ADD)
		message(STATUS "[FileIO] + [${LINE}]")
		file(APPEND ${FILEIO_PATH} "${LINE}\n")
	endforeach()

endfunction()

if (DEFINED FileIO_DIRECT_CALL)

	if (NOT DEFINED FileIO_PATH)
		message(FATAL_ERROR "[FileIO.cmake] : Missing mandatory argument FileIO_PATH")
	endif()
	if (NOT DEFINED FileIO_TO_ADD)
		message(FATAL_ERROR "[FileIO.cmake] : Missing mandatory argument FileIO_TO_ADD")
	endif()
	if (NOT DEFINED FileIO_TO_REMOVE_PATTERNS)
		message(FATAL_ERROR "[FileIO.cmake] : Missing mandatory argument FileIO_TO_REMOVE_PATTERNS")
	endif()

	message(STATUS ">>>> FILEIO : [${FileIO_TO_ADD}]")
	message(STATUS ">>>> FILEIO : [${FileIO_TO_REMOVE_PATTERNS}]")

	FileIO(PATH ${FileIO_PATH}
		TO_ADD
			${FileIO_TO_ADD}
		TO_REMOVE_PATTERNS
			${FileIO_TO_REMOVE_PATTERNS}
	)

endif()