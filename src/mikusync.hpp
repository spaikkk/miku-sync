#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
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
#include "nlohmann/json.hpp"
#include <d3d11.h>
#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include "stb_image.h"

#define KUCHI_A 0
#define KUCHI_E 1
#define KUCHI_O 2
#define KUCHI_I 10
#define KUCHI_U 11
#define KUCHI_RESET 8


struct note{
    int pos;
    int duration;
    int kuchi;
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

struct custom_img{
    int img_x;
    int img_y;
    std::string name;
    ID3D11ShaderResourceView *texture;
};


uint16_t read_asint16(std::vector<char>& buffer, int index);
uint32_t read_asint24(std::vector<char>& buffer, int index);
uint32_t read_asint32(std::vector<char>& buffer, int index);
int read_vpr(std::vector<char>& buffer, std::vector<std::string>& messages);
std::string get_file();
int process_json(std::vector<char>& buffer, std::vector<track>& tracks);
int is_vowel(char c);
int is_close(char c);
int assign_kuchi(std::string phoneme);
std::string print_kuchi(int kuchi);

/*
    IMGUI FUNCTIONS
*/

void load_images(std::vector<custom_img>& images);

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


