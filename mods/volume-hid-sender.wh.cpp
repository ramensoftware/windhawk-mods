// ==WindhawkMod==
// @id              volume-hid-sender
// @name            Volume HID Sendeer
// @description     HID Mod For Volume (Mountain Everest Max Only)
// @version         0.1
// @author          rom4ster
// @github          https://github.com/rom4ster
// @include         explorer.exe
// @include         ShellHost.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lurlmon -lruntimeobject
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Volume HID Sender
This mod allows you to send HID input for the MOUNTAIN EVEREST MAX keyboard everytime 
the volume is changed. This solution is not perfect. The volume will frequently be off by one. 
It is also possible to desync the volume temporarily (until next volume update) by taking
a very long time to decide which volume you want. 

Custom timeouts may be supported in the future

Arbritary HID output will be supported in the future

Requires HID HIDApiTester. 

# Getting started
First get HIDAPITester
[here](https://github.com/todbot/hidapitester/releases).

Next set the HIDApiTesterPath variable in settings to where you downloaded HIDAPITester to

Finally, Profit 


# Special Thanks
https://stackoverflow.com/questions/4559526/how-do-i-tell-if-the-master-volume-is-muted

https://github.com/todbot/hidapitester
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- HIDApiTesterPath: I:\\hidapitest\\hidapitester.exe
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <gdiplus.h>
#include <string.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <Urlmon.h>
#include <windhawk_utils.h>


#define WLog(X) Wh_Log( L"%s", X)
#define WhL(X) Wh_Log(L##X);
#define InitLog(X, ...) do {                                                 \
     char * ptr = Specialized_LogBuffer( (char *) "HELLO: ", (char *) X);    \
     Wh_Log(ptr , __VA_ARGS__);                 \
     free(ptr);                                     \
     } while(0);                      

#define DLLExport __declspec(dllexport)

#define SECONDS * 1000

#define SPACE + L" " +

#define HIDTesterSetting L"HIDApiTesterPath"





// #define APPCOMMAND_VOLUME_MUTE 0x80000
// #define APPCOMMAND_VOLUME_UP 0xA0000
// #define APPCOMMAND_VOLUME_DOWN 0x90000               
// #define WM_APPCOMMAND 0x319                                          


const char * Specialized_LogBuffer(char * dest, char * src) {
    int len = (strlen(dest) + strlen(src));
    char * buf = (char *) malloc(sizeof(char) * len);
    strcpy(buf, dest);
    strcat(buf, src);
    return buf;
}

enum class VolumeUnit {
    Decibel,
    Scalar
};


 PCWSTR HID_PATH = nullptr;


DLLExport float GetSystemVolume(VolumeUnit vUnit) {
        HRESULT hr;

        // -------------------------
        CoInitialize(NULL);
        IMMDeviceEnumerator *deviceEnumerator = NULL;
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
        IMMDevice *defaultDevice = NULL;

        hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
        deviceEnumerator->Release();
        deviceEnumerator = NULL;

        IAudioEndpointVolume *endpointVolume = NULL;
        hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
        defaultDevice->Release();
        defaultDevice = NULL;

        float currentVolume = 0;
        if (vUnit == VolumeUnit::Decibel) {
            //Current volume in dB
            hr = endpointVolume->GetMasterVolumeLevel(&currentVolume);
        }

        else if (vUnit == VolumeUnit::Scalar) {
            //Current volume as a scalar
            hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
        }
        endpointVolume->Release();
        CoUninitialize();

        return currentVolume;
    }




using SendMessageW_t = decltype(&SendMessageW);
SendMessageW_t  SendMessageW_Original;


