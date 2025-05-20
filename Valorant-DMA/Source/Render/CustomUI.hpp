#pragma once
#include <vector>
#include <random>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

namespace UI
{
   

   

    std::string GetKeyNameFromHex(int hexKey) {
        char keyName[256] = { 0 };

        switch (hexKey) {
        case VK_LBUTTON: return "Left Mouse";
        case VK_RBUTTON: return "Right Mouse";
        case VK_MBUTTON: return "Middle Mouse";
        case VK_XBUTTON1: return "Mouse Button 4";
        case VK_XBUTTON2: return "Mouse Button 5";
        default:
            return GetKeyNameTextA(MapVirtualKeyA(hexKey, MAPVK_VK_TO_VSC) << 16, keyName, sizeof(keyName)) ? std::string(keyName) : "Unknown";
        }
    }

    void KeyButton(std::string name, const std::string& id, int width, int& aimKey) {
        static std::map<std::string, bool> listeningMap;

        bool& isListeningForKey = listeningMap[id];

        if (isListeningForKey) {
            for (int i = 1; i <= 255; i++) {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_ESCAPE) {
                        aimKey = 0; 
                    }
                    else if (aimKey != i) {  
                        aimKey = i;  
                    }
                    isListeningForKey = false;
                    break;
                }
            }
        }

        std::string buttonText = isListeningForKey ? "Press any key..." : (aimKey == 0 ? name : GetKeyNameFromHex(aimKey));

        ImGui::PushID(id.c_str());  
        ImGui::SetNextItemWidth(width);
        if (ImGui::Button(buttonText.c_str(), ImVec2(width, 0))) {
            isListeningForKey = true;  
        }
        ImGui::PopID();  
    }
}