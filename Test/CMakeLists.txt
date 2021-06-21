find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(
    client-file-test 
    ClientFileTest.cpp)

target_link_libraries(
    client-file-test 
  PRIVATE 
    Boost::unit_test_framework)

add_test(
  NAME client-file-test 
  COMMAND client-file-test)

add_executable(
    command-test 
    CommandTest.cpp)
target_link_libraries(
    command-test 
  PRIVATE 
    Boost::unit_test_framework 
    Library
    LogLibrary)
add_test(
  NAME command-test
  COMMAND command-test)

add_executable(
    compile-test 
    CompileTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp)
        
target_link_libraries(
    compile-test
  PRIVATE 
    Boost::unit_test_framework
    Library
    LogLibrary)
        
add_test(
  NAME compile-test
  COMMAND compile-test)

  add_executable(
    data-test 
    DataTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
            
target_link_libraries(
    data-test
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
            
target_link_options(
    data-test 
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME data-test
  COMMAND data-test)

add_executable(
    import-test 
    ImportTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
                
target_link_libraries(
    import-test
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    import-test 
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME import-test
  COMMAND import-test)

add_executable(
    instruction-test 
    InstructionTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
target_link_libraries(
    instruction-test 
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    instruction-test  
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME instruction-test 
  COMMAND instruction-test)

add_executable(
    lexical-analizer-test 
    LexicalAnalizerTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
target_link_libraries(
    lexical-analizer-test  
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    lexical-analizer-test  
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME lexical-analizer-test  
  COMMAND lexical-analizer-test)

add_executable(
    mutation-test 
    MutationTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
target_link_libraries(
    mutation-test   
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    mutation-test   
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME mutation-test   
  COMMAND mutation-test)

add_executable(
    pe-header-test 
    PeHeaderTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
target_link_libraries(
    pe-header-test    
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    pe-header-test    
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME pe-header-test    
  COMMAND pe-header-test)

add_executable(
    protect-pe-test 
    ProtectPeTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
target_link_libraries(
    protect-pe-test     
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    protect-pe-test     
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME protect-pe-test     
  COMMAND protect-pe-test)

add_executable(
    resources-test 
    ResourcesTest.cpp
    ../Compile/CCompile.cpp
    ../Compile/CLexicalAnalizer.cpp
    ../PeProtector/Data.cpp
    ../PeProtector/Mutation.h
    ../PeProtector/Mutation.cpp
    ../PeProtector/ClientFile.h
    ../PeProtector/ClientFile.cpp
    ../PeProtector/Data.h
    ../PeProtector/Data.cpp
    ../PeProtector/Import.h
    ../PeProtector/Import.cpp
    ../PeProtector/Instruction.h
    ../PeProtector/Instruction.cpp
    ../PeProtector/InstructionModRM.h
    ../PeProtector/InstructionModRM.cpp
    ../PeProtector/Opcodes.h
    ../PeProtector/Opcodes.cpp
    ../PeProtector/PeHeader.h
    ../PeProtector/PeHeader.cpp
    ../PeProtector/PeProtector.rc
    ../PeProtector/ProtectPe.cpp
    ../PeProtector/ProtectPe.h
    ../PeProtector/resource.h
    ../PeProtector/Resources.h
    ../PeProtector/Resources.cpp)
target_link_libraries(
    resources-test      
  PRIVATE 
    Boost::unit_test_framework
    Library 
    LogLibrary
    Dbghelp
    "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")
target_link_options(
    resources-test      
  PRIVATE "/SAFESEH:NO")
add_test(
  NAME resources-test      
  COMMAND resources-test)