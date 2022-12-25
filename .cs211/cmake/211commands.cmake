cmake_minimum_required(VERSION 3.3)

include(211helpers)

# ADD_PROGRAM – Adds a program target with the given name, built from
# the given source files. Sets the language to C++ 14 and enables
# undefined behavior sanitizer by default.
#
# ## Usage
#
# ```
# add_program(NAME [OPTION...] SRCFILE...)
# ```
#
# where `OPTION`s include:
#
#   - `ASAN`     – enable address sanitizer
#   - `NO_UBSAN` – disable undefined behavior sanitizer
#   - `CXX17`    – enable C++ 2017
#   - `CXX20`    – (experimental) enable C++ 2020
#
# ## Examples
#
# Build the program Pong from pong.cxx with address sanitizer enabled:
#
# ```
# add_program(Pong pong.cxx ASAN)
# ```
#
# Build the program Frogger from the three listed source files, linking
# against GE211 and using C++ 2017:
#
# ```
# add_program(Frogger
#     src/frogger_main.cxx
#     src/frogger_ui.cxx
#     src/frogger_model.cxx
#     CXX17)
# target_link_libraries(Frogger ge211)
# ```
function (add_program name)
    cmake_parse_arguments(pa "ASAN;NO_UBSAN;CXX17;CXX20;" "" "" ${ARGN})

    add_executable(${name} ${pa_UNPARSED_ARGUMENTS})
    target_supported_compile_options(${name} ${CS211_CXXFLAGS})

    if(pa_ASAN)
        target_compile_options(${name} PRIVATE "-fsanitize=address")
        target_link_options(${name} PRIVATE "-fsanitize=address")
    endif(pa_ASAN)

    if(NOT pa_NO_UBSAN)
        target_supported_options(${name} "-fsanitize=undefined")
    endif(NOT pa_NO_UBSAN)

    if(pa_CXX20)
        set_property(TARGET ${name} PROPERTY CXX_STANDARD       20)
    elseif(pa_CXX17)
        set_property(TARGET ${name} PROPERTY CXX_STANDARD       17)
    else()
        set_property(TARGET ${name} PROPERTY CXX_STANDARD       14)
    endif()

    set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED  On)
    set_property(TARGET ${name} PROPERTY CXX_EXTENSIONS         Off)
endfunction(add_program)

# ADD_TEST_PROGRAM – Adds a test program target with the given name,
# built from the given source files and linked against the Catch2 test
# framework. The test framework provides a `main()` function, so you
# must not.
#
# The this command has the same options as `add_program`.
#
# This command defines the preprocessor macro CS211_TESTING, which means
# your code can test whether it is being compiled for testing via
# `#ifdef`:
#
# ```cxx
# #ifdef CS211_TESTING
# // ... code that will be compiled only when testing ...
# #else
# // ... code that will be compiled only when NOT testing ...
# #endif
# ```
function(add_test_program name)
    add_program(${name} ${ARGN})
    target_compile_definitions(${name} PRIVATE CS211_TESTING)
    target_link_libraries(${name} catch)
    add_test(Test_${name} ${name})
endfunction(add_test_program)

# Compilation flags we turn on automatically if available.
set(CS211_CXXFLAGS
    -Wall
    -Wcast-align=strict
    -Wcast-qual
    -Wdangling-else
    -Wnull-dereference
    -Wtype-limits
    -Wwrite-strings
    -Werror=bool-compare
    -Werror=bool-operation
    -Werror=int-to-pointer-cast
    -Werror=return-type
    -Werror=uninitialized)

# vim: ft=cmake
