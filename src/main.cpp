#include <d3d11.h>
#include <filesystem>
#include <imgui.h>
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


  //create vectors to store decode log and json content
  std::vector<std::string> messages;
  std::vector<track> tracce;
  /*
      GUI STUFF
  */

 
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

  std::vector<custom_img> textures;
  load_images(textures);
  int testicoli = 0;
  for (auto &sesso : textures) {
    fmt::print("{}: {}", testicoli, sesso.name);
    testicoli++;
  }
  /*
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  */

  ImVec4 blue_colour = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

  // IM_ASSERT(ret);
  //
  int contatore_note_debug = 0;
  //  Main loop
  bool custom_lips_wdw = false;
  bool custom_lips_sel_wdw = false;
  bool coglioni = true;
  bool done = false;
  bool log_window = false;
  bool json_window = false;
  bool random = false;
  bool sbinnala = false;
  bool finestra_beta = false;
  int custom_lip_id;
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

        //process the json and store the data in a track struct
        json_window = process_json(buffer, tracce);
        std::cout << "DEBUG: NUMERO NOTE" << tracce[0].parts[0].notes.size() << std::endl;
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
      if (ImGui::Button("Beta Lips")) {
        finestra_beta = true;
      }
      if(ImGui::Button("Custom lips animation")){
        custom_lips_wdw = true;
      }

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }
    if (json_window) {
      ImGui::Begin("Tracce", &json_window);
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

    if(custom_lips_wdw){
      ImGui::Begin("Custom lips animation");
      
      std::vector<std::string> lip_lirycs;
      for(int note_butt_it = 0; note_butt_it <= tracce[0].parts[0].notes.size()-1; note_butt_it++){
        if((note_butt_it%8!=0)){
          ImGui::SameLine();
        }
        lip_lirycs.push_back(tracce[0].parts[0].notes[note_butt_it].lyric);
        ImGui::PushID(note_butt_it);
        if(ImGui::Button(tracce[0].parts[0].notes[note_butt_it].lyric.c_str())){
          custom_lip_id = note_butt_it;
          custom_lips_sel_wdw = true;
        }
        ImGui::PopID();
      }
      ImGui::End();
    }

    if(custom_lips_sel_wdw){
      ImGui::Begin("Change lip animation");
      std::string title = fmt::format("Selected Phoeneme: {}", tracce[0].parts[0].notes[custom_lip_id].lyric);
      ImGui::Text("%s", title.c_str());
      std::string current_mouth = fmt::format("{}", 
                  print_kuchi(tracce[0].parts[0].notes[custom_lip_id].kuchi));
      ImGui::Text("Current mouth_anim assigned: ");
      ImGui::SameLine();
      ImGui::TextColored(blue_colour, "%s", current_mouth.c_str());
      if(ImGui::Button("KUCHI_A")){
        tracce[0].parts[0].notes[custom_lip_id].kuchi = KUCHI_A;
      }
      if(ImGui::Button("KUCHI_E")){
        tracce[0].parts[0].notes[custom_lip_id].kuchi = KUCHI_E;
      }
      if(ImGui::Button("KUCHI_I")){
        tracce[0].parts[0].notes[custom_lip_id].kuchi = KUCHI_I;
      }
      if(ImGui::Button("KUCHI_O")){
        tracce[0].parts[0].notes[custom_lip_id].kuchi = KUCHI_O;
      }
      if(ImGui::Button("KUCHI_U")){
        tracce[0].parts[0].notes[custom_lip_id].kuchi = KUCHI_U;
      }
      if(ImGui::Button("KUCHI_RESET")){
        tracce[0].parts[0].notes[custom_lip_id].kuchi = KUCHI_RESET;
      }
      if(ImGui::Button("Close")){
        custom_lips_sel_wdw = false;
        custom_lip_id = NULL;
      }
      ImGui::End();
    }


    // 3. Show another simple window.
    
    if (show_another_window) {
      ImGui::Begin("Damiano", &show_another_window);

      ImGui::Text("Vespasiano dei rasputin dice:");
      ImGui::Image((void *)textures[13].texture, ImVec2(textures[13].img_x, textures[13].img_y));

      if (ImGui::ImageButton((void*)textures[12].texture, ImVec2(textures[12].img_x,
      textures[12].img_y))){ show_another_window = false;
      }


      ImGui::End();
      if (coglioni) {
        PlaySound(TEXT("./bocche/viconviene.wav"), NULL, SND_FILENAME |
        SND_ASYNC); coglioni = false;
        }
      }else {
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