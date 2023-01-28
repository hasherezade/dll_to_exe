cmake_minimum_required ( VERSION 3.0 )
project (tests)

#add the application that will be used for tests:
add_subdirectory ( test_case1 )

#WARNING: in order for tests to work, all the binaries must be installed by: cmake --build . --target install

# 0) does dll_to_exe run
add_test(Dll2ExeRun "${CMAKE_INSTALL_PREFIX}//dll_to_exe.exe" "${CMAKE_INSTALL_PREFIX}//test_case1.dll" "${CMAKE_INSTALL_PREFIX}//test_case1.dll.exe" 1)
set_tests_properties(ConvTestCase1 PROPERTIES PASS_REGULAR_EXPRESSION "finished")

