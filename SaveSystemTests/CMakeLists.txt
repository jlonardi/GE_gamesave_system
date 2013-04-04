cmake_minimum_required(VERSION 2.6)

# These all are compiled in a same way
set(PROGRAMS Main.cpp NodeLoadTests.cpp NodeDataAdditionTests.cpp NodeSavingTests.cpp)
# Add them as executables
add_executable(SaveSystemTests ${PROGRAMS})

# Links the tests against the SaveSystem
target_link_libraries(SaveSystemTests SaveSystem)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
	target_link_libraries(SaveSystemTests ${GE_SaveSystem_SOURCE_DIR}/UnitTest++/lib/libUnitTest++.a)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	IF(WIN32)
	# Windows specific code
	# Links the tests against the UnitTest++ debug libraries
	target_link_libraries(SaveSystemTests debug ${GE_SaveSystem_SOURCE_DIR}/UnitTest++/lib/UnitTest++d.lib)
	# Links the tests against the UnitTest++ relsease libraries
	target_link_libraries(SaveSystemTests optimized ${GE_SaveSystem_SOURCE_DIR}/UnitTest++/lib/UnitTest++.lib)
	ENDIF(WIN32)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
									
include_directories(${GE_SaveSystem_SOURCE_DIR}/UnitTest++/src ${GE_SaveSystemT_SOURCE_DIR}/SaveSystem)
