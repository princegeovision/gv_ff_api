# App Folder
add_executable(gv_ff_app gv_ff_main.cc)
# add 3rd-ext1
# M1=nlohmann_json
# ?=spdlog::spdlog_header_only
target_link_libraries(gv_ff_app
                    PUBLIC
                    gv_ff_api
                    fmt::fmt)
