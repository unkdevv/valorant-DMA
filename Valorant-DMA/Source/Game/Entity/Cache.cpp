#include <Pch.h>
#include "Cache.h"

#include "Combat/Aimbot.h"
#include "../Render/RenderMenu.hpp"
#include "../sdk.hpp"

void Cache::EntityLoop()
{
	auto scatter_handle = mem.CreateScatterHandle();

	mem.AddScatterReadRequest(scatter_handle, GameCache::PlayerCameraManager + offsets::CameraCache, &GameCache::LocalCamera, sizeof(Camera));
	mem.AddScatterReadRequest(scatter_handle, GameCache::PersistentLevel + offsets::PlayerArray, &GameCache::PlayerArray, sizeof(uintptr_t));
	mem.AddScatterReadRequest(scatter_handle, GameCache::PersistentLevel + 0xA8, &GameCache::PlayerCount, sizeof(int));
	mem.ExecuteReadScatter(scatter_handle);

	if (GameCache::PlayerCount <= 0) return;

	std::vector<uintptr_t> actor_array(GameCache::PlayerCount);
	mem.Read(GameCache::PlayerArray, actor_array.data(), GameCache::PlayerCount * sizeof(uintptr_t));

	std::vector<PlayerData> players_data(GameCache::PlayerCount);

	for (int i = 0; i < GameCache::PlayerCount; i++) {
		uintptr_t actor = actor_array[i];
		if (CheckValidVA(actor)) {
			mem.AddScatterReadRequest(scatter_handle, actor + offsets::PawnPrivate, &players_data[i].pawnpriv, sizeof(uintptr_t));
			mem.AddScatterReadRequest(scatter_handle, actor + offsets::Mesh, &players_data[i].Mesh, sizeof(uintptr_t));
			mem.AddScatterReadRequest(scatter_handle, actor + offsets::damageHandler, &players_data[i].DamageHandler, sizeof(uintptr_t));
			mem.AddScatterReadRequest(scatter_handle, actor + offsets::Dormant, &players_data[i].Dormant, sizeof(BYTE));
		}
	}
	mem.ExecuteReadScatter(scatter_handle);

	for (int i = 0; i < GameCache::PlayerCount; i++) {
		if (CheckValidVA(players_data[i].Mesh) && players_data[i].pawnpriv != GameCache::LocalPawn && (!players_data[i].Dormant || players_data[i].Dormant == 1)) {
			mem.AddScatterReadRequest(scatter_handle, players_data[i].Mesh + offsets::BoneArray, &players_data[i].BoneArray, sizeof(uintptr_t));
			mem.AddScatterReadRequest(scatter_handle, players_data[i].Mesh + offsets::BoneArray + 0x10, &players_data[i].BoneArrayCache, sizeof(uintptr_t));
			mem.AddScatterReadRequest(scatter_handle, players_data[i].Mesh + offsets::ComponetToWorld, &players_data[i].Comp2World, sizeof(FTransform));
			mem.AddScatterReadRequest(scatter_handle, players_data[i].DamageHandler + offsets::Health, &players_data[i].Health, sizeof(float));
			mem.AddScatterReadRequest(scatter_handle, players_data[i].Mesh + offsets::LastSubmitTime, &players_data[i].LastSubmit, sizeof(float));
			mem.AddScatterReadRequest(scatter_handle, players_data[i].Mesh + offsets::LastRenderTimeOnScreen, &players_data[i].LastRender, sizeof(float));
		}
		
	}
	mem.ExecuteReadScatter(scatter_handle);

	for (int i = 0; i < GameCache::PlayerCount; i++) {
		if (!players_data[i].BoneArray) {
			players_data[i].BoneArray = players_data[i].BoneArrayCache;
		}

		if (players_data[i].BoneArray) {
			mem.AddScatterReadRequest(scatter_handle, players_data[i].BoneArray + (8 * 0x30), &players_data[i].FTHead, sizeof(FTransform));
			mem.AddScatterReadRequest(scatter_handle, players_data[i].BoneArray + (0 * 0x30), &players_data[i].FTRoot, sizeof(FTransform));
		}
	}
	mem.ExecuteReadScatter(scatter_handle);

	Vector2 Closest2DHead;
	float min_distance = std::numeric_limits<float>::max();

	for (int i = 0; i < GameCache::PlayerCount; i++) {
		const auto& data = players_data[i];
		if (!data.pawnpriv || data.pawnpriv == GameCache::LocalPawn || !data.Mesh || !data.BoneArray || data.Health == 0) {
			continue;
		}

		bool isvis = data.LastRender + 0.06f >= data.LastSubmit;
		ImColor boxcolor = isvis ? ImColor(0, 255, 0, 255) : ImColor(255, 0, 0, 255);

		if (!(!data.Dormant || data.Dormant == 1)) {

			Vector3 Head3D = doMatrix(data.FTHead, data.Comp2World);
			Vector3 Bottom3D = doMatrix(data.FTRoot, data.Comp2World);

			double distance = GameCache::LocalCamera.Location.Distance(Head3D) / 100.f;

			Vector2 Head2D = w2s(GameCache::LocalCamera, Head3D);
			Vector2 Bottom2D = w2s(GameCache::LocalCamera, Bottom3D);

			ImVec2 screenCenter = ImVec2(settings::width / 2, settings::height);

			float height = Bottom2D.y - Head2D.y;
			float width = height * 0.5f;

			if (settings::visuals::bBox) DrawBox(Head2D.x - (width / 2), Head2D.y, width, height, boxcolor, 1);
			if (settings::visuals::bLines) DrawLine(Bottom2D, screenCenter, boxcolor);
			if (settings::visuals::bHealth) DrawHealth(Head2D, Bottom2D, data.Health, width, height);

			float dx = Head2D.x - (settings::width / 2.0f);
			float dy = Head2D.y - (settings::height / 2.0f);
			float screen_dist = std::sqrt(dx * dx + dy * dy);

			if (screen_dist <= settings::aimbot::fov && screen_dist < min_distance) {
				min_distance = screen_dist;
				Closest2DHead = Head2D;
			}

			if (mem.GetKeyboard()->IsKeyDown(settings::aimbot::aimkey1) || mem.GetKeyboard()->IsKeyDown(settings::aimbot::aimkey2)) {
				std::lock_guard<std::mutex> lock(target_mutex);
				target_position2d = Closest2DHead;
				aim.StartAimbotThread();
			}
			else
				aim.StopAimbotThread();
		}
	}
	mem.CloseScatterHandle(scatter_handle);
}

WPARAM render_loop() {
	MSG message = { 0 };

	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			ImGuiIO& io = ImGui::GetIO();
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			io.MousePos = ImVec2(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y));
			io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) != 0;

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			render_menu();
			Entity.EntityLoop();
			ImGui::EndFrame();

			float clearColor[4] = { 0.0f, 0.0f, 0.0f, settings::misc::bTransparent ? 0.0f : 1.0f };
			p_device_context->ClearRenderTargetView(p_render_target_view, clearColor);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			HRESULT result = p_swap_chain->Present(0, 0);
			if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
				ImGui_ImplDX11_InvalidateDeviceObjects();
				p_device->Release();
				p_device_context->Release();
				p_swap_chain->Release();
				p_render_target_view->Release();

				if (FAILED(directx_init())) {
					return E_FAIL;
				}
				ImGui_ImplDX11_CreateDeviceObjects();
			}
		}
	}

	return message.wParam;
}

bool init() {
	return SUCCEEDED(directx_init());
}