find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(
  client-file-test
  ClientFileTest.cpp)

target_link_libraries(
  client-file-test
 PRIVATE
  Boost::unit_test_framework)

add_test(
 NAME
  client-file-test
 COMMAND
  client-file-test)

add_executable(
  command-test
  CommandTest.cpp)

target_link_libraries(
  command-test
 PRIVATE
  libcommon
  Boost::unit_test_framework )

add_test(
 NAME command-test
 COMMAND command-test)

add_executable(
  data-test
  DataTest.cpp)

target_link_libraries(
  data-test
 PRIVATE
  libpe-protector
  Boost::unit_test_framework)

add_test(
 NAME data-test
 COMMAND data-test)

add_executable(
  import-test
  ImportTest.cpp)

target_link_libraries(
  import-test
 PRIVATE
  libpe-protector
  Boost::unit_test_framework)

add_test(
 NAME import-test
 COMMAND import-test)

add_executable(
  instruction-test
  InstructionTest.cpp)

target_link_libraries(
  instruction-test
 PRIVATE
  libpe-protector
  Boost::unit_test_framework)

add_test(
 NAME instruction-test
 COMMAND instruction-test)

add_executable(
  parser-test
  ParserTest.cpp)

target_link_libraries(
  parser-test
 PRIVATE
  libcompiler
  Boost::unit_test_framework)

add_test(
 NAME parser-test
 COMMAND parser-test)

add_executable(
  mutation-test
  MutationTest.cpp)

target_link_libraries(
  mutation-test
 PRIVATE
  Boost::unit_test_framework
  libpe-protector)

add_test(
 NAME mutation-test
 COMMAND mutation-test)

add_executable(
  pe-header-test
  PeHeaderTest.cpp)

target_link_libraries(
  pe-header-test
 PRIVATE
  libpe-protector
  Boost::unit_test_framework)

add_test(
 NAME pe-header-test
 COMMAND pe-header-test)

add_executable(
  protect-pe-test
  ProtectPeTest.cpp)

target_link_libraries(
  protect-pe-test
 PRIVATE
  libpe-protector
  Boost::unit_test_framework)

add_test(
 NAME protect-pe-test
 COMMAND protect-pe-test)

add_executable(
    resources-test
    ResourcesTest.cpp)

target_link_libraries(
  resources-test
 PRIVATE
  libpe-protector
  Boost::unit_test_framework)

add_test(
 NAME resources-test
 COMMAND resources-test)