# CMake script for buiding very simple parser
# with bison and 'reflex' scanner generator.
#
#
cmake_minimum_required(VERSION 3.5.1)
project(AST)
set(CMAKE_CXX_STANDARD 14)

# Where are the reflex include and library files?
#
find_path(ReflexPath libreflex.a PATHS /usr/local/lib/)

if(ReflexPath)
	set(REFLEX_LIB "/usr/local/lib/libreflex.a")
	set(REFLEX_INCLUDE "/usr/local/include/reflex")
else()
	set(REFLEX_LIB "/home/faculty/michal/classes/cis461/RE-flex/lib/libreflex.a")
	set(REFLEX_INCLUDE "/home/faculty/michal/classes/cis461/RE-flex/include")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

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

add_executable(qcc
	quack.tab.cxx lex.yy.cpp lex.yy.h typechecker.h typechecker.cpp
	ASTNode.cpp ASTNode.h driver.cpp stubs.h Messages.h Messages.cpp CodegenContext.cpp CodegenContext.h EvalContext.h)

target_link_libraries(qcc ${REFLEX_LIB})
