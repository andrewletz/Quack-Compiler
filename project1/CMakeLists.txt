cmake_minimum_required(VERSION 3.12)
project(QuackParser)

set(CMAKE_CXX_STANDARD 14)

cmake_minimum_required(VERSION 3.12)


# Where are the reflex include and library files?
#
set(REFLEX_LIB "/usr/local/lib/libreflex.a")
set(REFLEX_INCLUDE "/usr/local/include/reflex")

# Recipes for generating C++ source files from .lxx and .yxx files
#

add_custom_command(
        OUTPUT lex.yy.cpp lex.yy.h
        COMMAND   reflex --bison-cc --bison-locations --header-file ${CMAKE_CURRENT_SOURCE_DIR}/quack.lxx
        DEPENDS quack.lxx quack.tab.hxx
)
add_custom_command(
        OUTPUT quack.tab.cxx quack.tab.hxx location.hh position.hh stack.hh
        COMMAND bison -d ${CMAKE_CURRENT_SOURCE_DIR}/quack.yxx
        DEPENDS quack.yxx
)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${REFLEX_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(parser
        quack.tab.cxx lex.yy.cpp lex.yy.h
        parser.cpp ASTNode.cpp ASTNode.h Messages.h Messages.cpp )

add_executable(lexer
        lex_driver.cpp
        lex.yy.cpp lex.yy.h
        location.hh position.hh
        quack.tab.hxx
        ASTNode.cpp ASTNode.h Messages.h Messages.cpp
        names.h)


target_link_libraries(parser ${REFLEX_LIB})
target_link_libraries(lexer ${REFLEX_LIB})
