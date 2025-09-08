cmake_minimum_required(VERSION 3.16)

# Configura las opciones por default del sdkconfig para una placa  en específico
set(SDKCONFIG_DEFAULTS "sdkconfig_esp32_2MB.defaults")

# set the project name and version from the BuildNumber file
file (STRINGS "BuildNumber" BUILD_NUMBER)
set(PROJECT_VER "${BUILD_NUMBER}")

# Add the project
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_basic_project)