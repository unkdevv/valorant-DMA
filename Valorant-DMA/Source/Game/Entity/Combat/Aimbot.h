#pragma once
#include "../../structs.hpp"
#include "../../../Settings.hpp"
#include "../../Kmbox/manager.hpp"

inline std::atomic<bool> aimbot_running(false);
inline Vector2 target_position2d = { 0, 0 };
inline std::mutex target_mutex;

class Aimbot
{
public:
	void CursorTo(float x, float y);
	void AimbotThread();
	void StartAimbotThread();
	void StopAimbotThread();
};

inline Aimbot aim;
