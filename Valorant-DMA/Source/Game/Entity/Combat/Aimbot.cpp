#include <Pch.h>
#include "Aimbot.h"

// by h3n - unkdev


void Aimbot::CursorTo(float x, float y) {
	Vector2 center(settings::width / 2, settings::height / 2);
	Vector3 target(0, 0, 0);

	if (x != 0) {
		if (x > center.x) {
			target.x = -(center.x - x);
			target.x *= (settings::aimbot::smoothness / 10);
			if (target.x + center.x > center.x * 2)
				target.x = 0;
		}

		if (x < center.x) {
			target.x = x - center.x;
			target.x *= (settings::aimbot::smoothness / 10);
			if (target.x + center.x < 0)
				target.x = 0;
		}
	}
	if (y != 0) {
		if (y > center.y) {
			target.y = -(center.y - y);
			target.y *= settings::aimbot::smoothness / 10;
			if (target.y + center.y > center.y * 2)
				target.y = 0;
		}

		if (y < center.y) {
			target.y = y - center.y;
			target.y *= settings::aimbot::smoothness / 10;
			if (target.y + center.y < 0)
				target.y = 0;
		}
	}

	if (settings::kmbox::kmboxB) {
		kmBox::sendMove(target.x, target.y);
	}

	if (settings::kmbox::kmboxNet) {
		kmNet_mouse_move(target.x, target.y);
	}
}


void Aimbot::AimbotThread() {

	while (aimbot_running) {

		Vector2 target2d;
		{
			std::lock_guard<std::mutex> lock(target_mutex);
			target2d = target_position2d;
		}

		CursorTo(target2d.x, target2d.y);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Aimbot::StartAimbotThread() {
	if (!aimbot_running) {
		aimbot_running = true;
		std::thread aimbot_thread(&Aimbot::AimbotThread, aim);
		aimbot_thread.detach();
	}
}

void Aimbot::StopAimbotThread() {
	aimbot_running = false;
}