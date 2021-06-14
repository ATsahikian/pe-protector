
add_executable(compiler
    CCompile.h
    CCompile.cpp
    CLexicalAnalizer.h
    CLexicalAnalizer.cpp
    main.cpp
    Opcodes.def)

target_link_libraries(compiler
  PRIVATE
    Library
    LogLibrary)
