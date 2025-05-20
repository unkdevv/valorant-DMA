#pragma once
#include "kmboxNet.h"
#include "kmbox.hpp"
#include <iostream>
#include <string>
#include <sstream>

inline bool kmBox::init(DWORD baudrate) {
    std::string port = kmBox::FindPort("USB-SERIAL CH340");

    if (port.empty()) {
        return false;
    }

    if (!kmBox::OpenPort(hSerial, port.c_str(), baudrate)) {
        return false;
    }
    return true;
}

inline bool kmBox::sendMove(int x, int y) {
    std::stringstream commandStream;
    commandStream << "km.move(" << x << "," << y << ")\r\n";
    std::string command = commandStream.str();

    return SendCommand(hSerial, command);
}

inline bool kmBox::kmclick() {
    std::string command = "km.left(" + std::to_string(1) + ")\r\n";
    std::string command1 = "km.left(" + std::to_string(0) + ")\r\n";
    if (!SendCommand(hSerial, command) || !SendCommand(hSerial, command1)) {
        return false;
    }
    return true;
}

inline bool kmBox::sendkey(const std::string& key) {
    std::string command = "km.press('" + key + "')\r\n";
    if (!SendCommand(hSerial, command)) {
        return false;
    }
    return true;
}

inline bool kmNet_click() {

    if (kmNet_mouse_left(1) == 0) {
        return false;
    }

    Sleep(10);
    if (kmNet_mouse_left(0) == 0) {
        return false;
    }

    return true;
}

inline bool kmNet_send_move(int x, int y) {
    if (kmNet_mouse_move(x, y) != 0)
        return false;

    return true;
}

inline bool kmNet_restart() {
    if (kmNet_reboot() != 0)
        return false;

    return true;
}