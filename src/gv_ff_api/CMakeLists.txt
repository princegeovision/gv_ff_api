# Add Lib
add_library(gv_ff_api 
    STATIC
    gv_ff_api.cpp
    reader/ff_reader.cpp
    reader/reader_handler.cpp
    decoder/ff_decoder.cpp)
# ff_decoder.cc

# Add Header into target
# target_sources(gv_ff_api 
#     PUBLIC 
#     include/reader/ff_reader.h
#     include/reader/ff_reader_type.h)
target_sources(gv_ff_api 
    PRIVATE 
    include/reader/ff_reader.h
    include/reader/ff_reader_type.h
    reader/reader_handler.hpp
    reader/ff_reader_internal.h)

# M2-1-B
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2
)
FetchContent_MakeAvailable(spdlog)

# for FFMPEG
find_package(FFmpeg REQUIRED)
include_directories(${FFMPEG_INCLUDE_DIRS})
# FFMPEG-End

# - spdlog::spdlog_header_only
target_link_libraries(gv_ff_api
                    PUBLIC
                    fmt::fmt
                    ${FFMPEG_LIBRARIES}
                    spdlog::spdlog_header_only)

# Header client find header file
# org target_include_directories(pg_client PUBLIC "./")
target_include_directories(gv_ff_api PUBLIC "./include")
target_include_directories(gv_ff_api PUBLIC "./include/decoder")
target_include_directories(gv_ff_api PUBLIC "./include/reader")