VOID startup(LPCTSTR lpApplicationName, LPWSTR cmdLine )
{
   // additional information
   STARTUPINFO si;     
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

  // start the program up
  CreateProcess( lpApplicationName,   // the path
    cmdLine,        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    CREATE_NO_WINDOW,              // No window creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}


std::wstring  FormCommand(float vol) {
    LPWSTR vid = (LPWSTR) L"3282"; //setting
    LPWSTR pid = (LPWSTR) L"0001"; //setting
    LPWSTR usage_page = (LPWSTR) L"65280"; //setting
    LPWSTR usage = (LPWSTR) L"1";          //setting
    // LPWSTR length = (LPWSTR) L"64";        //setting

    int voll = (int) (vol*100);

    std::wstringstream ss;
    ss << voll;
    std::wstring volstring = ss.str();

    std::wstring commandp1(L"64,0x11,0x83,00,00,");
    std::wstring commandp2(L",00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00"); //Setting
    std::wstring command = commandp1 + volstring + commandp2;
    std::wstring q1 = std::wstring(L"\"");
    std::wstring fullstr(q1 + HID_PATH + q1);

    std::wstring completestr =  std::wstring(fullstr SPACE L"--vidpid" SPACE vid + L":" + pid SPACE L"--usagePage" SPACE usage_page SPACE L"--usage" SPACE usage SPACE L"--open" SPACE L"--send-output" SPACE command);     
    Wh_Log( L"%s", ( PCWSTR) completestr.c_str());

    





    return completestr;

}


DWORD WINAPI ThreadFunc(void * data) {
        Sleep(2 SECONDS);
        float vol = GetSystemVolume(VolumeUnit::Scalar);

        Wh_Log(L"Volume Info: %f", vol);
        startup(HID_PATH, ((LPWSTR) FormCommand(vol).c_str()));
        Sleep(5 SECONDS);
        vol = GetSystemVolume(VolumeUnit::Scalar);
        Wh_Log(L"Volume Info: %f", vol);
        startup(HID_PATH, ((LPWSTR) FormCommand(vol).c_str()));
        Sleep(2 SECONDS);
        //Sleep(15 SECONDS);
        //vol = GetSystemVolume(VolumeUnit::Scalar);
        //Wh_Log(L"Volume Info: %f", vol);
        //startup(L"hidapitester.exe", FormCommand(vol));
        return 0;
}


 LRESULT WINAPI SendMessage_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  
  
  // Check for correct message 
  if (Msg == WM_APPCOMMAND) {
    WhL("HOOOKED ON LIFE")
  } else if (Msg  == 1046) {
    Wh_Log(L"Message: %u Sent", Msg);
    if (Msg == 1046) {
        Wh_Log(L" PTR FOUND %i",  (unsigned short ) lParam );
        if ((unsigned short) lParam >= 0) {
            CreateThread(nullptr, 0, ThreadFunc, nullptr, 0, nullptr);
        }
        
    }
  } else {
    //if (Msg != 78 && Msg != 20 )
      //Wh_Log(L"Message: %u Sent", Msg);

  }

  
  return SendMessageW_Original(hWnd, Msg, wParam, lParam);
}



void Set_Hooks() {
        HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        // SendMessageW_t * pSendMessageW =
        //     GetProcAddress(user32Module, "SendMessageW");

            WindhawkUtils::SetFunctionHook(SendMessageW,
                               SendMessage_Hook,
                               &SendMessageW_Original);
        
    }

}

void Download_EXE() {
    const wchar_t * download_url = L"https://github.com/todbot/hidapitester/releases";
    wchar_t message [3000];
    wcscpy(message, L"Use the following URL to download HIDAPITester, or set the path: ");
    wcscat(message, download_url);
    MessageBox(NULL, message, L"HIDApiTester Not Found", MB_OK);

}
BOOL FileExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


void UnInit_Settings() {
    if (HID_PATH != nullptr) {
         Wh_FreeStringSetting(HID_PATH);
    }
    
}

void Init_Settings() {
     UnInit_Settings();
    HID_PATH = Wh_GetStringSetting(HIDTesterSetting);
}



bool Check_Requirements() {
    if (!FileExists(HID_PATH)) {
        Download_EXE();
        return FALSE;
    }
    return TRUE;
}


void Wh_ModSettingsChanged() {
    Init_Settings();
}

void Wh_ModBeforeUninit() {
    UnInit_Settings();
}

BOOL Wh_ModInit() {

    // do {                                                 
    //  const char * ptr = Specialized_LogBuffer( (char *) "HELLO: ", (char *) "%i\n");    
    //  Wh_Log(ptr , 42);                 
    //  free(   ( char *)ptr);                                     
    //  } while(0);   





    


    WhL("Begin INIT");
    WhL("Begin Settings")
    Init_Settings();
    WhL("Begin Requirments")
    if (!Check_Requirements()) {
        return FALSE;
    }
    WhL("Begin Hooks")
    Set_Hooks();
    //if (!FileExists(L"hidapitester.exe")) {
        //Download_EXE();
    //}



    return TRUE;
}


