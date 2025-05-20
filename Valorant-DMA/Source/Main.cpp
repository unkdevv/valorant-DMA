#include <Pch.h>
#include <WinDef.h>
#include "Game/Core.h"
#include "Game/Entity/Cache.h"

bool debug = true; // global debug bool

/*
* Made by -- h3n unkdev
* this is trash code! 
*/

int main()
{
    SetConsoleTitleA("Valorant Free DMA - Jouh");
    settings::loadSettings("config.txt");

    if (!mem.Init("Valorant-Win64-Shipping.exe", true, false)) {
        LOG("Failed to init dma\n");
        Sleep(1000);
       // exit(0);
    }

    GameCache::BaseAddress = mem.GetBaseDaddy("Valorant-Win64-Shipping.exe");
    if (!GameCache::BaseAddress) {
        LOG("Failed to base address\n");
        Sleep(1000);
       // exit(0);
    }

    if (!init())
    {
        LOG("Failed to init Gui\n");
        Sleep(1000);
        exit(0);
    }

    if (!mem.GetKeyboard()->InitKeyboard()) {
        LOG("Failed to init keyboard\n");
        Sleep(1000);
        // exit(0);
    }

    bool kmBoxSuccess = kmBox::init(settings::kmbox::baudrate);
    bool kmNetSuccess = kmNet_init(settings::kmbox::ip, settings::kmbox::port, settings::kmbox::uuid) == 0;

    if (kmNetSuccess) {
        settings::kmbox::kmboxB = false;
        settings::kmbox::kmboxNet = true;
        std::cout << "Found KmBox .NET" << std::endl;
    }

    if (kmBoxSuccess) {
        settings::kmbox::kmboxB = true;
        settings::kmbox::kmboxNet = false;
        std::cout << "Found kmBox B+ " << std::endl;
    }

    std::thread(&Core::InitGame, InitCore, debug).detach();

    render_loop();
    return 0;
}