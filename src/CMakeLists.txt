set(DIRECTX "C:/Program Files (x86)/Windows Kits/10/Include")
set(DIRECTX_LIB "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64")
add_executable(palle main.cpp decode.cpp ../include/imgui_impl_dx11.cpp ../include/imgui_impl_win32.cpp interface.cpp)
find_package(imgui CONFIG REQUIRED)

target_include_directories(palle PRIVATE 
                ../include/
                "${DIRECTX/}"
                )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 ")
target_link_libraries(palle PRIVATE 
                fmt::fmt-header-only
                imgui::imgui
                "${DIRECTX_LIB}/d3d11.lib"
                "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64/WinMM.Lib"
                )

add_custom_command(TARGET palle
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy 
                    $<TARGET_FILE:palle> ${CMAKE_SOURCE_DIR}
)