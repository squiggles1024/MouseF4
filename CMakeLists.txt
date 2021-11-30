#Build with cmake -DCMAKE_MAKE_PROGRAM=make.exe -G "Unix Makefiles" ..
cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain.cmake)

###############################################################################
# NAME / PROJECT
###############################################################################
project(mmo-mouse-V1)
set(EXECUTABLE ${PROJECT_NAME}.elf)
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/device/STM32F412RETX_FLASH.ld)

###############################################################################
# LANGUAGE CONFIG
###############################################################################
enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Optional: issue a message to be sure it uses the correct toolchain file.
message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")


###############################################################################
# SOURCE
###############################################################################
set(APP_SRC_DIR "Core/Src/")
set(DEVICE_SRC_DIR "device/")
set(USB_MIDDLEWARE_CLASS_DIR "Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/")
set(USB_MIDDLEWARE_CORE_DIR "Middlewares/ST/STM32_USB_Device_Library/Core/Src/")
set(STM32_HAL_DIR "STM32_HAL/Src/")
set(USB_DEVICE_APP_DIR "USB_DEVICE/App/")
set(USB_DEVICE_TARGET_DIR "USB_DEVICE/Target/")
set(SRC_FILES
        ${STM32_HAL_DIR}stm32f4xx_hal_cortex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_dma_ex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_dma.c
        ${STM32_HAL_DIR}stm32f4xx_hal_exti.c
        ${STM32_HAL_DIR}stm32f4xx_hal_flash_ex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_flash_ramfunc.c
        ${STM32_HAL_DIR}stm32f4xx_hal_flash.c
        ${STM32_HAL_DIR}stm32f4xx_hal_gpio.c
        ${STM32_HAL_DIR}stm32f4xx_hal_i2c_ex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_i2c.c
        ${STM32_HAL_DIR}stm32f4xx_hal_msp.c
        ${STM32_HAL_DIR}stm32f4xx_hal_pcd_ex.c 
        ${STM32_HAL_DIR}stm32f4xx_hal_pcd.c
        ${STM32_HAL_DIR}stm32f4xx_hal_pwr_ex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_pwr.c
        ${STM32_HAL_DIR}stm32f4xx_hal_rcc_ex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_rcc.c
        ${STM32_HAL_DIR}stm32f4xx_hal_spi.c
        ${STM32_HAL_DIR}stm32f4xx_hal_tim_ex.c
        ${STM32_HAL_DIR}stm32f4xx_hal_tim.c 
        ${STM32_HAL_DIR}stm32f4xx_hal.c 
        ${STM32_HAL_DIR}stm32f4xx_ll_usb.c
        ${APP_SRC_DIR}main.c
        ${APP_SRC_DIR}button.c
        ${APP_SRC_DIR}delay_timer.c
        ${APP_SRC_DIR}eeprom.c
        ${APP_SRC_DIR}led.c
        ${APP_SRC_DIR}mouse.c
        ${APP_SRC_DIR}sensor.c
        ${APP_SRC_DIR}syscalls.c
        ${APP_SRC_DIR}wheel.c
        ${APP_SRC_DIR}stm32f4xx_it.c
        ${DEVICE_SRC_DIR}startup_stm32f412retx.s
        ${DEVICE_SRC_DIR}system_stm32f4xx.c
        ${USB_MIDDLEWARE_CLASS_DIR}usbd_customhid.c
        ${USB_MIDDLEWARE_CORE_DIR}usbd_core.c
        ${USB_MIDDLEWARE_CORE_DIR}usbd_ctlreq.c
        ${USB_MIDDLEWARE_CORE_DIR}usbd_ioreq.c
        ${USB_DEVICE_APP_DIR}usb_device.c
        ${USB_DEVICE_APP_DIR}usbd_desc.c
        ${USB_DEVICE_APP_DIR}usbd_custom_hid_if.c
        ${USB_DEVICE_TARGET_DIR}usbd_conf.c
        )

# Build the executable based on the source files
add_executable(${EXECUTABLE} ${SRC_FILES})

###############################################################################
# DEFINES
###############################################################################
target_compile_definitions(${EXECUTABLE} PRIVATE
    -DDEBUG 
    -DSTM32F412Rx 
    -DUSE_HAL_DRIVER
    )

###############################################################################
# INCLUDES
###############################################################################
set(CMSIS_DIR "CMSIS/")
set(CORE_INCLUDE_DIR "Core/Inc/")
set(DEVICE_INC_DIR "device/")
set(USB_MIDDLEWARE_CLASS_INC_DIR "Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc/")
set(USB_MIDDLEWARE_CORE_INC_DIR "Middlewares/ST/STM32_USB_Device_Library/Core/Inc/")
set(STM32_HAL_INC_DIR "STM32_Hal/Inc/")
set(STM32_HAL_LEGACY_INC_DIR "STM32_Hal/Inc/Legacy/")
set(USB_DEVICE_APP_INC_DIR "USB_DEVICE/App/")
set(USB_DEVICE_TARGET_INC_DIR "USB_DEVICE/Target/")
target_include_directories(${EXECUTABLE} PRIVATE
        ${CMSIS_DIR}
        ${CORE_INCLUDE_DIR}
        ${DEVICE_INC_DIR}
        ${USB_MIDDLEWARE_CLASS_INC_DIR}
        ${USB_MIDDLEWARE_CORE_INC_DIR}
        ${STM32_HAL_INC_DIR}
        ${STM32_HAL_LEGACY_INC_DIR}
        ${USB_DEVICE_APP_INC_DIR}
        ${USB_DEVICE_TARGET_INC_DIR}
        )

###############################################################################
# C FLAGS
###############################################################################
target_compile_options(${EXECUTABLE} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -g3
    -c
    -O0
    -ffunction-sections
    -fdata-sections
    -fstack-usage
    --specs=nano.specs
    -Wall   
    )

###############################################################################
# L FLAGS
###############################################################################
target_link_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m4 
        -T${LINKER_FILE} --specs=nosys.specs 
        -Wl,-Map=${PROJECT_NAME}.map,--cref  
        -Wl,--gc-sections 
        -static --specs=nano.specs 
        -mfpu=fpv4-sp-d16 
        -mfloat-abi=hard 
        -mthumb 
        -Wl,--start-group 
        -lc 
        -lm 
        -Wl,--end-group
        )

# Optional: Print executable size as part of the post build process
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE})

# Optional: Create hex, bin and S-Record files after the build
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${EXECUTABLE} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)