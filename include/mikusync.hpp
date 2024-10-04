#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#include <algorithm>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iomanip>
#include <nlohmann/json.hpp>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include "stb_image.h"


struct note{
    int duration;
    std::string lyric;
};

struct part{
    std::string name;
    std::vector<note> notes;
};

struct track{
    std::string name;
    std::vector<part> parts;
};

uint16_t read_asint16(std::vector<char>& buffer, int index);
uint32_t read_asint24(std::vector<char>& buffer, int index);
uint32_t read_asint32(std::vector<char>& buffer, int index);
int read_vpr(std::vector<char>& buffer, std::vector<std::string>& messages);
std::string get_file();
int processa_json(std::vector<char>& buffer, std::vector<track>& tracks);


/*
    IMGUI FUNCTIONS
*/


//Data
extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext;
extern IDXGISwapChain*          g_pSwapChain;
extern bool                     g_SwapChainOccluded;
extern UINT                     g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;



// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//
bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);


