cmake_minimum_required(VERSION 3.15)

project(stm32-eth-usb ASM C)

set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(SRC_FILES
   STM32CubeF2/Drivers/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc/startup_stm32f207xx.s
   STM32CubeF2/Drivers/CMSIS/Device/ST/STM32F2xx/Source/Templates/system_stm32f2xx.c
   
   STM32CubeF2/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal.c
   STM32CubeF2/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_cortex.c
   STM32CubeF2/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_gpio.c
   STM32CubeF2/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_rcc.c
   STM32CubeF2/Drivers/STM32F2xx_HAL_Driver/Src/stm32f2xx_hal_uart.c

   main.c
   uart.c
)

set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE} ${SRC_FILES})

target_compile_definitions(${EXECUTABLE} PRIVATE
   -DUSE_HAL_DRIVER
   -DSTM32F207xx
)

target_include_directories(${EXECUTABLE} PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}

   STM32CubeF2/Drivers/CMSIS/Device/ST/STM32F2xx/Include
   STM32CubeF2/Drivers/CMSIS/Include

   STM32CubeF2/Drivers/STM32F2xx_HAL_Driver/Inc
)

target_compile_options(${EXECUTABLE} PRIVATE  
   -mcpu=cortex-m3
   -mthumb
   -mfloat-abi=soft

   -fdata-sections
   -ffunction-sections

   -Wall

   -O3
   -g
)

target_link_options(${EXECUTABLE} PRIVATE
   -T${CMAKE_SOURCE_DIR}/STM32F207ZGTx_FLASH.ld
      
   -mcpu=cortex-m3
   -mthumb
   -mfloat-abi=soft

   -specs=nosys.specs
   -specs=nano.specs

   -Wl,-Map=${PROJECT_NAME}.map,--cref
   -Wl,--gc-sections
)

add_custom_command(TARGET ${EXECUTABLE}
   POST_BUILD
   COMMAND arm-none-eabi-size ${EXECUTABLE}
)

add_custom_target(flash
   DEPENDS ${EXECUTABLE}
   COMMAND openocd -f ../st_nucleo_f2.cfg -c init -c "reset halt" -c "flash write_image erase ${EXECUTABLE}" -c "reset run" -c exit
   VERBATIM
)

add_custom_target(gdb
   DEPENDS ${EXECUTABLE}
   COMMAND ${CMAKE_SOURCE_DIR}/gdb.sh ${EXECUTABLE}
   VERBATIM
)
