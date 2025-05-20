#pragma once
#include <Pch.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "CustomUI.hpp"
#include "Config.hpp"
#include "Font.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

ID3D11Device* p_device = nullptr;
ID3D11DeviceContext* p_device_context = nullptr;
IDXGISwapChain* p_swap_chain = nullptr;
ID3D11RenderTargetView* p_render_target_view = nullptr;
MSG messager = { nullptr };
HWND my_wnd = nullptr;

// Constants
const int WINDOW_WIDTH = settings::width;
const int WINDOW_HEIGHT = settings::height;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (p_device != NULL && wParam != SIZE_MINIMIZED)
        {
            if (p_render_target_view) { p_render_target_view->Release(); p_render_target_view = nullptr; }
            p_swap_chain->ResizeBuffers(0, WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_UNKNOWN, 0);
            ID3D11Texture2D* p_back_buffer;
            p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_back_buffer);
            p_device->CreateRenderTargetView(p_back_buffer, NULL, &p_render_target_view);
            p_back_buffer->Release();
            p_device_context->OMSetRenderTargets(1, &p_render_target_view, NULL);
            D3D11_VIEWPORT vp;
            vp.Width = (FLOAT)WINDOW_WIDTH;
            vp.Height = (FLOAT)WINDOW_HEIGHT;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            p_device_context->RSSetViewports(1, &vp);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HRESULT directx_init()
{
    // Register the window class
    WNDCLASSEXA wcex = {
        sizeof(WNDCLASSEXA),
        CS_CLASSDC,
        WindowProc, // Use custom window procedure
        0,
        0,
        GetModuleHandle(NULL),
        LoadIcon(0, IDI_APPLICATION),
        LoadCursor(0, IDC_ARROW),
        0,
        0,
        "OverlayWindowClass",
        LoadIcon(0, IDI_APPLICATION)
    };
    RegisterClassExA(&wcex);

    // Create the window
    my_wnd = CreateWindowExA(
        0,
        "OverlayWindowClass",
        "Valorant Free DMA - Jouh",
        WS_POPUP | WS_VISIBLE,
        0, 0,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr, nullptr,
        wcex.hInstance, nullptr
    );

    // Set the window style to layered
    SetWindowLong(my_wnd, GWL_EXSTYLE, GetWindowLong(my_wnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(my_wnd, &margin);
    ShowWindow(my_wnd, SW_SHOW);
    UpdateWindow(my_wnd);

    // Initialize Direct3D 11
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = WINDOW_WIDTH;
    sd.BufferDesc.Height = WINDOW_HEIGHT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = my_wnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &p_swap_chain, &p_device, NULL, &p_device_context)))
    {
        return E_FAIL;
    }

    ID3D11Texture2D* p_back_buffer;
    p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_back_buffer);
    p_device->CreateRenderTargetView(p_back_buffer, NULL, &p_render_target_view);
    p_back_buffer->Release();
    p_device_context->OMSetRenderTargets(1, &p_render_target_view, NULL);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)WINDOW_WIDTH;
    vp.Height = (FLOAT)WINDOW_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    p_device_context->RSSetViewports(1, &vp);

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(my_wnd);
    ImGui_ImplDX11_Init(p_device, p_device_context);
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(font, sizeof(font), 14.0f);
    ImGui::GetIO().Fonts->Build();
    style();

    return S_OK;
}

static int current_tab = -1;
static bool show_menu = true;

void case0() {
    ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable);

    if (settings::aimbot::enable) {

        UI::KeyButton("Hold Key 1", "1", 120, settings::aimbot::aimkey1);
        ImGui::SameLine();
        UI::KeyButton("Hold Key 2", "2", 120, settings::aimbot::aimkey2);
        ImGui::SetNextItemWidth(250);
        ImGui::SliderFloat("Speed", &settings::aimbot::smoothness, 1.0f, 20.0f, "%.0f");
        ImGui::SetNextItemWidth(250);
        ImGui::SliderFloat("##Fov", &settings::aimbot::fov, 1.0f, 300.0f, "%.0f");
        ImGui::SameLine();
        ImGui::Checkbox("FOV", &settings::aimbot::showfov);
    }
}


