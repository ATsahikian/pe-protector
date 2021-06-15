add_executable(pe-protector 
    main.cpp
    ClientFile.h
    ClientFile.cpp
    Data.h
    Data.cpp
    Import.h
    Import.cpp
    Instruction.h
    Instruction.cpp
    InstructionModRM.h
    InstructionModRM.cpp
    Mutation.h
    Mutation.cpp
    Opcodes.h
    Opcodes.cpp
    PeHeader.h
    PeHeader.cpp
    PeProtector.rc
    ProtectPe.cpp
    ProtectPe.h
    resource.h
    Resources.h
    Resources.cpp)

target_link_options (pe-protector PRIVATE "/SAFESEH:NO")

#TODO: asm file, bin file, custor rule to compile asm to bin
target_link_libraries(pe-protector PRIVATE
    Library
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
