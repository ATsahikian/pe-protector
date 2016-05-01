
add_executable(
  hello-world
  hello-world.cpp)

add_test(
 NAME
  protect-hello-world
 COMMAND
  pe-protector $<TARGET_FILE:hello-world>)

add_test(
 NAME
  protected-hello-world-test
 COMMAND
  $<TARGET_FILE_DIR:hello-world>/protected-$<TARGET_FILE_NAME:hello-world>)
