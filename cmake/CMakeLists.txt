#cmake_minimum_required(VERSION 3.9.2)
include(ExternalProject)

if(POLICY CMP0074)
	cmake_policy(SET CMP0074 NEW) # Suppress warning for zlib_ROOT variable
endif()

set_property (DIRECTORY PROPERTY EP_BASE third_party)
SET(EP_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party" PARENT_SCOPE)
SET(EP_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party")

# Set parallelization
include(ProcessorCount)
ProcessorCount(NUM_CORES)
SET(NUM_PARALLEL 1)
if (NUM_CORES GREATER_EQUAL 2)
    math(EXPR NUM_PARALLEL "${NUM_CORES} - 1")
endif()

# For development (ignore)
option(USE_SYSTEM_GTEST "Use system dependency" OFF)
option(USE_SYSTEM_EIGEN "Use system dependency" OFF)
option(USE_SYSTEM_PROTOBUF "Use system dependency" OFF)
option(USE_SYSTEM_ZLIB "Use system dependency" OFF)
option(USE_SYSTEM_OPENSSL "Use system dependency" OFF)
#if(WIN32)
#    option(USE_SYSTEM_PTHREADS "Use system dependency" OFF)
#endif()

# Set variables
set(EDDL_DEPENDENCIES)

# Collect args
get_cmake_property(vars CACHE_VARIABLES)
foreach(var ${vars})
    get_property(currentHelpString CACHE "${var}" PROPERTY HELPSTRING)
    if("${currentHelpString}" MATCHES "No help, variable specified on the command line." OR "${currentHelpString}" STREQUAL "")
        # message("${var} = [${${var}}]  --  ${currentHelpString}") # uncomment to see the variables being processed
        list(APPEND CL_ARGS "-D${var}=${${var}}")
    endif()
endforeach()

#################################################################################
#################################################################################
macro(CONF_PACKAGE package_name build_type only_substitution)
    configure_file(${package_name}.CMakeLists.txt.in ${EP_BASE_DIR}/${package_name}-download/CMakeLists.txt ${only_substitution})
    set(conf_command ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . -DCMAKE_BUILD_TYPE=${build_type})
    if(CMAKE_GENERATOR_PLATFORM)
        list(APPEND conf_command -A ${CMAKE_GENERATOR_PLATFORM})
    endif()
    execute_process(COMMAND ${conf_command}
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${EP_BASE_DIR}/${package_name}-download)
    if(result)
        message(FATAL_ERROR "CMake step for ${package_name} failed: ${result}")
    endif()
endmacro()

macro(BUILD_PACKAGE package_name build_type)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . -j ${NUM_PARALLEL} --config ${build_type}
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${EP_BASE_DIR}/${package_name}-download)
    if(result)
        message(FATAL_ERROR "Build step for ${package_name} failed: ${result}")
    endif()
endmacro()

set(config_types ${CMAKE_BUILD_TYPE})
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    # Build debug and release for multi-configuration generator
    # this also installs libraries with "d" suffix
    set(config_types "Debug;Release")
    list(REMOVE_ITEM config_types ${CMAKE_BUILD_TYPE})
    list(APPEND config_types ${CMAKE_BUILD_TYPE})
endif()

#############
##  GTEST  ##
#############
message(STATUS "Subproject: GTEST...")
if(USE_SYSTEM_GTEST)
    find_package(GTest REQUIRED)
else()
    # Download and unpack googletest at configure time
    foreach(config ${config_types})
        CONF_PACKAGE(googletest ${config} "")
        BUILD_PACKAGE(googletest ${config})
    endforeach()

    # Set variables
    SET(GTEST_ROOT "${EP_BASE_DIR}/googletest" PARENT_SCOPE)
    SET(GTEST_INCLUDE_DIRS "${EP_BASE_DIR}/googletest/include" PARENT_SCOPE)
endif()
message(STATUS "Subproject: GTEST...DONE")


##############
###  EIGEN  ##
##############
message(STATUS "Subproject: EIGEN...")
if(USE_SYSTEM_EIGEN)
    find_package(Eigen3 3.3 REQUIRED NO_MODULE)
else()
    CONF_PACKAGE(eigen ${CMAKE_BUILD_TYPE} @ONLY)
    BUILD_PACKAGE(eigen ${CMAKE_BUILD_TYPE})

    # Set variables
    SET(Eigen3_DIR "${EP_BASE_DIR}/eigen/share/eigen3/cmake" PARENT_SCOPE)
endif()
message(STATUS "Subproject: EIGEN...DONE")


################
##  PROTOBUF  ##
################
if(USE_SYSTEM_PROTOBUF)
    find_package(Protobuf REQUIRED)
else()
    # Download and unpack protobuf at configure time
    foreach(config ${config_types})
        CONF_PACKAGE(protobuf ${config} @ONLY)
        BUILD_PACKAGE(protobuf ${config})
    endforeach()

    set(Protobuf_ROOT "${EP_BASE_DIR}/protobuf" PARENT_SCOPE)
    set(Protobuf_INCLUDE_DIRS "${EP_BASE_DIR}/protobuf/include" PARENT_SCOPE)
    set(Protobuf_PROTOC_EXECUTABLE "${EP_BASE_DIR}/protobuf/bin/protoc" PARENT_SCOPE)
    set(Protobuf_USE_STATIC_LIBS ON PARENT_SCOPE) # Always static when superbuild is enabled
endif()
add_custom_target(protobuf_files
	protoc --cpp_out=../src/serialization/onnx ../src/serialization/onnx/onnx.proto
	)
message(STATUS "Subproject: Protobuf...DONE")


# Only for distributed versions
if(BUILD_DIST)

    ############
    ##  ZLIB  ##
    ############
    message(STATUS "Subproject: ZLIB...")
    if(USE_SYSTEM_ZLIB)
        find_package(ZLIB REQUIRED)
    else()
        # Download and unpack ZLIB at configure time
        foreach(config ${config_types})
            CONF_PACKAGE(zlib ${config} "")
            BUILD_PACKAGE(zlib ${config})
        endforeach()
        # Set variables
        SET(ZLIB_ROOT "${EP_BASE_DIR}/zlib" PARENT_SCOPE)
        SET(ZLIB_INCLUDE_DIRS "${EP_BASE_DIR}/zlib/include" PARENT_SCOPE)
    endif()
    message(STATUS "Subproject: ZLIB...DONE")


    ###############
    ##  OPENSSL  ##
    ###############
    message(STATUS "Subproject: OpenSSL...")
    if(USE_SYSTEM_OPENSSL)
        find_package(OpenSSL REQUIRED)
    else()
        # Download and unpack ZLIB at configure time
        foreach(config ${config_types})
            CONF_PACKAGE(openssl ${config} "")
            BUILD_PACKAGE(openssl ${config})
        endforeach()
        # Set variables
        SET(OPENSSL_ROOT_DIR "${EP_BASE_DIR}/openssl" PARENT_SCOPE)
        SET(OPENSSL_INCLUDE_DIR "${EP_BASE_DIR}/openssl/include" PARENT_SCOPE)
    endif()
    message(STATUS "Subproject: OpenSSL...DONE")

endif()

##################
##  PTHREADS4W  ##
##################
#if(WIN32)
#    if(USE_SYSTEM_PTHREADS)
#        set(PTHREADS_INSTALL_PATH "$ENV{PTHREADS_ROOT}" CACHE PATH "Path to the installation of Pthreads under Windows (PTHREADS_ROOT env variable)" PARENT_SCOPE)
#    else()
#        # Download and unpack pthreads4w at configure time
#        CONF_PACKAGE(pthreads4w ${CMAKE_BUILD_TYPE} @ONLY)
#        BUILD_PACKAGE(pthreads4w ${CMAKE_BUILD_TYPE})
#
#        set(PTHREADS_INSTALL_PATH "${EP_BASE_DIR}/PTHREADS-BUILT" PARENT_SCOPE)
#    endif()
#endif()

###############
#####  EDDL  ##
###############
#ExternalProject_Add(eddl
#        PREFIX eddl
#        DEPENDS ${EDDL_DEPENDENCIES}
#        SOURCE_DIR "${PROJECT_SOURCE_DIR}"
#        BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/eddl"
#        INSTALL_COMMAND ""
#        CMAKE_ARGS ${CL_ARGS}  # Overrides CMAKE_CACHE_ARGS when an argument is duplicated
#        CMAKE_CACHE_ARGS
#            -DUSE_SUPERBUILD:BOOL=OFF
#            -DUSE_SYSTEM_EIGEN:BOOL=${USE_SYSTEM_EIGEN}
#            -DUSE_SYSTEM_GTEST:BOOL=${USE_SYSTEM_GTEST}
#            -DUSE_SYSTEM_ZLIB:BOOL=${USE_SYSTEM_ZLIB}
#            -DUSE_SYSTEM_PROTOBUF:BOOL=${USE_SYSTEM_PROTOBUF}
#
#            -DEigen3_DIR:PATH=${Eigen3_DIR}
#
#            -DGTEST_ROOT:PATH=${GTEST_ROOT}  # Special
#
#            -DZLIB_ROOT:PATH=${ZLIB_ROOT}  # Problems
#            -DZLIB_INCLUDE_DIRS:PATH=${ZLIB_INCLUDE_DIRS}
#            -DZLIB_LIBRARIES:PATH=${ZLIB_LIBRARIES}
#            -DZLIB_LIBRARIES_DIRS:PATH=${ZLIB_LIBRARIES_DIRS}
#
#            -DProtobuf_ROOT:PATH=${Protobuf_ROOT}  # Problems
#            -DProtobuf_INCLUDE_DIRS:PATH=${Protobuf_INCLUDE_DIRS}
#            -DProtobuf_LIBRARIES:PATH=${Protobuf_LIBRARIES}
#            -DProtobuf_LIBRARIES_DIRS:PATH=${Protobuf_LIBRARIES_DIRS}
#            -DProtobuf_PROTOC_EXECUTABLE:PATH=${Protobuf_PROTOC_EXECUTABLE}
#        )


## SHOW OPTIONS
#message(STATUS "===========================================" )
#message(STATUS "===========================================" )
#message(STATUS "Cores used: " ${NUM_PARALLEL} )
#message(STATUS "Args used: " ${CL_ARGS} )
#message(STATUS "-------------------------------------------" )
#message(STATUS "Use system GTest: " ${USE_SYSTEM_GTEST} )
#message(STATUS "GTests root: " ${GTEST_ROOT})
#message(STATUS "-------------------------------------------" )
#message(STATUS "Use system Eigen: " ${USE_SYSTEM_EIGEN} )
#message(STATUS "Eigen dir: " ${Eigen3_DIR})
#message(STATUS "-------------------------------------------" )
#message(STATUS "Use system Zlib: " ${USE_SYSTEM_ZLIB} )
#message(STATUS "Zlib root: " ${ZLIB_ROOT})
#message(STATUS "Zlib include dirs: " ${ZLIB_INCLUDE_DIRS})
#message(STATUS "Zlib libraries dirs: " ${ZLIB_LIBRARIES_DIRS})
#message(STATUS "Zlib libraries: " ${ZLIB_LIBRARIES})
#message(STATUS "-------------------------------------------" )
#message(STATUS "Use system Protobuf: " ${USE_SYSTEM_PROTOBUF} )
#message(STATUS "Protobuf dir: " ${Protobuf_ROOT})
#message(STATUS "Protobuf include dirs: " ${Protobuf_INCLUDE_DIRS})
#message(STATUS "Protobuf libraries dirs: " ${Protobuf_LIBRARIES_DIRS})
#message(STATUS "Protobuf libraries: " ${Protobuf_LIBRARIES})
#message(STATUS "Protobuf compiler: " ${Protobuf_PROTOC_EXECUTABLE})
#message(STATUS "===========================================" )
#message(STATUS "===========================================" )
