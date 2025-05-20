#pragma once 
#include <Pch.h>
#include <DirectXMath.h>
#include <xmmintrin.h> 

using namespace DirectX; 
#define M_PI 3.14159265358979323846

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(float _x, float _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	float x;
	float y;
};
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}

	Vector3 operator/(float number) const {
		return Vector3(x / number, y / number, z / number);
	}
};

struct FQuat {
    double x, y, z, w;
};

typedef struct MMatrix {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};

struct alignas(16) FTransform
{
	XMVECTOR rot;
	XMVECTOR translation;
	XMVECTOR scale;

	XMMATRIX ToMatrixWithScale()
	{
		XMMATRIX m;
		float x = XMVectorGetX(rot);
		float y = XMVectorGetY(rot);
		float z = XMVectorGetZ(rot);
		float w = XMVectorGetW(rot);

		float sx = XMVectorGetX(scale);
		float sy = XMVectorGetY(scale);
		float sz = XMVectorGetZ(scale);

		float tx = XMVectorGetX(translation);
		float ty = XMVectorGetY(translation);
		float tz = XMVectorGetZ(translation);

		float x2 = x + x;
		float y2 = y + y;
		float z2 = z + z;
		float xx2 = x * x2;
		float yy2 = y * y2;
		float zz2 = z * z2;
		float yz2 = y * z2;
		float wx2 = w * x2;
		float xy2 = x * y2;
		float wz2 = w * z2;
		float xz2 = x * z2;
		float wy2 = w * y2;

		m.r[0] = XMVectorSet((1.0f - (yy2 + zz2)) * sx, (xy2 + wz2) * sx, (xz2 - wy2) * sx, 0.0f);
		m.r[1] = XMVectorSet((xy2 - wz2) * sy, (1.0f - (xx2 + zz2)) * sy, (yz2 + wx2) * sy, 0.0f);
		m.r[2] = XMVectorSet((xz2 + wy2) * sz, (yz2 - wx2) * sz, (1.0f - (xx2 + yy2)) * sz, 0.0f);
		m.r[3] = XMVectorSet(tx, ty, tz, 1.0f);

		return m;
	}

	bool IsValid() const
	{
		float x = XMVectorGetX(rot);
		float y = XMVectorGetY(rot);
		float z = XMVectorGetZ(rot);
		float w = XMVectorGetW(rot);

		float lengthSq = x * x + y * y + z * z + w * w;
		if (fabsf(lengthSq - 1.0f) > 0.01f) {
			return false; 
		}

		float sx = XMVectorGetX(scale);
		float sy = XMVectorGetY(scale);
		float sz = XMVectorGetZ(scale);
		if (sx <= 0.0f || sy <= 0.0f || sz <= 0.0f) {
			return false; 
		}

		float tx = XMVectorGetX(translation);
		float ty = XMVectorGetY(translation);
		float tz = XMVectorGetZ(translation);
		if (isnan(tx) || isnan(ty) || isnan(tz)) {
			return false; 
		}

		return true;
	}
};

struct Camera
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
};


namespace offsets
{
	inline uintptr_t UWorld;
	inline uintptr_t GameInstance = 0x1a0;
	inline uintptr_t PersistentLevel = 0x38;
	inline uintptr_t GameState = 0x1A0;
	inline uintptr_t LocalPlayers = 0x40;
	inline uintptr_t PlayerController = 0x38;
	inline uintptr_t LocalPawn = 0x448;
	inline uintptr_t PawnPrivate = 0x308;
	inline uintptr_t PlayerState = 0x3D8;
	inline uintptr_t TeamId = 0xF8;
	inline uintptr_t LocalUniqueId = 0x38;
	inline uintptr_t PlayerArray = 0xA0;
	inline uintptr_t LocationPtr;
	inline uintptr_t RotationPtr = 0x440;
	inline uintptr_t CameraManager = 0x460;
	inline uintptr_t CameraCache = 0x1f90;
	inline uintptr_t Dormant = 0x100;
	inline uintptr_t Mesh = 0x418;
	inline uintptr_t damageHandler = 0x9e8;
	inline uintptr_t Health = 0x1B0;
	inline uintptr_t ComponetToWorld = 0x250;
	inline uintptr_t LastSubmitTime = 0x380;
	inline uintptr_t LastRenderTimeOnScreen = 0x384;
	inline uintptr_t BoneArray = 0x5c8;
	inline uintptr_t BoneCount = 0x5E0;
	inline uintptr_t Velocity = 0x168;
	inline uintptr_t IsDBNO = 0x93A; // ( BitField Index -> 4 )
	inline uintptr_t isDying = 0x758; // ( BitField Index -> 4 )
	inline uintptr_t TeamComp = 0x610;
}

namespace GameCache
{
	inline uintptr_t GuardedRegion;
	inline uintptr_t BaseAddress;
	inline uintptr_t UWorld;
	inline uintptr_t GameInstance;
	inline uintptr_t GameState;
	inline uintptr_t LocalPlayers;
	inline uintptr_t PlayerController;
	inline uintptr_t LocalPawn;
	inline uintptr_t PlayerState;
	inline uintptr_t TeamId;
	inline uintptr_t LocationPtr;
	inline uintptr_t RotationPtr;
	inline uintptr_t PersistentLevel;
	inline uintptr_t PlayerCameraManager;
	inline uintptr_t PlayerArray;
	inline Camera LocalCamera;
	inline int PlayerCount;
}