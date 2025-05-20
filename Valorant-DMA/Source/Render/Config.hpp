#pragma once

void style() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;

    auto adjustColor = [](ImVec4 color, float factor) -> ImVec4 {
        color.x = std::clamp(color.x * factor, 0.0f, 1.0f);
        color.y = std::clamp(color.y * factor, 0.0f, 1.0f);
        color.z = std::clamp(color.z * factor, 0.0f, 1.0f);
        return color;
        };

    style.Colors[ImGuiCol_Text] = ImVec4(settings::misc::textColor[0], settings::misc::textColor[1], settings::misc::textColor[2], settings::misc::textColor[3]);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(settings::misc::bgColor[0], settings::misc::bgColor[1], settings::misc::bgColor[2], settings::misc::bgColor[3]);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(settings::misc::bgColor[0], settings::misc::bgColor[1], settings::misc::bgColor[2], settings::misc::bgColor[3]);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = adjustColor(style.Colors[ImGuiCol_FrameBg], 1.2f);
    style.Colors[ImGuiCol_FrameBgActive] = adjustColor(style.Colors[ImGuiCol_FrameBg], 0.8f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = adjustColor(style.Colors[ImGuiCol_TitleBg], 1.2f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

}