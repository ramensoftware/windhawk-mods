#include <windows.h>
#include <audioclient.h>
#include <audioclientactivationparams.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <roapi.h>
#include <winstring.h>
#include <inspectable.h>
#include <wrl.h>
#include <wrl/wrappers/corewrappers.h>
#include <dbghelp.h>

#include <algorithm>
#include <cmath>
#include <cwchar>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;

struct Failure {
    const wchar_t* operation;
    HRESULT code;
};

void Check(HRESULT hr, const wchar_t* operation) {
    if (FAILED(hr)) throw Failure{operation, hr};
}

#define CHECK(expr) Check((expr), L#expr)

struct CloseHandleDeleter {
    void operator()(void* handle) const { CloseHandle(handle); }
};
using Handle = std::unique_ptr<void, CloseHandleDeleter>;

void Require(bool condition, const wchar_t* operation) {
    if (!condition) throw Failure{operation, E_FAIL};
}

constexpr WORD channels = 2;
constexpr DWORD sampleRate = 48000;
constexpr WORD frameBytes = channels * sizeof(short);
constexpr WAVEFORMATEX format{WAVE_FORMAT_PCM, channels, sampleRate,
                              sampleRate * frameBytes, frameBytes, 16, 0};
constexpr DWORD convert = AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                          AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;

ComPtr<IMMDeviceEnumerator> Enumerator() {
    ComPtr<IMMDeviceEnumerator> enumerator;
    CHECK(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                           CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator)));
    return enumerator;
}

std::wstring Id(IMMDevice* device) {
    wchar_t* raw = nullptr;
    CHECK(device->GetId(&raw));
    std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> id(raw, CoTaskMemFree);
    return id.get();
}

std::wstring DeviceName(IMMDevice* device) {
    ComPtr<IPropertyStore> properties;
    CHECK(device->OpenPropertyStore(STGM_READ, &properties));
    PROPVARIANT value{};
    CHECK(properties->GetValue(PKEY_Device_FriendlyName, &value));
    const std::wstring name = value.vt == VT_LPWSTR ? value.pwszVal : L"";
    PropVariantClear(&value);
    return name;
}

struct Renderer {
    ComPtr<IAudioClient> client;
    ComPtr<IAudioRenderClient> render;
    UINT32 capacity{};
    unsigned long long written{};

    explicit Renderer(IMMDevice* device) {
        CHECK(device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
                               nullptr, reinterpret_cast<void**>(client.GetAddressOf())));
        CHECK(client->Initialize(AUDCLNT_SHAREMODE_SHARED, convert, 500000,
                                 0, &format, nullptr));
        CHECK(client->GetBufferSize(&capacity));
        CHECK(client->GetService(IID_PPV_ARGS(&render)));
    }

    UINT32 Available() const {
        UINT32 padding{};
        CHECK(client->GetCurrentPadding(&padding));
        return capacity - padding;
    }

    void Write(const BYTE* source, UINT32 frames) {
        if (!frames) return;
        BYTE* data{};
        CHECK(render->GetBuffer(frames, &data));
        if (source) std::memcpy(data, source, size_t(frames) * frameBytes);
        CHECK(render->ReleaseBuffer(frames, source ? 0 : AUDCLNT_BUFFERFLAGS_SILENT));
        written += frames;
    }
};

void Tone(const wchar_t* id) {
    auto en = Enumerator();
    ComPtr<IMMDevice> device;
    CHECK(en->GetDevice(id, &device));
    Renderer source(device.Get());
    std::vector<short> samples(size_t(source.capacity) * channels);
    CHECK(source.client->Start());
    const auto deadline = GetTickCount64() + 4400;
    while (GetTickCount64() < deadline) {
        const auto frames = source.Available();
        for (UINT32 i = 0; i < frames; ++i) {
            const auto phase = (source.written + i) % 100;
            const auto value = static_cast<short>(120 * std::sin(phase * (6.283185307179586 / 100)));
            samples[size_t(i) * channels] = samples[size_t(i) * channels + 1] = value;
        }
        source.Write(reinterpret_cast<const BYTE*>(samples.data()), frames);
        Sleep(5);
    }
    CHECK(source.client->Stop());
}

