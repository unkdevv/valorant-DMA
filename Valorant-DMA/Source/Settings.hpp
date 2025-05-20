#pragma once
#include <Windows.h>
#include <json.hpp>

using json = nlohmann::json;
namespace settings
{
	inline int width = 1920;
	inline int height = 1080;

	namespace aimbot
	{
		inline bool enable;
		inline float smoothness = 5;
		inline float deadzone = 0;
		inline float fov = 100;
		inline bool showfov = false;
		inline int aimkey1 = 0;
		inline int aimkey2 = 0;
	}

	namespace kmbox
	{
		inline bool kmboxB = false;
		inline bool kmboxNet = false;
 
		inline int baudrate = 115200;
		inline static char ip[128] = "";
		inline static char port[128] = "";
		inline static char uuid[128] = "";
	}

	namespace visuals
	{
		inline bool bBox;
		inline bool bLines;
		inline bool bHealth;

	}

	namespace misc
	{
		inline bool bTransparent = false;
        inline int current_fps;
		inline bool bCustomColor = false;
		inline float accentColor[4] = { 0.28f, 0.56f, 1.00f, 1.00f };
		inline float textColor[4] = { 0.95f, 0.96f, 0.98f, 1.00f };
		inline float bgColor[4] = { 0.10f, 0.11f, 0.12f, 0.90f };
	}


    inline void saveSettings(const std::string& filename)
    {
        json j;

        j["width"] = width;
        j["height"] = height;

        j["aimbot"]["enable"] = aimbot::enable;
        j["aimbot"]["smoothness"] = aimbot::smoothness;
        j["aimbot"]["deadzone"] = aimbot::deadzone;
        j["aimbot"]["fov"] = aimbot::fov;
        j["aimbot"]["showfov"] = aimbot::showfov;
        j["aimbot"]["aimkey1"] = aimbot::aimkey1;
        j["aimbot"]["aimkey2"] = aimbot::aimkey2;

        j["kmbox"]["kmboxB"] = kmbox::kmboxB;
        j["kmbox"]["kmboxNet"] = kmbox::kmboxNet;
        j["kmbox"]["baudrate"] = kmbox::baudrate;
        j["kmbox"]["ip"] = kmbox::ip;
        j["kmbox"]["port"] = kmbox::port;
        j["kmbox"]["uuid"] = kmbox::uuid;

        j["visuals"]["bBox"] = visuals::bBox;
        j["visuals"]["bLines"] = visuals::bLines;
        j["visuals"]["bHealth"] = visuals::bHealth;

        j["misc"]["bTransparent"] = misc::bTransparent;

        std::ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4); 
            file.close();
        }
    }

    inline void loadSettings(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        json j;
        file >> j;
        file.close();

        if (j.contains("width")) width = j["width"];
        if (j.contains("height")) height = j["height"];

        if (j.contains("aimbot"))
        {
            aimbot::enable = j["aimbot"].value("enable", false);
            aimbot::smoothness = j["aimbot"].value("smoothness", 5.0f);
            aimbot::deadzone = j["aimbot"].value("deadzone", 0.0f);
            aimbot::fov = j["aimbot"].value("fov", 100.0f);
            aimbot::showfov = j["aimbot"].value("showfov", false);
            aimbot::aimkey1 = j["aimbot"].value("aimkey1", 0);
            aimbot::aimkey2 = j["aimbot"].value("aimkey2", 0);
        }

        if (j.contains("kmbox"))
        {
            kmbox::baudrate = j["kmbox"].value("baudrate", 115200);
            strncpy(kmbox::ip, j["kmbox"]["ip"].get<std::string>().c_str(), sizeof(kmbox::ip));
            strncpy(kmbox::port, j["kmbox"]["port"].get<std::string>().c_str(), sizeof(kmbox::port));
            strncpy(kmbox::uuid, j["kmbox"]["uuid"].get<std::string>().c_str(), sizeof(kmbox::uuid));
        }

        if (j.contains("visuals"))
        {
            visuals::bBox = j["visuals"].value("bBox", false);
            visuals::bLines = j["visuals"].value("bLines", false);
            visuals::bHealth = j["visuals"].value("bHealth", false);
        }

        if (j.contains("misc"))
        {
            misc::bTransparent = j["misc"].value("bTransparent", false);
        }
    }
}