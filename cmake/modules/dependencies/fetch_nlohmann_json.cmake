include(FetchContent)

function(fetch_nlohmann_json)

	message(STATUS "[nlohmann::json] Fetching dependency ...")

	set(JSON_BuildTests OFF CACHE INTERNAL "")

	FetchContent_Declare(json
	  GIT_REPOSITORY https://github.com/nlohmann/json.git
	  GIT_TAG v3.7.3)

	FetchContent_GetProperties(json)
	if(NOT json_POPULATED)
	  FetchContent_Populate(json)
	  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
	endif()
endfunction()