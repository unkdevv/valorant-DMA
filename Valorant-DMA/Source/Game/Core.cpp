#include <Pch.h>
#include "Core.h"
#include "Entity/Cache.h"

#define HIDWORD(x) ((unsigned int)(((x) >> 32) & 0xFFFFFFFF))
#define __ROL8__(x, r) (((x) << (r)) | ((x) >> (64 - (r))))
#define __ROR8__(x, r) (((x) >> (r)) | ((x) << (64 - (r))))

__forceinline __int64 Decrypt_UWorld(const uint32_t key, const uintptr_t* state)
{

    unsigned __int64 v19; // r11
    unsigned __int64 v20; // r8
    unsigned __int64 v21; // r9
    unsigned int v22; // er10
    unsigned __int64 v23; // rcx
    unsigned __int64 v24; // rdx
    unsigned __int64 v25; // rcx
    int v26; // ebx
    unsigned int v27; // ecx
    __int64 v28; // rax
    unsigned __int64 v29; // r8
    unsigned __int64 v30; // r8
    unsigned __int64 v31; // rcx
    unsigned __int64 v32; // rdx
    unsigned __int64 v33; // rcx

    v19 = 2685821657736338717i64
        * ((unsigned int)key ^ (unsigned int)(key << 25) ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12))
        % 7;
    v20 = state[v19];
    v21 = (2685821657736338717i64
        * ((unsigned int)key ^ (unsigned int)(key << 25) ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12))) >> 32;
    v22 = (unsigned int)v19 % 7;
    if (!((unsigned int)v19 % 7))
    {
        v23 = (2 * (v20 - (unsigned int)(v21 - 1))) ^ ((2 * (v20 - (unsigned int)(v21 - 1))) ^ ((v20
            - (unsigned int)(v21 - 1)) >> 1)) & 0x5555555555555555i64;
        v24 = (4 * v23) ^ ((4 * v23) ^ (v23 >> 2)) & 0x3333333333333333i64;
        v25 = (16 * v24) ^ ((16 * v24) ^ (v24 >> 4)) & 0xF0F0F0F0F0F0F0Fi64;
        v20 = __ROL8__((v25 << 8) ^ ((v25 << 8) ^ (v25 >> 8)) & 0xFF00FF00FF00FFi64, 32);
    LABEL_26:
        v26 = 2 * v19;
        goto LABEL_27;
    }
    if (v22 != 1)
        goto LABEL_26;
    v26 = 2 * v19;
    v20 = __ROL8__(v20 - (unsigned int)(2 * v19 + v21), (unsigned __int8)(((int)v21 + (int)v19) % 0x3Fu) + 1);
LABEL_27:
    v27 = v26 + v21;
    if (v22 == 2)
        v20 = ~(v20 - v27);
    switch (v22)
    {
    case 3u:
        v28 = 2 * ((2 * v20) ^ ((2 * v20) ^ (v20 >> 1)) & 0x5555555555555555i64);
        v20 = v28 ^ (v28 ^ (((2 * v20) ^ ((2 * v20) ^ (v20 >> 1)) & 0x5555555555555555i64) >> 1)) & 0x5555555555555555i64;
        break;
    case 4u:
        v29 = __ROR8__(v20, (unsigned __int8)(v27 % 0x3F) + 1);
        v20 = (2 * v29) ^ ((2 * v29) ^ (v29 >> 1)) & 0x5555555555555555i64;
        break;
    case 5u:
        v30 = __ROR8__(v20, (unsigned __int8)(v27 % 0x3F) + 1);
        v31 = (2 * v30) ^ ((2 * v30) ^ (v30 >> 1)) & 0x5555555555555555i64;
        v32 = (4 * v31) ^ ((4 * v31) ^ (v31 >> 2)) & 0x3333333333333333i64;
        v33 = (16 * v32) ^ ((16 * v32) ^ (v32 >> 4)) & 0xF0F0F0F0F0F0F0Fi64;
        v20 = __ROL8__((v33 << 8) ^ ((v33 << 8) ^ (v33 >> 8)) & 0xFF00FF00FF00FFi64, 32);
        break;
    case 6u:
        v20 = ~v20 - (unsigned int)(v21 + v19);
        break;
    }
    return v20 ^ (unsigned int)key;
}

uintptr_t Stateaddy = 0xAF30580,
KeyAddy = Stateaddy + 0x38;

inline auto decryptWorld(uintptr_t base_address) -> uintptr_t {

    auto key = mem.Read<uintptr_t>(base_address + Stateaddy + KeyAddy);
#pragma pack(push, 1)
    struct State {
        uint64_t Keys[7];
    };
#pragma pack(pop)
    auto state = mem.Read<State>(base_address + Stateaddy);

    auto decrypt = Decrypt_UWorld(key, (uintptr_t*)&state);
    auto decrypt_read = mem.Read<uintptr_t>(decrypt);

    return decrypt_read;
}

auto Core::InitGame(bool debug) -> void
{
	while (true) 
	{
		GameCache::UWorld = decryptWorld(GameCache::BaseAddress);
		if (debug) LOG("UWorld: 0x%p\n", GameCache::UWorld);

		GameCache::GameState = mem.Read<uintptr_t>(GameCache::UWorld + offsets::GameState);
		if (debug) LOG("GameState: 0x%p\n", GameCache::GameState);

		GameCache::GameInstance = mem.Read<uintptr_t>(GameCache::UWorld + offsets::GameInstance);
		if (debug) LOG("GameInstance: 0x%p\n", GameCache::GameInstance);

		GameCache::PersistentLevel = mem.Read<uintptr_t>(GameCache::UWorld + offsets::PersistentLevel);
		if (debug) LOG("PersistentLevel: 0x%p\n", GameCache::PersistentLevel);

		auto templocal = mem.Read<uintptr_t>(GameCache::GameInstance + offsets::LocalPlayers);
		GameCache::LocalPlayers = mem.Read<uintptr_t>(templocal);
		if (debug) LOG("LocalPlayers: 0x%p\n", GameCache::LocalPlayers);

		GameCache::PlayerController = mem.Read<uintptr_t>(GameCache::LocalPlayers + offsets::PlayerController);
		if (debug) LOG("PlayerController: 0x%p\n", GameCache::PlayerController);

		if (GameCache::PlayerController) 
		{
			GameCache::LocalPawn = mem.Read<uintptr_t>(GameCache::PlayerController + offsets::LocalPawn);
			if (debug) LOG("LocalPawn: 0x%p\n", GameCache::LocalPawn);

			GameCache::PlayerCameraManager = mem.Read<uintptr_t>(GameCache::PlayerController + offsets::CameraManager);
			if (debug) LOG("PlayerCameraManager: 0x%p\n", GameCache::PlayerCameraManager);
		}

		GameCache::PlayerCount = mem.Read<uintptr_t>(GameCache::PersistentLevel + 0xA8);
		if (debug) LOG("PlayerCount: 0x%p\n", GameCache::PlayerCount);

		Sleep(2000);
	}
}