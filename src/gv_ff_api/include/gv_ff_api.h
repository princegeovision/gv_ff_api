#ifndef FF_API_H_
#define FF_API_H_
//CXX include Guard
#pragma once

//3rd-party
#include <spdlog/spdlog.h>
#include <fmt/core.h>

#include "ff_decoder.h"
#include "ff_reader.h"
#include "ff_file_handler.h"

namespace gv
{

/**
 * @brief Initialization of whole library
 * @attention this function must be called before any other function
 *
 * @param cfg : configure structure Pointer
 */
void ff_api_init(void *cfg);
/**
 * @brief Shutdown of whole Library
 * @attention this funtion must be call at the end of library usage
 */
void ff_api_shutdown();

void ff_api_version(char* pVersion);

void ff_api_command();

}

#endif  // FF_API_H_
