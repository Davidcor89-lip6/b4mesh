cmake_minimum_required(VERSION 3.12)

function (unquote_string input_string result)

    string(LENGTH ${input_string} input_string_length)

    if (${input_string_length} EQUAL 0)
        return()
    endif()

    string(SUBSTRING ${input_string} 0 1 first_char)
    if (${first_char} STREQUAL "\"")
        math(EXPR input_string_length "${input_string_length} - 1")
        string(SUBSTRING ${input_string} 1 ${input_string_length} input_string)
    endif()

    math(EXPR input_string_length "${input_string_length} - 1")
    string(SUBSTRING ${input_string} ${input_string_length} 1 last_char)
    if (${last_char} STREQUAL "\"")
        string(SUBSTRING ${input_string} 0 ${input_string_length} input_string)
    endif()
    set(${result} ${input_string} PARENT_SCOPE)
endfunction()