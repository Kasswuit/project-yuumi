cmake_minimum_required(VERSION 3.16)

project(YuumiPet VERSION 0.1.0 LANGUAGES CXX)

add_executable(yuumi_pet
	src/main.cpp
	src/core/BehaviorEngine.cpp
	src/core/PetState.cpp
)

target_include_directories(yuumi_pet PRIVATE include)
target_compile_features(yuumi_pet PRIVATE cxx_std_17)

include(CTest)
if(BUILD_TESTING)
	add_executable(yuumi_pet_tests
		tests/PetCoreTests.cpp
		src/core/BehaviorEngine.cpp
		src/core/PetState.cpp
	)
	target_include_directories(yuumi_pet_tests PRIVATE include)
	target_compile_features(yuumi_pet_tests PRIVATE cxx_std_17)
	add_test(NAME yuumi_pet_core COMMAND yuumi_pet_tests)
endif()
