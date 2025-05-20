#pragma once

#include <Windows.h>
#include <iostream>
#include "conio.h"
#include <setupapi.h>
#include <devguid.h>

#pragma comment(lib, "setupapi.lib")

namespace kmBox
{
    inline  HANDLE hSerial;

    bool init(DWORD baudrate);
    bool sendMove(int X, int Y);
    bool kmclick ();
    bool sendkey(const std::string& key);
    bool Restart(DWORD baudrate);

    inline std::string FindPort(const std::string& targetDescription) {
        HDEVINFO hDevInfo = SetupDiGetClassDevsA(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
        if (hDevInfo == INVALID_HANDLE_VALUE) return "";

        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceInfoData); ++i) {
            char buf[512];
            DWORD nSize = 0;

            if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)buf, sizeof(buf), &nSize) && nSize > 0) {
                buf[nSize] = '\0';
                std::string deviceDescription = buf;

                size_t comPos = deviceDescription.find("COM");
                size_t endPos = deviceDescription.find(")", comPos);

                if (comPos != std::string::npos && endPos != std::string::npos && deviceDescription.find(targetDescription) != std::string::npos) {
                    SetupDiDestroyDeviceInfoList(hDevInfo);
                    return deviceDescription.substr(comPos, endPos - comPos);
                }
            }
        }

        SetupDiDestroyDeviceInfoList(hDevInfo);
        return "";
    }


    inline bool OpenPort(HANDLE& hSerial, const char* portName, DWORD baudRate) {
        hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (hSerial == INVALID_HANDLE_VALUE) return false;

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        if (!GetCommState(hSerial, &dcbSerialParams)) {
            CloseHandle(hSerial);
            return false;
        }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            CloseHandle(hSerial);
            return false;
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 5;
        timeouts.ReadTotalTimeoutConstant = 5;
        timeouts.ReadTotalTimeoutMultiplier = 5;
        timeouts.WriteTotalTimeoutConstant = 5;
        timeouts.WriteTotalTimeoutMultiplier = 5;

        if (!SetCommTimeouts(hSerial, &timeouts)) {
            std::cerr << "Error setting timeouts!" << std::endl;
            CloseHandle(hSerial);
            return false;
        }

        return true;
    }

    inline bool SendCommand(HANDLE hSerial, const std::string& command) {
        DWORD bytesWritten;
        if (!WriteFile(hSerial, command.c_str(), command.length(), &bytesWritten, NULL)) {
            std::cerr << "Failed to write to serial port!" << std::endl;
            return false;
        }
        return true;
    }

    inline bool Restart(DWORD baudRat) {
        // Close the current handle if it's valid
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
        }

        std::string portName = FindPort("USB-SERIAL CH340");
        if (portName.empty()) {
            return false;
        }

        std::string fullPortName = "\\\\.\\" + portName;

        if (!OpenPort(hSerial, fullPortName.c_str(), baudRat)) {
            return false;
        }

        return true;
    }
}

