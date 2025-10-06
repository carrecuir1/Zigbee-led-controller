cmake_minimum_required(VERSION 3.22)

project(stm32WB55RX)
add_library(stm32WB55RX INTERFACE)


# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32WB55RX INTERFACE 
    DEBUG
	USE_HAL_DRIVER 
    USE_STM32WBXX_NUCLEO
	STM32WB55xx
    ZIGBEE_WB
)

# Collect all directories containing header files
macro(collect_directories HEADER_LIST OUTPUT_VAR)
    set(TEMP_DIRS "")
    foreach(FILE ${HEADER_LIST})
        get_filename_component(DIR ${FILE} DIRECTORY)
        list(APPEND TEMP_DIRS ${DIR})
    endforeach()
    list(REMOVE_DUPLICATES TEMP_DIRS)
    set(${OUTPUT_VAR} ${TEMP_DIRS})
endmacro()


file(GLOB_RECURSE KERNEL_HEADERS        ../../code/01_Kernel/*.h)

# Chooses which HAL to use
file(GLOB BSP_HEADERS                   ../../code/02_Bsp/*.h)
file(GLOB_RECURSE BSP_DEFINES_HEADERS   ../../code/02_Bsp/defines/*.h)

file(GLOB_RECURSE APP_HEADERS           ../../code/03_App/*.h)
file(GLOB_RECURSE THREADS_HEADERS       ../../code/04_Threads/*.h)
file(GLOB_RECURSE DRIVERS_HEADERS       ../../code/05_Drivers/*.h)
file(GLOB_RECURSE PERIPHERALS_HEADERS   ../../code/06_Peripherals/*.h)
file(GLOB_RECURSE MIDDLEWARE_HEADERS    ../../code/07_Middleware/*.h)
file(GLOB_RECURSE MISC_HEADERS          ../../code/08_Misc/*.h)

# Private systems
file(GLOB CONF_HEADERS                  ../../code/02_Bsp/ST/*.h)
file(GLOB_RECURSE CMSIS_HEADERS         ../../code/02_Bsp/ST/CMSIS/*.h)
file(GLOB_RECURSE STM32WBxx_HAL_HEADERS ../../code/02_Bsp/ST/STM32WBXX/STM32WBxx_HAL_Driver/*.h)
file(GLOB_RECURSE NUCLEO_HEADERS        ../../code/02_Bsp/ST/STM32WBXX/P-NUCLEO-WB55.Nucleo/*.h)
file(GLOB_RECURSE PERIPHERAL_ST         ../../code/06_Peripherals/ST/*.h)
file(GLOB_RECURSE STM32_WPAN            ../../code/07_Middleware/ST/STM32_WPAN/*.h)

collect_directories("${KERNEL_HEADERS}" KERNEL_DIRS)
collect_directories("${BSP_HEADERS}" BSP_DIRS)
collect_directories("${BSP_DEFINES_HEADERS}" DEFINES_DIRS)
collect_directories("${APP_HEADERS}" APP_DIRS)
collect_directories("${THREADS_HEADERS}" THREADS_DIRS)
collect_directories("${DRIVERS_HEADERS}" DRIVERS_DIRS)
collect_directories("${PERIPHERALS_HEADERS}" PERIPHERALS_DIRS)
collect_directories("${MIDDLEWARE_HEADERS}" MIDDLEWARE_DIRS)
collect_directories("${MISC_HEADERS}" MISC_DIRS)
collect_directories("${CONF_HEADERS}" CONF_DIRS)
collect_directories("${CMSIS_HEADERS}" CMSIS_DIRS)
collect_directories("${STM32WBxx_HAL_HEADERS}" STM32WBxx_HAL_DIRS)
collect_directories("${NUCLEO_HEADERS}" NUCLEO_DIRS)
collect_directories("${PERIPHERAL_ST}" PERIPHERAL_ST_DIRS)
collect_directories("${STM32_WPAN}" STM32_WPAN_DIRS)

target_include_directories(stm32WB55RX INTERFACE
    # Application
    ${KERNEL_DIRS}
    ${BSP_DIRS}
    ${DEFINES_DIRS}
    ${APP_DIRS}
    ${THREADS_DIRS}
    ${DRIVERS_DIRS}
    ${PERIPHERALS_DIRS}
    ${MIDDLEWARE_DIRS}
    ${MISC_DIRS}

    # Libraries
    ${CONF_DIRS}
    ${CMSIS_DIRS}
    ${STM32WBxx_HAL_DIRS}
    ${NUCLEO_DIRS}
    ${PERIPHERAL_ST_DIRS}
    ${STM32_WPAN_DIRS}
)

file(GLOB_RECURSE KERNEL_SOURCES        ../../code/01_Kernel/*.c)

# Chooses which HAL to use
file(GLOB BSP_SOURCES                   ../../code/02_Bsp/*.c)
file(GLOB_RECURSE BSP_DEFINES_SOURCES   ../../code/02_Bsp/defines/*.c)
file(GLOB CONF_SOURCES                  ../../code/02_Bsp/ST/*.c)
file(GLOB_RECURSE CMSIS_SOURCES         ../../code/02_Bsp/ST/CMSIS/*.c)
file(GLOB_RECURSE STM32WBxx_HAL_SOURCES ../../code/02_Bsp/ST/STM32WBXX/STM32WBxx_HAL_Driver/*.c)
file(GLOB_RECURSE NUCLEO_SOURCES        ../../code/02_Bsp/ST/STM32WBXX/P-NUCLEO-WB55.Nucleo/*.c)

file(GLOB_RECURSE APP_SOURCES           ../../code/03_App/*.c)
file(GLOB_RECURSE THREADS_SOURCES       ../../code/04_Threads/*.c)
file(GLOB_RECURSE DRIVERS_SOURCES       ../../code/05_Drivers/*.c)
file(GLOB_RECURSE PERIPHERALS_SOURCES   ../../code/06_Peripherals/*.c)
file(GLOB_RECURSE MIDDLEWARE_SOURCES    ../../code/07_Middleware/*.c)
file(GLOB_RECURSE MISC_SOURCES          ../../code/08_Misc/*.c)

target_sources(stm32WB55RX INTERFACE
    # outside
    ../../Startup/startup_stm32wb55rgvx.s
    
    ${KERNEL_SOURCES}

    ${BSP_DEFINES_SOURCES}
    ${BSP_SOURCES}
    ${CONF_SOURCES}
    ${APP_SOURCES}
    ${THREADS_SOURCES}
    ${DRIVERS_SOURCES}
    ${PERIPHERALS_SOURCES}
    ${MISC_SOURCES}
    ${MIDDLEWARE_SOURCES}

    ${CMSIS_SOURCES}
    ${STM32WBxx_HAL_SOURCES}
    ${NUCLEO_SOURCES}

)

# set_source_files_properties(
#     path/to/hal_file.c
#     PROPERTIES COMPILE_FLAGS "-w"
# )

target_link_directories(stm32WB55RX INTERFACE
    ../../code/07_Middleware/ST/STM32_WPAN/zigbee/lib/
)

target_link_libraries(stm32WB55RX INTERFACE
    stm32wb_zigbee_wb_lib
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


