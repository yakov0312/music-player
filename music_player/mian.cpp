#include <iostream>
#include <Windows.h>
#include <string>

#pragma comment(lib, "winmm.lib")

void playWAV(const std::wstring& filePath);

int main()
{
    std::wstring path = L"C:\\Users\\Cyber_User\\Desktop\\magshimim_projects\\myProjects\\spotify\\song1.wav";
    playWAV(path);
}

void playWAV(const std::wstring& filePath) {
    std::wstring command = L"open \"" + filePath + L"\" type waveaudio alias audiofile";
    mciSendStringW(command.c_str(), NULL, 0, NULL);
    mciSendStringW(L"play audiofile wait", NULL, 0, NULL);
    mciSendStringW(L"close audiofile", NULL, 0, NULL);
}
