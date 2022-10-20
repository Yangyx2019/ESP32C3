# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Espressif/frameworks/esp-idf-v4.4.2/components/bootloader/subproject"
  "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader"
  "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader-prefix"
  "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader-prefix/tmp"
  "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader-prefix/src"
  "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Yang_2019/Desktop/gatt_server_noweb/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