// The published Win11 IID and method positions agree in AppAudioRouter and
// Nimbus. Only this exact IID is queried. Unused methods are never called.
struct __declspec(uuid("ab3d4648-e242-459f-b02f-541c70306324")) Policy : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Reserved01() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved02() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved03() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved04() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved05() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved06() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved07() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved08() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved09() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved10() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved11() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved12() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved13() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved14() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved15() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved16() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved17() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved18() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reserved19() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPersistedDefaultAudioEndpoint(
        UINT32, EDataFlow, ERole, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPersistedDefaultAudioEndpoint(
        UINT32, EDataFlow, ERole, HSTRING*) = 0;
};

void ProbePolicy(const std::wstring& id, bool write) {
    ComPtr<Policy> policy;
    CHECK(RoGetActivationFactory(HStringReference(L"Windows.Media.Internal.AudioPolicyConfig").Get(),
                                 IID_PPV_ARGS(&policy)));
    const DWORD pid = GetCurrentProcessId();
    HString previous;
    auto hr = policy->GetPersistedDefaultAudioEndpoint(pid, eRender, eMultimedia,
                                                       previous.GetAddressOf());
    if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) CHECK(hr);
    wprintf(L"Policy IID accepted, read HRESULT=0x%08lX\n", static_cast<unsigned long>(hr));
    if (!write) return;

    const auto path = L"\\\\?\\SWD#MMDEVAPI#" + id +
                      L"#{e6327cad-dcec-4949-ae8a-991e976a79d2}";
    HString target;
    CHECK(target.Set(path.c_str(), static_cast<UINT32>(path.size())));
    CHECK(policy->SetPersistedDefaultAudioEndpoint(pid, eRender, eMultimedia, target.Get()));
    HString actual;
    const auto read = policy->GetPersistedDefaultAudioEndpoint(pid, eRender, eMultimedia,
                                                               actual.GetAddressOf());
    CHECK(policy->SetPersistedDefaultAudioEndpoint(pid, eRender, eMultimedia, previous.Get()));
    HString restored;
    const auto restoredRead = policy->GetPersistedDefaultAudioEndpoint(
        pid, eRender, eMultimedia, restored.GetAddressOf());
    if (restoredRead != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) CHECK(restoredRead);
    CHECK(read);
    Require(CompareStringOrdinal(actual.GetRawBuffer(nullptr), -1, path.c_str(), -1, TRUE) ==
                CSTR_EQUAL,
            L"Read back explicit endpoint");
    Require(std::wstring_view(restored.GetRawBuffer(nullptr)) ==
                std::wstring_view(previous.GetRawBuffer(nullptr)),
            L"Verify restored policy");
    wprintf(L"Policy write, read-back and restoration passed for the probe process.\n");
}

class Activation final : public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    IActivateAudioInterfaceCompletionHandler, Microsoft::WRL::FtmBase> {
public:
    Handle ready{CreateEventW(nullptr, TRUE, FALSE, nullptr)};
    AUDIOCLIENT_ACTIVATION_PARAMS params{};
    PROPVARIANT activation{};

    HRESULT STDMETHODCALLTYPE ActivateCompleted(IActivateAudioInterfaceAsyncOperation*) override {
        SetEvent(ready.get());
        return S_OK;
    }
};

ComPtr<IAudioClient> Capture(DWORD pid) {
    auto handler = Microsoft::WRL::Make<Activation>();
    Require(handler && handler->ready, L"Create activation event");
    handler->params.ActivationType = AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK;
    handler->params.ProcessLoopbackParams = {pid, PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE};
    handler->activation.vt = VT_BLOB;
    handler->activation.blob = {sizeof(handler->params), reinterpret_cast<BYTE*>(&handler->params)};
    ComPtr<IActivateAudioInterfaceAsyncOperation> op;
    CHECK(ActivateAudioInterfaceAsync(VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK, __uuidof(IAudioClient),
                                     &handler->activation, handler.Get(), &op));
    Require(WaitForSingleObject(handler->ready.get(), 5000) == WAIT_OBJECT_0,
            L"Wait for process-loopback activation");
    HRESULT result = E_FAIL;
    ComPtr<IUnknown> activated;
    CHECK(op->GetActivateResult(&result, &activated));
    CHECK(result);
    ComPtr<IAudioClient> client;
    CHECK(activated.As(&client));
    CHECK(client->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK |
                             AUDCLNT_STREAMFLAGS_EVENTCALLBACK | convert,
                             0, 0, &format, nullptr));
    return client;
}

