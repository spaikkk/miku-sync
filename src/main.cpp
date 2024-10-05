#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "mikusync.hpp"

ID3D11Device *g_pd3dDevice = nullptr;
ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
IDXGISwapChain *g_pSwapChain = nullptr;
bool g_SwapChainOccluded = false;
UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;

std::string get_file() {
  // open a dialog box to select the file
  char path[MAX_PATH] = {};
  OPENFILENAME ofn = {sizeof(ofn)};
  ofn.hwndOwner = NULL;
  ofn.lpstrFilter = "All Files\0*.*\0";
  ofn.lpstrFile = path;
  ofn.nMaxFile = ARRAYSIZE(path);

  BOOL open_ok = GetOpenFileName(&ofn);
  if (!open_ok) {
    std::cerr << "error opening file" << std::endl;
  }

  std::string str_path(path);

  // check if the extension is correct
  std::filesystem::path path_obj = str_path;
  if (path_obj.extension() != ".vpr") {
    std::cerr << "File extension not recognised" << std::endl;
  }
  return str_path;
}

int main() {

  /*
      GUI STUFF
  */

  std::vector<std::string> messages;
  std::vector<track> tracce;
  // Create application window
  WNDCLASSEXW wc = {sizeof(wc),
                    CS_CLASSDC,
                    WndProc,
                    0L,
                    0L,
                    GetModuleHandle(nullptr),
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    L"ImGui Example",
                    nullptr};
  ::RegisterClassExW(&wc);
  HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Spike's MikuSync",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr,
                              nullptr, wc.hInstance, nullptr);

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  // Our state
  bool show_demo_window = false;
  bool show_another_window = false;
  bool palle = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // still image rendering
  int my_image_width = 0;
  int my_image_height = 0;
  ID3D11ShaderResourceView *my_texture = NULL;
  bool ret = LoadTextureFromFile("damiano.jpg", &my_texture, &my_image_width,
                                 &my_image_height);

  IM_ASSERT(ret);
  //
  // Main loop
  bool coglioni = true;
  bool done = false;
  bool log_window = false;
  bool json_window = false;
  bool random = false;
  bool sbinnala = false;
  while (!done) {
    // Poll and handle messages
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT)
        done = true;
    }
    if (done)
      break;

    // Handle window being minimized or screen locked
    if (g_SwapChainOccluded &&
        g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      ::Sleep(10);
      continue;
    }
    g_SwapChainOccluded = false;

    // Handle window resize
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
      CleanupRenderTarget();
      g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight,
                                  DXGI_FORMAT_UNKNOWN, 0);
      g_ResizeWidth = g_ResizeHeight = 0;
      CreateRenderTarget();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 main_size = io.DisplaySize;
    ImGui::SetNextWindowSize(main_size);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Spaik's MikuSync");
      if (ImGui::Button("Select File")) {
        // open the file and store it in a vector
        std::string path = get_file();
        std::ifstream zip_file(path, std::ios::in | std::ios::binary);
        if (!zip_file.is_open()) {
          std::cerr << "File non aperto" << std::endl;
        }
        uint32_t fsize = std::filesystem::file_size(path);
        std::cout << fsize << std::endl;
        std::vector<char> buffer(fsize);
        zip_file.read(buffer.data(), fsize);
        zip_file.close();

        read_vpr(buffer, messages);
        json_window = processa_json(buffer, tracce);
      }
      ImGui::Text("Seleziona la traccia da associare al lip sync");
      ImGui::Checkbox("Soft [VB: Hatsune Miku]", &palle);
      ImGui::Checkbox("Power [VB: Hatsune Miku]", &palle);
      ImGui::Checkbox("Choirs [VB: Kasane Teto]", &palle);
      if (ImGui::Button("Damiano Brividi")) {
        show_another_window = true;
      }
      if (ImGui::Button("Decode Log")) {
        log_window = true;
      }

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }
    if (json_window) {
      ImGui::Text("Rilevate tracce multiple!");
      for (auto traccia : tracce) {
        std::string messaggio = fmt::format("Name: {}", traccia.name);
        ImGui::Checkbox(messaggio.c_str(), &random);
      }
      ImGui::End();
    }

    if (log_window) {
      ImGui::Begin("Decode log", &log_window);

      for (auto mess : messages) {
        ImGui::Text("%s", mess.c_str());
      }

      ImGui::Text("Prova");
      ImGui::End();
    }
    // 3. Show another simple window.
    if (show_another_window) {
      ImGui::Begin("Damiano", &show_another_window);

      ImGui::Text("Vespasiano dei rasputin dice:");
      ImGui::Image((void *)my_texture, ImVec2(my_image_width, my_image_height));

      if (ImGui::Button("Mi sono toccato i coglioni"))
        show_another_window = false;

      ImGui::End();
      if (coglioni) {
        PlaySound(TEXT("fuori.wav"), NULL, SND_FILENAME | SND_ASYNC);
        coglioni = false;
      }
    } else {
      coglioni = true;
    }

    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w, clear_color.y * clear_color.w,
        clear_color.z * clear_color.w, clear_color.w};
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView,
                                            nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView,
                                               clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    HRESULT hr = g_pSwapChain->Present(1, 0); // Present with vsync
    g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
  }

  // Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  ::DestroyWindow(hwnd);
  ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

  return 0;
}
