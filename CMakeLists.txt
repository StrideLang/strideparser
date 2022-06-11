cmake_minimum_required(VERSION 3.13) 
project(strideparser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PARSER_SRC
  src/ast.cpp
  src/astfunctions.cpp
  src/astquery.cpp
  src/astvalidation.cpp
  src/astruntime.cpp
  src/streamnode.cpp
  src/valuenode.cpp
  src/bundlenode.cpp
  src/propertynode.cpp
  src/functionnode.cpp
  src/expressionnode.cpp
  src/listnode.cpp
  src/importnode.cpp
  src/rangenode.cpp
  src/langerror.cpp
  src/keywordnode.cpp
  src/declarationnode.cpp
  src/blocknode.cpp
  src/scopenode.cpp
  src/platformnode.cpp
  src/portpropertynode.cpp
)

set(PARSER_HEADERS
  include/ast.h
  include/astfunctions.h
  include/astquery.h
  include/astruntime.h
  include/astvalidation.h
  include/streamnode.h
  include/valuenode.h
  include/bundlenode.h
  include/propertynode.h
  include/functionnode.h
  include/expressionnode.h
  include/listnode.h
  include/importnode.h
  include/rangenode.h
  include/langerror.h
  include/strideparser.h
  include/declarationnode.h
  include/blocknode.h
  include/scopenode.h
  include/keywordnode.h
  include/platformnode.h
  include/portpropertynode.h
)

if (WIN32)

  # On Windows you need a lot of magic to get flex and bison working.
  # Download and install flex and bison from:
  # http://gnuwin32.sourceforge.net/packages/flex.htm
  # http://gnuwin32.sourceforge.net/packages/bison.htm
  # Bison is broken, must replace binary with: http://marin.jb.free.fr/bison/bison-2.4.1-modified.zip

  set(BISON_EXECUTABLE "C:/Program Files (x86)/GnuWin32/bin/bison.exe")
  set(FLEX_EXECUTABLE "C:/Program Files (x86)/GnuWin32/bin/flex.exe")

  # Previous procedure appears to be broken. This now works:
  # Download winflexbison from:
  # https://sourceforge.net/projects/winflexbison/files/win_flex_bison-2.5.24.zip/download
  # Point the following variables to the extracted fodler and binaries

  set(BISON_EXECUTABLE "C:/Users/Andres/source/repos/win_flex_bison/win_bison.exe")
  set(FLEX_EXECUTABLE "C:/Users/Andres/source/repos/win_flex_bison/win_flex.exe")

  set(FLEX_MyScanner_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/lang_stride.lexer.cpp")
  set(BISON_MyParser_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/lang_stride.parser.cpp")
  add_custom_command(
    COMMENT "Generating BISON: -o${BISON_MyParser_OUTPUTS} ${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.y"
          OUTPUT ${BISON_MyParser_OUTPUTS}
          DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.y"
          COMMAND "${BISON_EXECUTABLE}"
          -d
          -o"${BISON_MyParser_OUTPUTS}"
          "${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.y"
  )

  add_custom_command(
          OUTPUT ${FLEX_MyScanner_OUTPUTS}
          DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.l"
          COMMAND ${FLEX_EXECUTABLE} --wincompat
          -o"${FLEX_MyScanner_OUTPUTS}"
          "${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.l"
          COMMENT "Generating lang_stride.lexer.cpp"
  )

#  include_directories("C:/GnuWin32/include")
else()
  # On Linux you can use the standard cmake scripts
  if (APPLE)
    set(BISON_EXECUTABLE "/usr/local/Cellar/bison/3.8.2/bin/bison" CACHE PATH "Bison executable")
    find_package(BISON 3.8.2 REQUIRED)
  else()
    find_package(BISON)
  endif()

  find_package(FLEX)
  BISON_TARGET(MyParser ${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.y ${CMAKE_CURRENT_BINARY_DIR}/lang_stride.parser.cpp)
  FLEX_TARGET(MyScanner ${CMAKE_CURRENT_SOURCE_DIR}/lang_stride.l ${CMAKE_CURRENT_BINARY_DIR}/lang_stride.lexer.cpp)
  ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
endif()

add_library(StrideParser 
  ${PARSER_SRC} ${PARSER_HEADERS}
  ${BISON_MyParser_OUTPUTS}
  ${FLEX_MyScanner_OUTPUTS}
)

target_compile_options(StrideParser PRIVATE -fPIC)
target_include_directories(StrideParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)

