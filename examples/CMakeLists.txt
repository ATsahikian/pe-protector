
add_executable(
  hello-world
  hello-world.cpp)

add_custom_command(
 TARGET hello-world POST_BUILD
 COMMAND
  pe-protector $<TARGET_FILE:hello-world>
 COMMAND
  $<TARGET_FILE_DIR:hello-world>/protected-$<TARGET_FILE_NAME:hello-world>)