void case1()
{
    ImGui::Checkbox("Enable Box", &settings::visuals::bBox);
    ImGui::Checkbox("Enable Lines", &settings::visuals::bLines);
    ImGui::Checkbox("Enable Heatlh", &settings::visuals::bHealth);
}

void case2()
{
    ImGui::Text("Insert to open/close menu");
    if (ImGui::Button("Save Config", { 120, 20 })) settings::saveSettings("config.txt");
    ImGui::Checkbox("Transparent", &settings::misc::bTransparent);

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 130);
    if (ImGui::Button("Close", { 120, 20 })) exit(0);

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y);
    ImGui::SetCursorPosX(10);
    static bool select = false;
    if (ImGui::Button("Select kmbox type", { 140, 20 })) select = true;
    ImGui::SameLine();
    static bool showb = false;

    if (select) {
        ImGui::OpenPopup("Kmbox Settings");
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - 400) / 2, (io.DisplaySize.y - 300) / 2));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        if (ImGui::BeginPopupModal("Kmbox Settings", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Kmbox should connect on load.");
            ImGui::Dummy(ImVec2(0.0f, 7.5f));
            if (ImGui::Button("Kmbox B+", { 200, 20 })) showb = true;
            if (showb) {
                ImGui::SetNextItemWidth(130);
                ImGui::InputScalar("BaudRate", ImGuiDataType_S32, &settings::kmbox::baudrate, nullptr, nullptr, "%d", ImGuiInputTextFlags_CharsDecimal);
                if (ImGui::Button("Connect to B+", { 200, 20 })) {
                    if (!kmBox::init(settings::kmbox::baudrate)) {
                        std::cout << "Failed to connect to KmBox B+" << std::endl;
                    }
                    else {
                        settings::kmbox::kmboxB = true;
                    }
                }
            }
            static bool shownet = false;
            ImGui::Dummy(ImVec2(0.0f, 7.5f));
            if (ImGui::Button("Kmbox .NET", { 200, 20 })) shownet = true;
            if (shownet) {
                ImGui::Dummy(ImVec2(0.0f, 7.5f));
                ImGui::SetNextItemWidth(170);
                ImGui::InputText("Ip", settings::kmbox::ip, IM_ARRAYSIZE(settings::kmbox::ip));
                ImGui::Dummy(ImVec2(0.0f, 7.5f));
                ImGui::SetNextItemWidth(170);
                ImGui::InputText("Port", settings::kmbox::port, IM_ARRAYSIZE(settings::kmbox::port));
                ImGui::Dummy(ImVec2(0.0f, 7.5f));
                ImGui::SetNextItemWidth(170);
                ImGui::InputText("UUID", settings::kmbox::uuid, IM_ARRAYSIZE(settings::kmbox::uuid));
                ImGui::Dummy(ImVec2(0.0f, 7.5f));
                if (ImGui::Button("Connect to .NET", { 200, 20 })) {
                    if (kmNet_init(settings::kmbox::ip, settings::kmbox::port, settings::kmbox::uuid) == 0) {
                        settings::kmbox::kmboxNet = true;
                    }
                    else {
                        std::cout << "Failed to connect to KmBox .NET" << std::endl;
                    }
                }
            }
            ImGui::Dummy(ImVec2(0.0f, 7.5f));
            if (ImGui::Button("Close", { 200, 20 })) {
                select = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
    }
}


void render_menu()
{
    ImGuiIO& io = ImGui::GetIO();

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        show_menu = !show_menu;
    }

    if (settings::aimbot::showfov) {
        ImGui::GetForegroundDrawList()->AddCircle(
            ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2),
            settings::aimbot::fov,
            ImColor(1.0f, 1.0f, 1.0f, 1.0f),
            100,
            1.0f
        );
    }
    

    if (show_menu)
    {
        ImGui::SetNextWindowSize({ 620, 350 });
        ImGui::Begin("FREE", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button("Aimbot", { 195, 20 })) current_tab = 0;
        ImGui::SameLine();
        if (ImGui::Button("Visuals", { 195, 20 })) current_tab = 1;
        ImGui::SameLine();
        if (ImGui::Button("Misc", { 195, 20 })) current_tab = 2;

        switch (current_tab)
        {
        case 0:
            case0();
            break;

        case 1:
            case1();
            break;

        case 2:
            case2();
            break;
        }
        ImGui::End();
    }
    ImGui::EndFrame();
}