void ProbeMirror(const std::wstring& primary, const std::vector<ComPtr<IMMDevice>>& targets) {
    wchar_t executable[MAX_PATH]{};
    const auto length = GetModuleFileNameW(nullptr, executable, MAX_PATH);
    Require(length && length < MAX_PATH, L"Locate probe executable");
    auto command = L"\"" + std::wstring(executable) + L"\" --tone \"" + primary + L"\"";
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    Require(CreateProcessW(executable, command.data(), nullptr, nullptr, FALSE,
                           CREATE_SUSPENDED, nullptr, nullptr, &startup, &process),
            L"Create source process");
    struct Child {
        Handle process;
        ~Child() {
            if (WaitForSingleObject(process.get(), 5000) == WAIT_TIMEOUT) {
                TerminateProcess(process.get(), 1);
                WaitForSingleObject(process.get(), 1000);
            }
        }
    } child{Handle(process.hProcess)};
    Handle thread(process.hThread);
    Handle packet(CreateEventW(nullptr, FALSE, FALSE, nullptr));
    Require(bool(packet), L"Create capture event");
    Require(ResumeThread(thread.get()) != DWORD(-1), L"Start source process");
    auto capture = Capture(process.dwProcessId);
    CHECK(capture->SetEventHandle(packet.get()));
    ComPtr<IAudioCaptureClient> reader;
    CHECK(capture->GetService(IID_PPV_ARGS(&reader)));

    std::vector<Renderer> renders;
    for (const auto& target : targets) {
        auto& render = renders.emplace_back(target.Get());
        CHECK(render.client->Start());
    }
    CHECK(capture->Start());
    unsigned long long captured{};
    unsigned long long signalFrames{};
    auto deadline = GetTickCount64() + 3000;
    while (GetTickCount64() < deadline) {
        if (WaitForSingleObject(packet.get(), 100) != WAIT_OBJECT_0) continue;
        UINT32 frames{};
        CHECK(reader->GetNextPacketSize(&frames));
        while (frames) {
            BYTE* data{};
            DWORD flags{};
            CHECK(reader->GetBuffer(&data, &frames, &flags, nullptr, nullptr));
            const bool silence = (flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0;
            if (!silence) {
                const auto* samples = reinterpret_cast<const short*>(data);
                for (UINT32 i = 0; i < frames; ++i)
                    if (samples[size_t(i) * channels] != 0) ++signalFrames;
            }
            for (auto& render : renders)
                render.Write(silence ? nullptr : data, std::min(frames, render.Available()));
            captured += frames;
            CHECK(reader->ReleaseBuffer(frames));
            CHECK(reader->GetNextPacketSize(&frames));
        }
    }
    CHECK(capture->Stop());
    wprintf(L"Loopback frames=%llu nonzero frames=%llu\n", captured, signalFrames);
    Require(signalFrames > sampleRate, L"Capture more than one second of tone");
    for (size_t i = 0; i < renders.size(); ++i) {
        auto& render = renders[i];
        CHECK(render.client->Stop());
        wprintf(L"Mirror %ls frames=%llu\n", DeviceName(targets[i].Get()).c_str(), render.written);
        Require(render.written > sampleRate, L"Submit more than one second to mirror");
    }
}

struct SymbolQuery {
    DWORD64 base;
    const wchar_t* filter;
};

BOOL CALLBACK PrintShellSymbol(PSYMBOL_INFOW symbol, ULONG, void* context) {
    const auto& query = *static_cast<SymbolQuery*>(context);
    const std::wstring_view name(symbol->Name, symbol->NameLen);
    if (name.starts_with(L"`")) return TRUE;
    const auto contains = [name](std::wstring_view token) {
        return name.find(token) != std::wstring_view::npos;
    };
    const auto print = [&] {
        wprintf(L"RVA %llX %ls\n", symbol->Address - query.base, symbol->Name);
    };
    if (query.filter) {
        if (contains(query.filter)) print();
        return TRUE;
    }
    if (contains(L"winrt::impl::produce<") && (contains(L"OnGotFocus") || contains(L"::Connect")) &&
        (contains(L"ControlCenterView") || contains(L"FullScreenPage"))) {
        print();
    }
    if ((contains(L"AudioSession") || contains(L"AppVolume") || contains(L"VolumeMixer")) &&
        (contains(L"ProcessId") || contains(L"Identifier") || contains(L"AppName") ||
         contains(L"SessionId") || contains(L"AppId") || contains(L"AppUserModel"))) {
        print();
    }
    return TRUE;
}

void ProbeSymbols(const wchar_t* directory, const wchar_t* dll, wchar_t* filter) {
    const auto process = GetCurrentProcess();
    SymSetOptions((filter && filter[0] == L'?' ? 0 : SYMOPT_UNDNAME) |
                  SYMOPT_EXACT_SYMBOLS | SYMOPT_FAIL_CRITICAL_ERRORS);
    Require(SymInitializeW(process, directory, FALSE), L"Initialize DbgHelp");
    struct Cleanup {
        HANDLE process;
        ~Cleanup() { SymCleanup(process); }
    } cleanup{process};
    const auto base = SymLoadModuleExW(process, nullptr, dll, nullptr, 0, 0, nullptr, 0);
    Require(base != 0, L"Load symbol module");
    IMAGEHLP_MODULEW64 info{sizeof(info)};
    Require(SymGetModuleInfoW64(process, base, &info) &&
                info.SymType == SymPdb && !info.PdbUnmatched,
            L"Verify exact PDB");
    wprintf(L"Matched PDB %ls, age %lu\n", info.LoadedPdbName, info.PdbAge);
    SymbolQuery query{base, filter};
    Require(SymEnumSymbolsW(process, base, L"*", PrintShellSymbol, &query),
            L"Enumerate symbols");
}

int wmain(int argc, wchar_t** argv) {
    const auto initialized = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialized)) return 1;
    int code = 0;
    try {
        const std::wstring_view mode = argc >= 2 ? argv[1] : L"";
        if (mode == L"--symbols" && (argc == 4 || argc == 5)) {
            ProbeSymbols(argv[2], argv[3], argc == 5 ? argv[4] : nullptr);
        } else if (mode == L"--tone" && argc == 3) {
            Tone(argv[2]);
        } else {
            Require((argc <= 2 && (mode.empty() || mode == L"--policy-write")) ||
                        (mode == L"--mirror" && argc >= 3 && argc <= 4),
                    L"Usage: audio-probe.exe [--mirror <index> [index] | --policy-write | "
                    L"--symbols <pdb-dir> <dll> [filter]]");
            auto en = Enumerator();
            ComPtr<IMMDevice> primary;
            CHECK(en->GetDefaultAudioEndpoint(eRender, eMultimedia, &primary));
            const auto primaryId = Id(primary.Get());
            wprintf(L"Primary: %ls\n", DeviceName(primary.Get()).c_str());
            ComPtr<IMMDeviceCollection> devices;
            CHECK(en->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices));
            UINT count{};
            CHECK(devices->GetCount(&count));
            std::vector<ComPtr<IMMDevice>> outputs;
            std::wstring policyTarget;
            for (UINT i = 0; i < count; ++i) {
                ComPtr<IMMDevice> device;
                CHECK(devices->Item(i, &device));
                auto id = Id(device.Get());
                wprintf(L"Active [%u]: %ls\n", i, DeviceName(device.Get()).c_str());
                if (id != primaryId && policyTarget.empty()) policyTarget = id;
                outputs.push_back(std::move(device));
            }
            if (mode == L"--mirror") {
                std::vector<ComPtr<IMMDevice>> targets;
                for (int i = 2; i < argc; ++i) {
                    wchar_t* end{};
                    const auto index = std::wcstoul(argv[i], &end, 10);
                    Require(end != argv[i] && *end == 0 && index < outputs.size(),
                            L"Select an active endpoint by its index");
                    auto& output = outputs[index];
                    Require(output && Id(output.Get()) != primaryId,
                            L"Select distinct non-primary endpoints");
                    targets.push_back(std::move(output));
                }
                ProbeMirror(primaryId, targets);
            } else {
                Require(mode.empty() || !policyTarget.empty(), L"At least one active non-primary endpoint");
                ProbePolicy(policyTarget, mode == L"--policy-write");
            }
        }
    } catch (const Failure& error) {
        fwprintf(stderr, L"%ls failed: 0x%08lX\n", error.operation,
                 static_cast<unsigned long>(error.code));
        code = 1;
    }
    RoUninitialize();
    return code;
}
