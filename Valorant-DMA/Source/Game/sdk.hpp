#pragma once
#include <Pch.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "structs.hpp"
#include "../Settings.hpp"

using namespace DirectX;

XMMATRIX MatrixMultiplication(XMMATRIX pM1, XMMATRIX pM2)
{
	XMMATRIX pOut = XMMatrixMultiply(pM1, pM2);
	return pOut;
}

XMMATRIX to_matrix(const Vector3& rot, const Vector3& origin = Vector3(0, 0, 0))
{
	float radpitch = XMConvertToRadians(rot.x);
	float radyaw = XMConvertToRadians(rot.y);
	float radroll = XMConvertToRadians(rot.z);

	float sp = std::sin(radpitch);
	float cp = std::cos(radpitch);
	float sy = std::sin(radyaw);
	float cy = std::cos(radyaw);
	float sr = std::sin(radroll);
	float cr = std::cos(radroll);

	XMMATRIX matrix = XMMATRIX(
		cp * cy, cp * sy, sp, 0.0f,
		sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp, 0.0f,
		-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp, 0.0f,
		origin.x, origin.y, origin.z, 1.0f
	);

	return matrix;
}

Vector2 w2s(const Camera& camera, Vector3 world_location) {
	XMMATRIX temp_matrix = to_matrix(camera.Rotation);

	XMVECTOR vaxisx = temp_matrix.r[0];
	XMVECTOR vaxisy = temp_matrix.r[1];
	XMVECTOR vaxisz = temp_matrix.r[2];

	Vector3 vdelta = world_location - camera.Location;
	XMVECTOR vdelta_vec = XMVectorSet(vdelta.x, vdelta.y, vdelta.z, 0.0f);

	float vtransformed_x = XMVectorGetX(XMVector3Dot(vaxisy, vdelta_vec));
	float vtransformed_y = XMVectorGetX(XMVector3Dot(vaxisz, vdelta_vec));
	float vtransformed_z = XMVectorGetX(XMVector3Dot(vaxisx, vdelta_vec));

	if (vtransformed_z < 1.0f) vtransformed_z = 1.0f;

	int screen_center_x = static_cast<int>(settings::width) / 2;
	int screen_center_y = static_cast<int>(settings::height) / 2;

	float fov = camera.FieldOfView;
	float tan_fov = tanf(fov * static_cast<float>(XM_PI) / 360.0f);

	float screen_x = screen_center_x + (vtransformed_x / tan_fov) * screen_center_x / vtransformed_z;
	float screen_y = screen_center_y - (vtransformed_y / tan_fov) * screen_center_x / vtransformed_z;

	return { screen_x, screen_y };
}


Vector3 doMatrix(FTransform bone, FTransform component_to_world)
{
	XMMATRIX boneMatrix = bone.ToMatrixWithScale();
	XMMATRIX componentMatrix = component_to_world.ToMatrixWithScale();

	XMMATRIX resultMatrix = XMMatrixMultiply(boneMatrix, componentMatrix);

	Vector3 result;
	result.x = XMVectorGetX(resultMatrix.r[3]);
	result.y = XMVectorGetY(resultMatrix.r[3]);
	result.z = XMVectorGetZ(resultMatrix.r[3]);

	return result;
}

bool CheckValidVA(uintptr_t address) {
	if (address != 0 && address >= 0x10000 && address <= 0x7FFFFFFFFFFF) {
		return true;
	}
	return false;
}
