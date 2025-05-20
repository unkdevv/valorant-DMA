#pragma once
#include "../structs.hpp"
#include "../Kmbox/manager.hpp"
#include "../Settings.hpp"
#include <imgui/imgui.h>

bool init();
WPARAM render_loop();

struct PlayerData {
	uintptr_t pawnpriv;
	uintptr_t Mesh;
	uintptr_t BoneArray;
	uintptr_t BoneArrayCache;
	uintptr_t DamageHandler;
	float Health;
	FTransform Comp2World;
	FTransform FTHead;
	FTransform FTRoot;
	BYTE Dormant;
	float LastSubmit;
	float LastRender;
};

class Cache
{
public:
	void UpdateEntity(bool debug);
	void EntityLoop();

	void DrawBox(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetForegroundDrawList()->AddRect(
			ImVec2(x, y), 
			ImVec2(x + w, y + h), 
			color, 3.0f, 
			0, 
			thickness);
	}

	void DrawLine(Vector2 bone, ImVec2 screenCenter, ImColor color)
	{
		ImGui::GetForegroundDrawList()->AddLine(
			screenCenter,
			ImVec2(bone.x, bone.y),
			color,
			1.0f
		);
	}

	void DrawHealth(Vector2 Head2D, Vector2 Bottom2D, float health, int width, int height)
	{
		ImVec2 topRight(Head2D.x + width / 2, Head2D.y);
		ImVec2 bottomRight(Head2D.x + width / 2, Bottom2D.y);
		float healthPercentage = health / 100.0f;
		float healthBarWidth = 5.0f;
		float healthBarHeight = height * healthPercentage;

		ImVec2 healthBarTopLeft(topRight.x + 2, bottomRight.y);
		ImVec2 healthBarBottomRight(healthBarTopLeft.x + healthBarWidth, healthBarTopLeft.y - healthBarHeight);

		int r = static_cast<int>((1.0f - healthPercentage) * 255);
		int g = static_cast<int>(healthPercentage * 255);

		ImColor healthColor(r, g, 0, 255);

		ImGui::GetForegroundDrawList()->AddRectFilled(
			healthBarTopLeft,
			ImVec2(healthBarBottomRight.x, bottomRight.y),
			ImColor(255, 0, 0, 255)
		);

		ImGui::GetForegroundDrawList()->AddRectFilled(
			healthBarTopLeft,
			healthBarBottomRight,
			healthColor
		);
	}
};

inline Cache Entity;