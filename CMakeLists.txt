cmake_minimum_required(VERSION 3.10)

project(GV_FF_API
    VERSION 0.1.0
    LANGUAGES C CXX
)
set(CXX_STANDARD_REQUIRED ON) #Make C++20 a hard requirement
#set(CMAKE_CXX_STANDARD 11) # Default C++ standard for targets
#set(CMAKE_CXX_STANDARD 11) # 11->fail (use std::make_unique)
set(CMAKE_CXX_STANDARD 14) # 14->ok
#set(CMAKE_CXX_STANDARD 17) # 17->ok
set(CMAKE_C_STANDARD 99)

# << vcpkg >>
# Dependency fmt
find_package(fmt CONFIG REQUIRED)

# Compile lib
add_subdirectory(src)
# Compile app
add_subdirectory(app_src)
