// ==WindhawkMod==
// @id              volume-key-step
// @name            Volume Key Step
// @description     Choose how much each volume key press raises or lowers the volume
// @version         1.0
// @author          Artistro08
// @github          https://github.com/artistro08
// @include         explorer.exe
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Volume Key Step
Windows raises or lowers the volume by 2% per volume key press. This mod lets you
pick any step from 1% to 100%. The Windows volume flyout still shows as usual.

The step also applies to anything else that steps the volume through Explorer, such as
scrolling with the Windows 11 indicator in the Taskbar Volume Control mod.

At least one audio device (speaker or mic) must be connected when Explorer starts. If
none was, reload the mod after connecting one.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- step: 5
  $name: Volume step
  $description: Percent to change per key press (1-100).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>

using Microsoft::WRL::ComPtr;
using VolumeStep = HRESULT STDMETHODCALLTYPE(IAudioEndpointVolume*, LPCGUID);

constexpr int kVolumeStepUpSlot   = 17;
constexpr int kVolumeStepDownSlot = 18;

std::atomic<int> g_step;
VolumeStep* VolumeStepUp_Original;
VolumeStep* VolumeStepDown_Original;

HRESULT StepVolume(IAudioEndpointVolume* self, LPCGUID context, int direction, VolumeStep* fallback) {
    float level;
    if (FAILED(self->GetMasterVolumeLevelScalar(&level))) {
        return fallback(self, context);
    }

    float percent = std::clamp(std::round(level * 100) + direction * g_step, 0.0f, 100.0f);
    return self->SetMasterVolumeLevelScalar(percent / 100, context);
}

HRESULT STDMETHODCALLTYPE VolumeStepUp_Hook(IAudioEndpointVolume* self, LPCGUID context) {
    return StepVolume(self, context, +1, VolumeStepUp_Original);
}

HRESULT STDMETHODCALLTYPE VolumeStepDown_Hook(IAudioEndpointVolume* self, LPCGUID context) {
    return StepVolume(self, context, -1, VolumeStepDown_Original);
}

VolumeStep** EndpointVolumeVtable() {
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IMMDeviceCollection> devices;
    ComPtr<IAudioEndpointVolume> volume;
    UINT count = 0;

    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator))) ||
        FAILED(enumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &devices)) ||
        FAILED(devices->GetCount(&count))) {
        return nullptr;
    }

    for (UINT i = 0; i < count && !volume; i++) {
        ComPtr<IMMDevice> device;
        if (SUCCEEDED(devices->Item(i, &device))) {
            device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, &volume);
        }
    }

    if (!volume) {
        return nullptr;
    }

    auto vtable = *reinterpret_cast<VolumeStep***>(volume.Get());

    HMODULE pinned_audio_module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                            reinterpret_cast<LPCWSTR>(vtable[kVolumeStepUpSlot]), &pinned_audio_module)) {
        return nullptr;
    }

    return vtable;
}

void LoadSettings() {
    g_step = std::clamp(Wh_GetIntSetting(L"step"), 1, 100);
}

BOOL Wh_ModInit() {
    LoadSettings();

    HRESULT com  = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    auto vtable  = EndpointVolumeVtable();
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }

    if (!vtable) {
        Wh_Log(L"No active audio device found");
        return FALSE;
    }

    return WindhawkUtils::SetFunctionHook(vtable[kVolumeStepUpSlot], VolumeStepUp_Hook, &VolumeStepUp_Original) &&
           WindhawkUtils::SetFunctionHook(vtable[kVolumeStepDownSlot], VolumeStepDown_Hook, &VolumeStepDown_Original);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
