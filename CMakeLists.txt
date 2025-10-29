cmake_minimum_required(VERSION 3.14)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

unset(CMAKE_C_STANDARD_LIBRARIES CACHE)
unset(CMAKE_CXX_STANDARD_LIBRARIES CACHE)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(stm32_template C)

set(TARGET_NAME ${PROJECT_NAME})

if(DEFINED ENV{ARM_TOOLCHAIN_PATH})
    set(TOOLCHAIN_BIN_DIR $ENV{ARM_TOOLCHAIN_PATH})
    if(NOT TOOLCHAIN_BIN_DIR MATCHES ".+/$")
        set(TOOLCHAIN_BIN_DIR "${TOOLCHAIN_BIN_DIR}/")
    endif()
else()
    set(TOOLCHAIN_BIN_DIR "")
endif()

set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}arm-none-eabi-gcc)
find_program(OBJCOPY_EXECUTABLE ${TOOLCHAIN_BIN_DIR}arm-none-eabi-objcopy)
if(NOT OBJCOPY_EXECUTABLE)
    message(FATAL_ERROR "arm-none-eabi-objcopy not found! Please install ARM GCC toolchain and add it to PATH or set ARM_TOOLCHAIN_PATH.")
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -mlittle-endian -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-move-loop-invariants -fsigned-char -ffreestanding")

set(LINKER_FLAGS "-nostdlib -nostartfiles")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -Map -Xlinker ${TARGET_NAME}.map")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS ${LINKER_FLAGS})

add_definitions(-DSTM32F103C8 -DSTM32F10X_MD)

include_directories(cmsis stm32)

set(SOURCE_FILES
    stm32/startup/startup_stm32f10x_md.c
    stm32/system_stm32f10x.c
    syscalls/syscalls.c
    main.c
)

add_executable(${TARGET_NAME} ${SOURCE_FILES})

set(ELF_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
set(BIN_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.bin")
set(HEX_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.hex")

add_custom_command(
    OUTPUT ${BIN_TARGET}
    COMMAND ${OBJCOPY_EXECUTABLE} -O binary ${ELF_TARGET} ${BIN_TARGET}
    DEPENDS ${TARGET_NAME}
    COMMENT "Generating ${BIN_TARGET}"
)

add_custom_command(
    OUTPUT ${HEX_TARGET}
    COMMAND ${OBJCOPY_EXECUTABLE} -O ihex ${ELF_TARGET} ${HEX_TARGET}
    DEPENDS ${TARGET_NAME}
    COMMENT "Generating ${HEX_TARGET}"
)

add_custom_target(bin DEPENDS ${BIN_TARGET})
add_custom_target(hex DEPENDS ${HEX_TARGET})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit DESTINATION ${EXECUTABLE_OUTPUT_PATH})
endif()

find_program(STFLASH_EXECUTABLE st-flash)
if(STFLASH_EXECUTABLE)
    add_custom_target(flash
        COMMAND ${STFLASH_EXECUTABLE} write ${BIN_TARGET} 0x8000000
        DEPENDS ${BIN_TARGET}
        COMMENT "Flashing ${BIN_TARGET} to STM32"
    )
endif()