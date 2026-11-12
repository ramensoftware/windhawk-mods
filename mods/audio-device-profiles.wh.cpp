// ==WindhawkMod==
// @id              audio-device-profiles
// @name            Audio Device Profiles
// @description     Remembers each audio device's format, speaker configuration and spatial sound, and restores them when Windows resets them
// @version         0.4.0
// @author          Corey Shay
// @github          https://github.com/CoreyShay
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lpropsys -luuid -lgdi32 -lshell32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Audio Device Profiles

Windows forgets an audio endpoint's speaker configuration whenever the device is re-detected: after switching devices, after a driver
update, and every time a Remote Desktop session takes the audio device and hands it back. The sample rate and bit depth usually
survive, but the channel mask is silently reset to stereo, and the only way to restore it is the Configure wizard buried inside the
legacy Sound control panel.

This mod remembers the format, speaker configuration and spatial sound format of every device you use, and puts them back when the
device returns.

It also gives you one panel, opened from its **tray icon** or with **Ctrl+Shift+Alt+A**, that shows your devices, bit depths, sample
rates, speaker layouts and spatial sound formats side by side. The value currently in effect for each is highlighted, so everything is
readable at a glance, and anything the selected device cannot do is greyed out. Clicking applies immediately.

There is nothing to configure. The mod learns each device's settings as you use them: change a device's format or speaker layout the
normal way, and that becomes what gets restored from then on. A change made in the first few seconds after a device appears is treated
as Windows resetting it, and is undone.

If you would rather nothing at all could change your settings, turn on **Lock settings chosen in the panel**. Anything you picked in
the panel is then treated as final and put back whenever it changes, whatever caused it, instead of being relearned.

The device you are using is also remembered as your preferred default. If it disappears, Windows will move the default elsewhere as
usual, but the moment your device comes back it is made the default again with its settings intact. Deliberately switching to another
device while your preferred one is still present updates the preference instead.

## Spatial sound

Turning on Windows Sonic, Dolby Atmos or DTS:X hands the endpoint's format to the spatial renderer, so while one of them is active the
bit depth, sample rate and speaker columns are greyed out, exactly as the Sound control panel greys out its format dropdown. The
format you chose is kept, shown greyed for reference, and restored the moment you turn spatial sound off again.

Windows reports some spatial formats as supported on hardware that has no license for them. Selecting one of those appears to succeed
but changes nothing; the mod detects this by reading the setting back, says so in the log, and does not keep retrying.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enforceProfiles: true
  $name: Restore device settings
  $description: >-
    Puts a device's remembered format and speaker configuration back when Windows resets them.
- restoreSpatial: true
  $name: Restore spatial sound format
  $description: >-
    Also remembers and restores each device's Windows Sonic, Dolby Atmos or DTS:X setting. Turn this off if you would rather manage
    spatial sound yourself.
- restorePreferredDefault: true
  $name: Restore preferred device
  $description: >-
    Makes your preferred device the default again when it becomes available, instead of leaving whatever Windows switched to.
- lockUserSettings: false
  $name: Lock settings chosen in the panel
  $description: >-
    Normally a change made outside the panel is taken as you reconfiguring the device and becomes the new remembered setting. With
    this on, anything you picked in the panel is treated as final and is put back whenever it changes, no matter what caused it. Use
    the panel to change those devices from then on.
- skipInRemoteSession: true
  $name: Do nothing during Remote Desktop sessions
  $description: >-
    Leaves audio devices alone while you are connected remotely, so the Remote Audio device is not interfered with. Your local device
    is restored once you are back at the machine.
- logCapabilities: false
  $name: Log device capabilities
  $description: >-
    Writes the full list of supported speaker configurations, bit depths and sample rates for every audio device to the Windhawk log.
    Useful for reporting problems.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <inspectable.h>
#include <mmdeviceapi.h>
#include <roapi.h>
#include <shellapi.h>
#include <windowsx.h>
#include <winstring.h>

#include <cstdint>
#include <cwchar>
#include <string>
#include <vector>

// ====================================================================================================
//	Speaker masks
//
//	Defined here rather than pulled from <ksmedia.h>, which drags in kernel streaming headers that do not agree with the mingw
//	toolchain Windhawk compiles with.
// ====================================================================================================

namespace speaker
{
	constexpr DWORD kFrontLeft = 0x1;
	constexpr DWORD kFrontRight = 0x2;
	constexpr DWORD kFrontCenter = 0x4;
	constexpr DWORD kLowFrequency = 0x8;
	constexpr DWORD kBackLeft = 0x10;
	constexpr DWORD kBackRight = 0x20;
	constexpr DWORD kSideLeft = 0x200;
	constexpr DWORD kSideRight = 0x400;
	constexpr DWORD kBackCenter = 0x100;

	constexpr DWORD kMono = kFrontCenter;
	constexpr DWORD kStereo = kFrontLeft | kFrontRight;
	constexpr DWORD kQuad = kStereo | kBackLeft | kBackRight;

	//	What the Sound applet calls plain "Surround": front pair, center and a single rear channel. Distinct from Quadraphonic.
	constexpr DWORD kSurround = kStereo | kFrontCenter | kBackCenter;
	constexpr DWORD kSurround51 = kStereo | kFrontCenter | kLowFrequency | kBackLeft | kBackRight;
	constexpr DWORD kSurround51Side = kStereo | kFrontCenter | kLowFrequency | kSideLeft | kSideRight;
	constexpr DWORD kSurround71 = kSurround51 | kSideLeft | kSideRight;
}

// ====================================================================================================
//	Audio subformat GUIDs
//
//	Declared locally so the mod does not have to link against ksuser, which is the only import library that exports them.
// ====================================================================================================

namespace subformat
{
	const GUID kPcm = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
	const GUID kFloat = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
}

// ====================================================================================================
//	IPolicyConfig
//
//	Undocumented, and the only way to write an endpoint's format. It is what the Sound control panel itself uses. Two details are easy
//	to get wrong and both were verified against a live system:
//
//	1. IPolicyConfigVista does not exist on Windows 11. Only the IID below resolves.
//	2. GetPropertyValue and SetPropertyValue take a leading BOOL bFxStore. The declaration that circulates online omits it, which
//	   misaligns every following argument and access-violates inside the host process.
//
//	Because the layout is undocumented it is verified at runtime before any write is attempted. See PolicyConfigWriter::Validate.
// ====================================================================================================

struct DeviceShareMode;

struct IPolicyConfig : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR deviceId, WAVEFORMATEX** format) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR deviceId, BOOL defaultDevice, WAVEFORMATEX** format) = 0;
	virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR deviceId) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR deviceId, WAVEFORMATEX* endpointFormat, WAVEFORMATEX* mixFormat) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR deviceId, BOOL defaultDevice, INT64* defaultPeriod, INT64* minimumPeriod) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR deviceId, INT64* period) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR deviceId, DeviceShareMode* mode) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR deviceId, DeviceShareMode* mode) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR deviceId, BOOL fxStore, const PROPERTYKEY& key, PROPVARIANT* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR deviceId, BOOL fxStore, const PROPERTYKEY& key, PROPVARIANT* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR deviceId, BOOL visible) = 0;
};

const CLSID kClsidPolicyConfigClient = { 0x870af99c, 0x171d, 0x4f9e, { 0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9 } };
const IID kIidPolicyConfig = { 0xf8679f50, 0x850a, 0x41cf, { 0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8 } };

//	The endpoint's configured format, as shown by the Sound control panel's Default Format dropdown.
const PROPERTYKEY kPkeyDeviceFormat = { { 0xf19f064d, 0x082c, 0x4e27, { 0xbc, 0x73, 0x68, 0x82, 0xa1, 0xbb, 0x8e, 0x4c } }, 0 };

//	The speaker layout chosen in the Configure wizard. The channel mask in the device format is what actually drives the engine, but
//	this is kept in sync so the control panel agrees with us.
const PROPERTYKEY kPkeyPhysicalSpeakers = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e } }, 3 };

// ====================================================================================================
//	Small RAII helpers
// ====================================================================================================

//	Runs a lambda when the enclosing scope ends, however it ends. Used for the teardown that has no natural owning object, such as
//	CoUninitialize and unregistering the device notification callback, so those cannot be skipped by an early return.
template <typename FN>
class [[nodiscard]] ScopeExiter
{
public:
	explicit ScopeExiter(FN fn)
		: m_fn(fn)
	{
	}

	~ScopeExiter()
	{
		m_fn();
	}

	ScopeExiter(const ScopeExiter&) = delete;
	ScopeExiter& operator=(const ScopeExiter&) = delete;

private:
	FN m_fn;
};

template <typename FN>
static ScopeExiter<FN> MakeScopeExiter(FN fn)
{
	return ScopeExiter<FN>(fn);
}

#define ON_SCOPE_EXIT_JOIN2(a, b) a##b
#define ON_SCOPE_EXIT_JOIN(a, b) ON_SCOPE_EXIT_JOIN2(a, b)
#define ON_SCOPE_EXIT(fn) auto ON_SCOPE_EXIT_JOIN(scopeExiter, __COUNTER__) = MakeScopeExiter(fn)

template <typename T>
class ComPtr
{
public:
	ComPtr() = default;

	~ComPtr()
	{
		Reset();
	}

	ComPtr(const ComPtr&) = delete;
	ComPtr& operator=(const ComPtr&) = delete;

	T** operator&()
	{
		Reset();
		return &m_p;
	}

	T* operator->() const
	{
		return m_p;
	}

	T* Get() const
	{
		return m_p;
	}

	explicit operator bool() const
	{
		return m_p != nullptr;
	}

	void Reset()
	{
		if (m_p)
		{
			m_p->Release();
			m_p = nullptr;
		}
	}

private:
	T* m_p = nullptr;
};

class CoTaskMemPtr
{
public:
	CoTaskMemPtr() = default;

	~CoTaskMemPtr()
	{
		Reset();
	}

	CoTaskMemPtr(const CoTaskMemPtr&) = delete;
	CoTaskMemPtr& operator=(const CoTaskMemPtr&) = delete;

	void** operator&()
	{
		Reset();
		return &m_p;
	}

	void* Get() const
	{
		return m_p;
	}

	void Reset()
	{
		if (m_p)
		{
			CoTaskMemFree(m_p);
			m_p = nullptr;
		}
	}

private:
	void* m_p = nullptr;
};

class PropVariant
{
public:
	PropVariant()
	{
		PropVariantInit(&m_value);
	}

	~PropVariant()
	{
		PropVariantClear(&m_value);
	}

	PropVariant(const PropVariant&) = delete;
	PropVariant& operator=(const PropVariant&) = delete;

	PROPVARIANT* operator&()
	{
		return &m_value;
	}

	const PROPVARIANT& Get() const
	{
		return m_value;
	}

private:
	PROPVARIANT m_value;
};

// ====================================================================================================
//	Format description
// ====================================================================================================

struct SpeakerConfig
{
	const wchar_t* const name;
	const DWORD mask;
	const WORD channels;
};

struct DepthOption
{
	const wchar_t* const name;
	const WORD bitsPerSample;
	const WORD containerBits;
};

//	Both 5.1 variants are probed because hardware commonly supports only one of them, and guessing wrong produces a device that
//	silently refuses the format. The side variant is what HDMI and most AV receivers report.
constexpr SpeakerConfig kSpeakerConfigs[] = {
	{ L"Mono", speaker::kMono, 1 },
	{ L"Stereo", speaker::kStereo, 2 },
	{ L"Quadraphonic", speaker::kQuad, 4 },
	{ L"Surround", speaker::kSurround, 4 },
	{ L"5.1 Surround", speaker::kSurround51, 6 },
	{ L"5.1 Surround (side)", speaker::kSurround51Side, 6 },
	{ L"7.1 Surround", speaker::kSurround71, 8 },
};

constexpr DepthOption kDepthOptions[] = {
	{ L"16 bit", 16, 16 },
	{ L"24 bit", 24, 24 },
	{ L"24 bit (32-bit container)", 24, 32 },
	{ L"32 bit", 32, 32 },
};

constexpr DWORD kSampleRates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000 };

constexpr size_t kSpeakerConfigCount = ARRAYSIZE(kSpeakerConfigs);
constexpr size_t kDepthOptionCount = ARRAYSIZE(kDepthOptions);
constexpr size_t kSampleRateCount = ARRAYSIZE(kSampleRates);

struct FormatSpec
{
	DWORD channelMask = speaker::kStereo;
	WORD channels = 2;
	DWORD sampleRate = 48000;
	WORD bitsPerSample = 16;
	WORD containerBits = 16;
	bool isFloat = false;
};

static void BuildWaveFormat(const FormatSpec& spec, WAVEFORMATEXTENSIBLE* out)
{
	ZeroMemory(out, sizeof(*out));

	out->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	out->Format.nChannels = spec.channels;
	out->Format.nSamplesPerSec = spec.sampleRate;
	out->Format.wBitsPerSample = spec.containerBits;
	out->Format.nBlockAlign = static_cast<WORD>(spec.channels * spec.containerBits / 8);
	out->Format.nAvgBytesPerSec = spec.sampleRate * out->Format.nBlockAlign;
	out->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

	out->Samples.wValidBitsPerSample = spec.bitsPerSample;
	out->dwChannelMask = spec.channelMask;
	out->SubFormat = spec.isFloat ? subformat::kFloat : subformat::kPcm;
}

static bool ParseWaveFormat(const WAVEFORMATEX* wfx, FormatSpec* out)
{
	if (!wfx)
	{
		return false;
	}

	out->channels = wfx->nChannels;
	out->sampleRate = wfx->nSamplesPerSec;
	out->containerBits = wfx->wBitsPerSample;
	out->bitsPerSample = wfx->wBitsPerSample;
	out->isFloat = (wfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT);
	out->channelMask = (wfx->nChannels == 1) ? speaker::kMono : speaker::kStereo;

	if (wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE && wfx->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))
	{
		const WAVEFORMATEXTENSIBLE* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(wfx);
		out->bitsPerSample = ext->Samples.wValidBitsPerSample ? ext->Samples.wValidBitsPerSample : wfx->wBitsPerSample;
		out->channelMask = ext->dwChannelMask;
		out->isFloat = IsEqualGUID(ext->SubFormat, subformat::kFloat) != FALSE;
	}

	return true;
}

static std::wstring DescribeSpeakerMask(DWORD mask)
{
	for (size_t i = 0; i < kSpeakerConfigCount; i++)
	{
		if (kSpeakerConfigs[i].mask == mask)
		{
			return kSpeakerConfigs[i].name;
		}
	}

	wchar_t buf[64];
	swprintf(buf, ARRAYSIZE(buf), L"Custom (0x%X)", static_cast<unsigned>(mask));
	return buf;
}

static std::wstring DescribeFormat(const FormatSpec& spec)
{
	wchar_t buf[200];

	if (spec.containerBits != spec.bitsPerSample)
	{
		swprintf(buf, ARRAYSIZE(buf), L"%u ch, %u Hz, %u bit (%u-bit container), mask 0x%X", spec.channels, static_cast<unsigned>(spec.sampleRate), spec.bitsPerSample, spec.containerBits,
			static_cast<unsigned>(spec.channelMask));
	}
	else
	{
		swprintf(buf, ARRAYSIZE(buf), L"%u ch, %u Hz, %u bit, mask 0x%X", spec.channels, static_cast<unsigned>(spec.sampleRate), spec.bitsPerSample, static_cast<unsigned>(spec.channelMask));
	}

	return buf;
}

static const wchar_t* DescribeState(DWORD state)
{
	switch (state)
	{
		case DEVICE_STATE_ACTIVE:
			return L"active";
		case DEVICE_STATE_DISABLED:
			return L"disabled";
		case DEVICE_STATE_NOTPRESENT:
			return L"not present";
		case DEVICE_STATE_UNPLUGGED:
			return L"unplugged";
		default:
			return L"unknown";
	}
}

// ====================================================================================================
//	Spatial sound formats
//
//	The Sound applet's "Spatial sound" tab is driven by the WinRT runtime class Windows.Media.Audio.SpatialAudioDeviceConfiguration.
//	The C++/WinRT headers that ship with the Windhawk toolchain predate it, so the ABI below is declared by hand from the interface
//	definitions in C:\Windows\System32\WinMetadata\Windows.Media.winmd.
//
//	Two behaviors here are worth knowing about, because neither is discoverable from the documentation:
//
//	GetForDeviceId does not take an MMDevice endpoint id. It takes the PnP device interface path, and handed an endpoint id it returns
//	a perfectly valid object with no error that reports every format as unsupported. The endpoint id has to be wrapped first.
//
//	IsSpatialAudioFormatSupported answers whether the endpoint could carry the format, not whether it can actually be selected. The
//	licensed formats (Dolby Atmos for Speakers, DTS:X) report true on hardware that has no license, and setting them then completes
//	successfully while leaving the format unchanged. The only reliable test is to read the value back afterwards.
// ====================================================================================================

struct ISpatialAudioDeviceConfiguration : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE get_DeviceId(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsSpatialAudioSupported(boolean* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE IsSpatialAudioFormatSupported(HSTRING subtype, boolean* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ActiveSpatialAudioFormat(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DefaultSpatialAudioFormat(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetDefaultSpatialAudioFormatAsync(HSTRING subtype, IInspectable** operation) = 0;
	virtual HRESULT STDMETHODCALLTYPE add_ConfigurationChanged(IInspectable* handler, INT64* token) = 0;
	virtual HRESULT STDMETHODCALLTYPE remove_ConfigurationChanged(INT64 token) = 0;
};

struct ISpatialAudioDeviceConfigurationStatics : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE GetForDeviceId(HSTRING deviceId, ISpatialAudioDeviceConfiguration** value) = 0;
};

struct ISpatialAudioFormatSubtypeStatics : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE get_WindowsSonic(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DolbyAtmosForHeadphones(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DolbyAtmosForHomeTheater(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DolbyAtmosForSpeakers(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DTSHeadphoneX(HSTRING* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DTSXUltra(HSTRING* value) = 0;
};

struct ISpatialAudioFormatSubtypeStatics2 : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE get_DTSXForHomeTheater(HSTRING* value) = 0;
};

struct ISpatialAsyncInfo : public IInspectable
{
	virtual HRESULT STDMETHODCALLTYPE get_Id(UINT32* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Status(int* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* value) = 0;
	virtual HRESULT STDMETHODCALLTYPE Cancel() = 0;
	virtual HRESULT STDMETHODCALLTYPE Close() = 0;
};

static const IID kIidSpatialDeviceConfigStatics = { 0x3EC37F7B, 0x936D, 0x4E04, { 0x97, 0x28, 0x28, 0x27, 0xD9, 0xF7, 0x58, 0xC4 } };
static const IID kIidSpatialSubtypeStatics = { 0xB3DE8A47, 0x83EE, 0x4266, { 0xA9, 0x45, 0xBE, 0xDF, 0x50, 0x7A, 0xFE, 0xED } };
static const IID kIidSpatialSubtypeStatics2 = { 0x4565E6CB, 0xD95B, 0x5621, { 0xB6, 0xAF, 0x0E, 0x88, 0x49, 0xC5, 0x7C, 0x80 } };
static const IID kIidSpatialAsyncInfo = { 0x00000036, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };

//	The device interface class every MMDevice endpoint is published under. Wrapping an endpoint id in this is what turns it into the
//	PnP path GetForDeviceId expects.
const wchar_t kMMDeviceInterfaceClass[] = L"{e6327cad-dcec-4949-ae8a-991e976a79d2}";

//	An off-format is the all-zero subtype, which is also what a device with no spatial support reports.
static const GUID kSpatialOff = {};

struct SpatialFormat
{
	const wchar_t* name;
	GUID subtype;
};

//	Populated once from the SpatialAudioFormatSubtype statics rather than hard-coded, so the mod picks up whatever the running build of
//	Windows actually calls these. Index 0 is always "Off".
static std::vector<SpatialFormat> g_spatialFormats;
static std::vector<std::wstring> g_spatialFormatNames;
static INIT_ONCE g_spatialInitOnce = INIT_ONCE_STATIC_INIT;

class HString
{
public:
	HString() = default;

	explicit HString(const std::wstring& value)
	{
		WindowsCreateString(value.c_str(), static_cast<UINT32>(value.size()), &m_handle);
	}

	~HString()
	{
		Reset();
	}

	HString(const HString&) = delete;
	HString& operator=(const HString&) = delete;

	void Reset()
	{
		if (m_handle)
		{
			WindowsDeleteString(m_handle);
			m_handle = nullptr;
		}
	}

	HSTRING Get() const
	{
		return m_handle;
	}

	HSTRING* Receive()
	{
		Reset();
		return &m_handle;
	}

	std::wstring ToString() const
	{
		UINT32 length = 0;
		PCWSTR raw = WindowsGetStringRawBuffer(m_handle, &length);
		return raw ? std::wstring(raw, length) : std::wstring();
	}

private:
	HSTRING m_handle = nullptr;
};

static std::wstring GuidToString(const GUID& value)
{
	wchar_t buffer[64] = {};
	StringFromGUID2(value, buffer, ARRAYSIZE(buffer));
	return buffer;
}

static bool StringToGuid(const std::wstring& text, GUID* out)
{
	return !text.empty() && SUCCEEDED(CLSIDFromString(text.c_str(), out));
}

template <typename T>
static HRESULT GetActivationFactory(PCWSTR className, REFIID iid, T** out)
{
	HString name(className);
	if (!name.Get())
	{
		return E_OUTOFMEMORY;
	}

	return RoGetActivationFactory(name.Get(), iid, reinterpret_cast<void**>(out));
}

static BOOL CALLBACK InitSpatialFormats(PINIT_ONCE, PVOID, PVOID*)
{
	g_spatialFormats.push_back({ L"Off", kSpatialOff });

	ComPtr<ISpatialAudioFormatSubtypeStatics> statics;
	if (FAILED(GetActivationFactory(L"Windows.Media.Audio.SpatialAudioFormatSubtype", kIidSpatialSubtypeStatics, &statics)) || !statics)
	{
		return TRUE;
	}

	struct Entry
	{
		const wchar_t* name;
		HRESULT (STDMETHODCALLTYPE ISpatialAudioFormatSubtypeStatics::*getter)(HSTRING*);
	};

	const Entry entries[] = {
		{ L"Windows Sonic", &ISpatialAudioFormatSubtypeStatics::get_WindowsSonic },
		{ L"Dolby Atmos (headphones)", &ISpatialAudioFormatSubtypeStatics::get_DolbyAtmosForHeadphones },
		{ L"Dolby Atmos (home theater)", &ISpatialAudioFormatSubtypeStatics::get_DolbyAtmosForHomeTheater },
		{ L"Dolby Atmos (speakers)", &ISpatialAudioFormatSubtypeStatics::get_DolbyAtmosForSpeakers },
		{ L"DTS Headphone:X", &ISpatialAudioFormatSubtypeStatics::get_DTSHeadphoneX },
		{ L"DTS:X Ultra", &ISpatialAudioFormatSubtypeStatics::get_DTSXUltra },
	};

	for (size_t i = 0; i < ARRAYSIZE(entries); i++)
	{
		HString value;
		GUID subtype = {};

		if (SUCCEEDED((statics.Get()->*entries[i].getter)(value.Receive())) && StringToGuid(value.ToString(), &subtype))
		{
			g_spatialFormats.push_back({ entries[i].name, subtype });
		}
	}

	//	DTS:X for Home Theater arrived in a later revision of the class and lives on a separate statics interface, so its absence is
	//	not an error.
	ComPtr<ISpatialAudioFormatSubtypeStatics2> statics2;
	if (SUCCEEDED(GetActivationFactory(L"Windows.Media.Audio.SpatialAudioFormatSubtype", kIidSpatialSubtypeStatics2, &statics2)) && statics2)
	{
		HString value;
		GUID subtype = {};

		if (SUCCEEDED(statics2->get_DTSXForHomeTheater(value.Receive())) && StringToGuid(value.ToString(), &subtype))
		{
			g_spatialFormats.push_back({ L"DTS:X (home theater)", subtype });
		}
	}

	return TRUE;
}

static const std::vector<SpatialFormat>& SpatialFormats()
{
	InitOnceExecuteOnce(&g_spatialInitOnce, InitSpatialFormats, nullptr, nullptr);
	return g_spatialFormats;
}

static size_t FindSpatialFormat(const GUID& subtype)
{
	const std::vector<SpatialFormat>& formats = SpatialFormats();
	for (size_t i = 0; i < formats.size(); i++)
	{
		if (IsEqualGUID(formats[i].subtype, subtype))
		{
			return i;
		}
	}

	return static_cast<size_t>(-1);
}

static std::wstring DescribeSpatial(const GUID& subtype)
{
	const size_t index = FindSpatialFormat(subtype);
	if (index != static_cast<size_t>(-1))
	{
		return SpatialFormats()[index].name;
	}

	return GuidToString(subtype);
}

//	Wraps one endpoint's spatial configuration. Cheap enough to construct per use, so nothing holds one open across a device change.
class SpatialConfig
{
public:
	bool Open(const std::wstring& endpointId)
	{
		m_config.Reset();

		ComPtr<ISpatialAudioDeviceConfigurationStatics> statics;
		if (FAILED(GetActivationFactory(L"Windows.Media.Audio.SpatialAudioDeviceConfiguration", kIidSpatialDeviceConfigStatics, &statics)) || !statics)
		{
			return false;
		}

		const HString deviceId(L"\\\\?\\SWD#MMDEVAPI#" + endpointId + L"#" + kMMDeviceInterfaceClass);
		return SUCCEEDED(statics->GetForDeviceId(deviceId.Get(), &m_config)) && m_config;
	}

	bool IsAvailable() const
	{
		if (!m_config)
		{
			return false;
		}

		boolean supported = 0;
		return SUCCEEDED(m_config->get_IsSpatialAudioSupported(&supported)) && supported;
	}

	bool Read(GUID* out) const
	{
		if (!m_config)
		{
			return false;
		}

		HString value;
		return SUCCEEDED(m_config->get_DefaultSpatialAudioFormat(value.Receive())) && StringToGuid(value.ToString(), out);
	}

	bool IsSupported(const GUID& subtype) const
	{
		if (!m_config)
		{
			return false;
		}

		//	Off is always selectable and is not a subtype the API knows about.
		if (IsEqualGUID(subtype, kSpatialOff))
		{
			return true;
		}

		const HString name(GuidToString(subtype));
		boolean supported = 0;
		return SUCCEEDED(m_config->IsSpatialAudioFormatSupported(name.Get(), &supported)) && supported;
	}

	//	Returns true only if the format is in effect afterwards. An unlicensed format completes without error and changes nothing, so
	//	the read-back is the actual result rather than a sanity check.
	bool Apply(const GUID& subtype)
	{
		if (!m_config)
		{
			return false;
		}

		const HString name(GuidToString(subtype));
		ComPtr<IInspectable> operation;
		if (FAILED(m_config->SetDefaultSpatialAudioFormatAsync(name.Get(), &operation)) || !operation)
		{
			return false;
		}

		ComPtr<ISpatialAsyncInfo> info;
		if (SUCCEEDED(operation->QueryInterface(kIidSpatialAsyncInfo, reinterpret_cast<void**>(&info))) && info)
		{
			for (int i = 0; i < kSpatialWaitSteps; i++)
			{
				int status = 0;
				if (FAILED(info->get_Status(&status)) || status != 0)
				{
					break;
				}

				Sleep(kSpatialWaitStepMs);
			}
		}

		GUID applied = {};
		return Read(&applied) && IsEqualGUID(applied, subtype);
	}

private:
	static constexpr int kSpatialWaitSteps = 80;
	static constexpr DWORD kSpatialWaitStepMs = 25;

	ComPtr<ISpatialAudioDeviceConfiguration> m_config;
};

// ====================================================================================================
//	Endpoint enumeration and capability probing
// ====================================================================================================

struct Endpoint
{
	std::wstring id;
	std::wstring friendlyName;
	DWORD state = DEVICE_STATE_NOTPRESENT;
	bool isDefault = false;

	FormatSpec current;
	bool hasCurrent = false;

	DWORD physicalSpeakers = 0;
	bool hasPhysicalSpeakers = false;

	GUID spatialFormat = {};
	bool hasSpatial = false;

	//	Which spatial formats this endpoint reports, indexed in step with SpatialFormats(). Only filled in when capabilities are probed.
	std::vector<unsigned char> spatialSupported;

	bool IsSpatialSupported(size_t format) const
	{
		return format < spatialSupported.size() && spatialSupported[format] != 0;
	}

	//	Indexed by (config * kDepthOptionCount + depth) * kSampleRateCount + rate.
	std::vector<unsigned char> supported;

	bool IsSupported(size_t config, size_t depth, size_t rate) const
	{
		if (supported.empty())
		{
			return false;
		}

		return supported[(config * kDepthOptionCount + depth) * kSampleRateCount + rate] != 0;
	}
};

static std::wstring ReadStringProperty(IPropertyStore* store, const PROPERTYKEY& key)
{
	if (!store)
	{
		return std::wstring();
	}

	PropVariant value;
	if (FAILED(store->GetValue(key, &value)) || value.Get().vt != VT_LPWSTR || !value.Get().pwszVal)
	{
		return std::wstring();
	}

	return value.Get().pwszVal;
}

//	Probing is not free in the way a query normally is. Asking an HDMI or S/PDIF endpoint about an exclusive-mode format makes the
//	driver interrogate the link, and a receiver on the other end can respond by re-negotiating it: an audible relay click, and a brief
//	drop-out. Several hundred queries in a row turns that into a machine gun, so results are cached for the life of the process and an
//	endpoint is probed at most once.
//
//	This is keyed by endpoint id and never invalidated. What a device is physically capable of does not change while it is plugged in,
//	and an endpoint that is unplugged and returns is a different object that gets probed again anyway.
struct CapabilityCacheEntry
{
	std::wstring id;
	std::vector<unsigned char> supported;
	std::vector<unsigned char> spatialSupported;
	bool hasSpatial = false;
};

static std::vector<CapabilityCacheEntry> g_capabilityCache;
static CRITICAL_SECTION g_capabilityLock;
static bool g_capabilityLockReady = false;

class CapabilityLock
{
public:
	CapabilityLock()
	{
		if (g_capabilityLockReady)
		{
			EnterCriticalSection(&g_capabilityLock);
		}
	}

	~CapabilityLock()
	{
		if (g_capabilityLockReady)
		{
			LeaveCriticalSection(&g_capabilityLock);
		}
	}

	CapabilityLock(const CapabilityLock&) = delete;
	CapabilityLock& operator=(const CapabilityLock&) = delete;
};

static bool LoadCachedCapabilities(const std::wstring& id, Endpoint* endpoint)
{
	CapabilityLock lock;

	for (size_t i = 0; i < g_capabilityCache.size(); i++)
	{
		if (g_capabilityCache[i].id == id)
		{
			endpoint->supported = g_capabilityCache[i].supported;
			endpoint->spatialSupported = g_capabilityCache[i].spatialSupported;
			return true;
		}
	}

	return false;
}

static void StoreCachedCapabilities(const Endpoint& endpoint)
{
	CapabilityLock lock;

	for (size_t i = 0; i < g_capabilityCache.size(); i++)
	{
		if (g_capabilityCache[i].id == endpoint.id)
		{
			g_capabilityCache[i].supported = endpoint.supported;
			g_capabilityCache[i].spatialSupported = endpoint.spatialSupported;
			return;
		}
	}

	CapabilityCacheEntry entry;
	entry.id = endpoint.id;
	entry.supported = endpoint.supported;
	entry.spatialSupported = endpoint.spatialSupported;
	g_capabilityCache.push_back(entry);
}

//	Shared mode is useless for this: IsFormatSupported only ever echoes back the format the engine is currently configured for.
//	Exclusive mode asks the driver what the hardware can really do, and matches the Sound control panel's dropdown exactly.
static void ProbeCapabilities(IMMDevice* device, Endpoint* endpoint)
{
	ComPtr<IAudioClient> client;
	if (FAILED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client))))
	{
		return;
	}

	endpoint->supported.assign(kSpeakerConfigCount * kDepthOptionCount * kSampleRateCount, 0);

	for (size_t c = 0; c < kSpeakerConfigCount; c++)
	{
		for (size_t d = 0; d < kDepthOptionCount; d++)
		{
			for (size_t r = 0; r < kSampleRateCount; r++)
			{
				FormatSpec spec;
				spec.channelMask = kSpeakerConfigs[c].mask;
				spec.channels = kSpeakerConfigs[c].channels;
				spec.bitsPerSample = kDepthOptions[d].bitsPerSample;
				spec.containerBits = kDepthOptions[d].containerBits;
				spec.sampleRate = kSampleRates[r];

				WAVEFORMATEXTENSIBLE wf;
				BuildWaveFormat(spec, &wf);

				if (client->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, &wf.Format, nullptr) == S_OK)
				{
					endpoint->supported[(c * kDepthOptionCount + d) * kSampleRateCount + r] = 1;
				}
			}
		}
	}
}

static HRESULT EnumerateEndpoints(bool probeCapabilities, bool readSpatial, std::vector<Endpoint>* out)
{
	ComPtr<IMMDeviceEnumerator> enumerator;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
	if (FAILED(hr))
	{
		return hr;
	}

	std::wstring defaultId;
	{
		ComPtr<IMMDevice> defaultDevice;
		if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice)))
		{
			LPWSTR id = nullptr;
			if (SUCCEEDED(defaultDevice->GetId(&id)) && id)
			{
				defaultId = id;
				CoTaskMemFree(id);
			}
		}
	}

	//	DEVICE_STATEMASK_ALL also returns every endpoint Windows has ever seen, which on a machine with several GPUs and monitors runs
	//	to dozens of stale duplicates that can never be selected. Devices that are merely off or unplugged are still worth tracking,
	//	so only DEVICE_STATE_NOTPRESENT is excluded.
	const DWORD kStateMask = DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED | DEVICE_STATE_UNPLUGGED;

	ComPtr<IMMDeviceCollection> collection;
	hr = enumerator->EnumAudioEndpoints(eRender, kStateMask, &collection);
	if (FAILED(hr))
	{
		return hr;
	}

	UINT count = 0;
	hr = collection->GetCount(&count);
	if (FAILED(hr))
	{
		return hr;
	}

	for (UINT i = 0; i < count; i++)
	{
		ComPtr<IMMDevice> device;
		if (FAILED(collection->Item(i, &device)))
		{
			continue;
		}

		Endpoint endpoint;

		LPWSTR id = nullptr;
		if (FAILED(device->GetId(&id)) || !id)
		{
			continue;
		}

		endpoint.id = id;
		CoTaskMemFree(id);

		device->GetState(&endpoint.state);
		endpoint.isDefault = (!defaultId.empty() && endpoint.id == defaultId);

		ComPtr<IPropertyStore> store;
		if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &store)))
		{
			endpoint.friendlyName = ReadStringProperty(store.Get(), PKEY_Device_FriendlyName);

			PropVariant format;
			if (SUCCEEDED(store->GetValue(kPkeyDeviceFormat, &format)) && format.Get().vt == VT_BLOB && format.Get().blob.pBlobData && format.Get().blob.cbSize >= sizeof(WAVEFORMATEX))
			{
				endpoint.hasCurrent = ParseWaveFormat(reinterpret_cast<const WAVEFORMATEX*>(format.Get().blob.pBlobData), &endpoint.current);
			}

			PropVariant speakers;
			if (SUCCEEDED(store->GetValue(kPkeyPhysicalSpeakers, &speakers)) && speakers.Get().vt == VT_UI4)
			{
				endpoint.physicalSpeakers = speakers.Get().ulVal;
				endpoint.hasPhysicalSpeakers = true;
			}
		}

		//	Only active devices can be opened, so probing anything else just produces a wall of failures.
		if (probeCapabilities && endpoint.state == DEVICE_STATE_ACTIVE && !LoadCachedCapabilities(endpoint.id, &endpoint))
		{
			ProbeCapabilities(device.Get(), &endpoint);

			SpatialConfig spatial;
			if (spatial.Open(endpoint.id) && spatial.IsAvailable())
			{
				const std::vector<SpatialFormat>& formats = SpatialFormats();
				endpoint.spatialSupported.assign(formats.size(), 0);

				for (size_t f = 0; f < formats.size(); f++)
				{
					endpoint.spatialSupported[f] = spatial.IsSupported(formats[f].subtype) ? 1 : 0;
				}
			}

			StoreCachedCapabilities(endpoint);
		}

		//	Reading the current spatial format is a cheap property read, unlike the support query above, but it still activates a WinRT
		//	object per endpoint. The watcher does not need it on every notification, so it is only read when asked for.
		if (readSpatial && endpoint.state == DEVICE_STATE_ACTIVE)
		{
			SpatialConfig spatial;
			if (spatial.Open(endpoint.id))
			{
				endpoint.hasSpatial = spatial.Read(&endpoint.spatialFormat);
			}
		}

		out->push_back(std::move(endpoint));
	}

	return S_OK;
}

// ====================================================================================================
//	Writing settings back
// ====================================================================================================

enum class WriteSupport
{
	Untested,
	Enabled,
	Unavailable,
	LayoutMismatch,
};

class PolicyConfigWriter
{
public:
	//	IPolicyConfig is undocumented, so the vtable layout is confirmed before it is trusted: GetDeviceFormat is called for every
	//	endpoint and the result is compared against the same format read through the documented IPropertyStore path. If the layout
	//	ever shifts, the two disagree and every write is disabled rather than risking a corrupted call.
	WriteSupport Validate(const std::vector<Endpoint>& endpoints)
	{
		m_support = WriteSupport::Untested;
		m_policy.Reset();

		if (FAILED(CoCreateInstance(kClsidPolicyConfigClient, nullptr, CLSCTX_ALL, kIidPolicyConfig, reinterpret_cast<void**>(&m_policy))) || !m_policy)
		{
			m_support = WriteSupport::Unavailable;
			return m_support;
		}

		int checked = 0;

		for (size_t i = 0; i < endpoints.size(); i++)
		{
			const Endpoint& endpoint = endpoints[i];

			//	Stale endpoints left behind by long-gone hardware report whatever was last cached for them, which is not a fair
			//	comparison. Only devices that are actually present can confirm the layout.
			if (!endpoint.hasCurrent || endpoint.state != DEVICE_STATE_ACTIVE)
			{
				continue;
			}

			//	Passing TRUE here asks for the driver's factory default rather than the format currently in effect, which would make
			//	every comparison below fail. Only FALSE is a like-for-like match against what IPropertyStore reports.
			WAVEFORMATEX* wfx = nullptr;
			if (FAILED(m_policy->GetDeviceFormat(endpoint.id.c_str(), FALSE, &wfx)) || !wfx)
			{
				continue;
			}

			FormatSpec spec;
			const bool parsed = ParseWaveFormat(wfx, &spec);
			CoTaskMemFree(wfx);

			if (!parsed)
			{
				continue;
			}

			if (spec.channels != endpoint.current.channels || spec.sampleRate != endpoint.current.sampleRate || spec.containerBits != endpoint.current.containerBits ||
				spec.channelMask != endpoint.current.channelMask)
			{
				Wh_Log(L"Vtable check FAILED for %s", endpoint.friendlyName.c_str());
				Wh_Log(L"  IPolicyConfig: %s", DescribeFormat(spec).c_str());
				Wh_Log(L"  IPropertyStore: %s", DescribeFormat(endpoint.current).c_str());

				m_policy.Reset();
				m_support = WriteSupport::LayoutMismatch;
				return m_support;
			}

			checked++;
		}

		if (checked == 0)
		{
			m_policy.Reset();
			m_support = WriteSupport::Unavailable;
			return m_support;
		}

		Wh_Log(L"Vtable check passed on %d endpoint(s)", checked);
		m_support = WriteSupport::Enabled;
		return m_support;
	}

	WriteSupport Support() const
	{
		return m_support;
	}

	//	The endpoint format is PCM and the mix format is the same layout in float32, which is what the audio engine runs internally.
	//	The channel mask carried in the device format is what actually drives surround output; PhysicalSpeakers is written only so the
	//	legacy Configure wizard shows the matching layout, and a failure there is deliberately not fatal.
	HRESULT Apply(const std::wstring& deviceId, const FormatSpec& spec)
	{
		if (m_support != WriteSupport::Enabled || !m_policy)
		{
			return E_FAIL;
		}

		FormatSpec mixSpec = spec;
		mixSpec.isFloat = true;
		mixSpec.bitsPerSample = 32;
		mixSpec.containerBits = 32;

		WAVEFORMATEXTENSIBLE endpointFormat;
		WAVEFORMATEXTENSIBLE mixFormat;
		BuildWaveFormat(spec, &endpointFormat);
		BuildWaveFormat(mixSpec, &mixFormat);

		HRESULT hr = m_policy->SetDeviceFormat(deviceId.c_str(), &endpointFormat.Format, &mixFormat.Format);
		if (FAILED(hr))
		{
			return hr;
		}

		PropVariant speakers;
		PROPVARIANT* raw = &speakers;
		raw->vt = VT_UI4;
		raw->ulVal = spec.channelMask;

		HRESULT speakersHr = m_policy->SetPropertyValue(deviceId.c_str(), FALSE, kPkeyPhysicalSpeakers, raw);
		if (FAILED(speakersHr))
		{
			Wh_Log(L"PhysicalSpeakers write failed (0x%08X), format was still applied", static_cast<unsigned>(speakersHr));
		}

		return S_OK;
	}

	HRESULT SetDefaultEndpoint(const std::wstring& deviceId)
	{
		if (m_support != WriteSupport::Enabled || !m_policy)
		{
			return E_FAIL;
		}

		//	All three roles are set so the device becomes default for playback and communications alike, matching what the Sound
		//	control panel does when a device is made default.
		HRESULT hr = m_policy->SetDefaultEndpoint(deviceId.c_str(), eConsole);
		if (FAILED(hr))
		{
			return hr;
		}

		m_policy->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
		m_policy->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
		return S_OK;
	}

private:
	ComPtr<IPolicyConfig> m_policy;
	WriteSupport m_support = WriteSupport::Untested;
};

// ====================================================================================================
//	Profile store
//
//	One record per endpoint, keyed by the endpoint's device id rather than its friendly name. Names are emphatically not unique: this
//	machine reports several distinct endpoints all called "AV Receiver (NVIDIA High Definition Audio)", left behind by past GPU and
//	monitor combinations. The id is stable across reboots, driver updates and the device disappearing and coming back.
// ====================================================================================================

constexpr uint32_t kProfileVersion = 2;

constexpr uint16_t kProfileFlagHasSpeakers = 0x1;

//	Marks a profile the user chose from the panel, as opposed to one the mod simply observed. Only observed profiles are ever pruned,
//	so a deliberate choice is never thrown away no matter how long the device stays away.
constexpr uint16_t kProfileFlagUserSet = 0x2;

//	Distinguishes "this device has no spatial format" from "the spatial format was never recorded", which matters because the former
//	has to be restored just like any other value.
constexpr uint16_t kProfileFlagHasSpatial = 0x4;

struct StoredProfile
{
	uint32_t version;
	uint32_t channelMask;
	uint32_t sampleRate;
	uint32_t physicalSpeakers;
	uint16_t channels;
	uint16_t bitsPerSample;
	uint16_t containerBits;
	uint16_t flags;
	GUID spatialFormat;
	wchar_t friendlyName[128];
};

//	Kept for the settings UI, which needs to show remembered devices that are not currently plugged in and therefore never appear in
//	an endpoint enumeration.
const wchar_t kKnownEndpointsValue[] = L"knownEndpoints";
const wchar_t kPreferredDefaultValue[] = L"preferredDefault";

static std::wstring ProfileValueName(const std::wstring& endpointId)
{
	return L"profile:" + endpointId;
}

static FormatSpec ProfileToFormat(const StoredProfile& profile)
{
	FormatSpec spec;
	spec.channelMask = profile.channelMask;
	spec.channels = profile.channels;
	spec.sampleRate = profile.sampleRate;
	spec.bitsPerSample = profile.bitsPerSample;
	spec.containerBits = profile.containerBits;
	spec.isFloat = false;
	return spec;
}

static bool LoadProfile(const std::wstring& endpointId, StoredProfile* out)
{
	StoredProfile raw = {};
	const std::wstring name = ProfileValueName(endpointId);

	if (Wh_GetBinaryValue(name.c_str(), &raw, sizeof(raw)) != sizeof(raw))
	{
		return false;
	}

	//	A record written by a future version of the mod is ignored rather than misread, which would apply a garbage format.
	if (raw.version != kProfileVersion)
	{
		return false;
	}

	*out = raw;
	return true;
}

static std::vector<std::wstring> LoadKnownEndpoints()
{
	std::vector<std::wstring> ids;

	//	Stored as one newline-separated string rather than an indexed set of values, so the whole list can be read and rewritten
	//	atomically without leaving orphaned entries behind.
	std::vector<wchar_t> buffer(32768);
	if (Wh_GetStringValue(kKnownEndpointsValue, buffer.data(), buffer.size()) == 0)
	{
		return ids;
	}

	std::wstring current;
	for (const wchar_t* p = buffer.data(); *p; p++)
	{
		if (*p == L'\n')
		{
			if (!current.empty())
			{
				ids.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += *p;
		}
	}

	if (!current.empty())
	{
		ids.push_back(current);
	}

	return ids;
}

static void SaveKnownEndpoints(const std::vector<std::wstring>& ids)
{
	std::wstring joined;
	for (size_t i = 0; i < ids.size(); i++)
	{
		if (i > 0)
		{
			joined += L'\n';
		}

		joined += ids[i];
	}

	Wh_SetStringValue(kKnownEndpointsValue, joined.c_str());
}

static void RememberEndpoint(const std::wstring& endpointId)
{
	std::vector<std::wstring> ids = LoadKnownEndpoints();

	for (size_t i = 0; i < ids.size(); i++)
	{
		if (ids[i] == endpointId)
		{
			return;
		}
	}

	ids.push_back(endpointId);
	SaveKnownEndpoints(ids);
}

static void ForgetEndpoint(const std::wstring& endpointId)
{
	const std::wstring name = ProfileValueName(endpointId);
	Wh_DeleteValue(name.c_str());

	std::vector<std::wstring> ids = LoadKnownEndpoints();
	std::vector<std::wstring> remaining;

	for (size_t i = 0; i < ids.size(); i++)
	{
		if (ids[i] != endpointId)
		{
			remaining.push_back(ids[i]);
		}
	}

	SaveKnownEndpoints(remaining);
}

//	Gathered into a struct rather than passed positionally: there are enough optional pieces now that a call site full of bare bools
//	would be unreadable and easy to get wrong.
struct ProfileUpdate
{
	std::wstring endpointId;
	std::wstring friendlyName;
	FormatSpec spec;
	bool hasSpeakers = false;
	DWORD physicalSpeakers = 0;
	bool hasSpatial = false;
	GUID spatialFormat = {};
	bool userSet = false;
};

static bool SaveProfile(const ProfileUpdate& update)
{
	StoredProfile profile = {};
	profile.version = kProfileVersion;
	profile.channelMask = update.spec.channelMask;
	profile.sampleRate = update.spec.sampleRate;
	profile.physicalSpeakers = update.hasSpeakers ? update.physicalSpeakers : update.spec.channelMask;
	profile.channels = update.spec.channels;
	profile.bitsPerSample = update.spec.bitsPerSample;
	profile.containerBits = update.spec.containerBits;
	profile.spatialFormat = update.spatialFormat;
	profile.flags = static_cast<uint16_t>((update.hasSpeakers ? kProfileFlagHasSpeakers : 0) | (update.userSet ? kProfileFlagUserSet : 0) | (update.hasSpatial ? kProfileFlagHasSpatial : 0));

	StoredProfile existing = {};
	const bool hadExisting = LoadProfile(update.endpointId, &existing);

	//	A profile the user picked must not be demoted to an observed one by a later automatic save.
	if (!update.userSet && hadExisting && (existing.flags & kProfileFlagUserSet) != 0)
	{
		profile.flags = static_cast<uint16_t>(profile.flags | kProfileFlagUserSet);
	}

	//	A device that could not be queried for its spatial format keeps whatever was recorded last, rather than losing it because the
	//	endpoint happened to be busy at the moment it was read.
	if (!update.hasSpatial && hadExisting && (existing.flags & kProfileFlagHasSpatial) != 0)
	{
		profile.spatialFormat = existing.spatialFormat;
		profile.flags = static_cast<uint16_t>(profile.flags | kProfileFlagHasSpatial);
	}

	//	Truncating is fine: the name is only ever used for display, never for lookup.
	const size_t copyChars = (update.friendlyName.size() < ARRAYSIZE(profile.friendlyName) - 1) ? update.friendlyName.size() : ARRAYSIZE(profile.friendlyName) - 1;
	wmemcpy(profile.friendlyName, update.friendlyName.c_str(), copyChars);
	profile.friendlyName[copyChars] = L'\0';

	const std::wstring name = ProfileValueName(update.endpointId);
	if (!Wh_SetBinaryValue(name.c_str(), &profile, sizeof(profile)))
	{
		return false;
	}

	RememberEndpoint(update.endpointId);
	return true;
}

//	The common case: record exactly what an endpoint is currently set to.
static ProfileUpdate ProfileUpdateFrom(const Endpoint& endpoint)
{
	ProfileUpdate update;
	update.endpointId = endpoint.id;
	update.friendlyName = endpoint.friendlyName;
	update.spec = endpoint.current;
	update.hasSpeakers = endpoint.hasPhysicalSpeakers;
	update.physicalSpeakers = endpoint.physicalSpeakers;
	update.hasSpatial = endpoint.hasSpatial;
	update.spatialFormat = endpoint.spatialFormat;
	return update;
}

static std::wstring LoadPreferredDefault()
{
	std::vector<wchar_t> buffer(512);
	if (Wh_GetStringValue(kPreferredDefaultValue, buffer.data(), buffer.size()) == 0)
	{
		return std::wstring();
	}

	return buffer.data();
}

static void SavePreferredDefault(const std::wstring& endpointId)
{
	Wh_SetStringValue(kPreferredDefaultValue, endpointId.c_str());
}

//	Some hardware presents a different endpoint depending on its own state. This machine's AV receiver is one: powered off, the GPU
//	publishes a separate stereo-only "AV Receiver" endpoint, which the mod observes and remembers like any other device. Those records
//	are noise, so once the real device is back an observed record is dropped if a device of the same name is active under another id.
//
//	Only observed records are eligible. A profile the user picked, or the preferred default, is never removed, and neither is anything
//	belonging to a device that is merely unplugged, since remembering those is the entire point of the mod.
static void PruneProfiles(const std::vector<Endpoint>& endpoints)
{
	const std::vector<std::wstring> known = LoadKnownEndpoints();
	const std::wstring preferred = LoadPreferredDefault();

	for (size_t i = 0; i < known.size(); i++)
	{
		bool present = false;
		for (size_t e = 0; e < endpoints.size() && !present; e++)
		{
			present = endpoints[e].id == known[i];
		}

		if (present || known[i] == preferred)
		{
			continue;
		}

		StoredProfile profile;
		if (!LoadProfile(known[i], &profile) || (profile.flags & kProfileFlagUserSet) != 0)
		{
			continue;
		}

		for (size_t e = 0; e < endpoints.size(); e++)
		{
			if (endpoints[e].state == DEVICE_STATE_ACTIVE && endpoints[e].friendlyName == profile.friendlyName)
			{
				Wh_Log(L"Forgetting stale profile for %s", profile.friendlyName);
				ForgetEndpoint(known[i]);
				break;
			}
		}
	}
}

// ====================================================================================================
//	Logging
// ====================================================================================================

static void LogCapabilities(const Endpoint& endpoint)
{
	for (size_t c = 0; c < kSpeakerConfigCount; c++)
	{
		for (size_t d = 0; d < kDepthOptionCount; d++)
		{
			std::wstring rates;

			for (size_t r = 0; r < kSampleRateCount; r++)
			{
				if (!endpoint.IsSupported(c, d, r))
				{
					continue;
				}

				wchar_t buf[24];
				swprintf(buf, ARRAYSIZE(buf), L"%u", static_cast<unsigned>(kSampleRates[r]));

				if (!rates.empty())
				{
					rates += L", ";
				}

				rates += buf;
			}

			if (!rates.empty())
			{
				Wh_Log(L"    %-20s %-26s %s", kSpeakerConfigs[c].name, kDepthOptions[d].name, rates.c_str());
			}
		}
	}
}

static void LogEndpoints(const std::vector<Endpoint>& endpoints, bool logCapabilities)
{
	Wh_Log(L"Found %u render endpoint(s)", static_cast<unsigned>(endpoints.size()));

	for (size_t i = 0; i < endpoints.size(); i++)
	{
		const Endpoint& endpoint = endpoints[i];

		Wh_Log(L"  %s %s [%s]", endpoint.isDefault ? L"*" : L"-", endpoint.friendlyName.c_str(), DescribeState(endpoint.state));

		if (endpoint.hasCurrent)
		{
			Wh_Log(L"    format: %s", DescribeFormat(endpoint.current).c_str());
			Wh_Log(L"    layout: %s", DescribeSpeakerMask(endpoint.current.channelMask).c_str());
		}

		if (endpoint.hasPhysicalSpeakers)
		{
			Wh_Log(L"    speaker config: %s (0x%X)", DescribeSpeakerMask(endpoint.physicalSpeakers).c_str(), static_cast<unsigned>(endpoint.physicalSpeakers));
		}
		else
		{
			//	This is the fingerprint of the bug the mod exists to fix: Windows dropped the speaker layout entirely.
			Wh_Log(L"    speaker config: not set");
		}

		if (logCapabilities && !endpoint.supported.empty())
		{
			LogCapabilities(endpoint);
		}
	}
}

// ====================================================================================================
//	Mod plumbing
// ====================================================================================================

struct Settings
{
	bool logCapabilities = false;
	bool enforceProfiles = true;
	bool restoreSpatial = true;
	bool restorePreferredDefault = true;
	bool lockUserSettings = false;
	bool skipInRemoteSession = true;
};

static Settings g_settings;
static HANDLE g_workerThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_changeEvent = nullptr;

//	Windows does not rewrite an endpoint's format the moment a device appears; the audio service does it a beat later. Reacting
//	immediately would just get overwritten, so each burst of notifications is allowed to settle before anything is applied.
constexpr DWORD kSettleMs = 1500;

//	How long after a device appears its stored profile is enforced. Inside this window a format change is treated as Windows resetting
//	the device and is reverted; outside it, the same change is treated as the user deliberately reconfiguring the device and is learned.
//	This single distinction is what separates "fix the bug" from "fight the user".
constexpr ULONGLONG kEnforceWindowMs = 15000;

struct DeviceState
{
	std::wstring id;
	bool wasActive = false;
	ULONGLONG enforceUntil = 0;

	//	A spatial format that is reported as supported but refuses to take, because the machine has no license for it, would otherwise
	//	be retried on every notification forever. Each attempt re-negotiates the link, which on a receiver is an audible relay click,
	//	so a format that has already failed for this device is not tried again.
	GUID spatialRefused = {};
	bool hasSpatialRefused = false;
};

static std::vector<DeviceState> g_deviceStates;

//	The watcher thread and the panel's UI thread both read the device states and write to the profile store, so both go through this.
//	Holding it across a whole reconcile pass is deliberate: a pass must not observe a half-applied change made from the panel.
static CRITICAL_SECTION g_stateLock;
static bool g_stateLockReady = false;

class StateLock
{
public:
	StateLock()
	{
		if (g_stateLockReady)
		{
			EnterCriticalSection(&g_stateLock);
		}
	}

	~StateLock()
	{
		if (g_stateLockReady)
		{
			LeaveCriticalSection(&g_stateLock);
		}
	}

	StateLock(const StateLock&) = delete;
	StateLock& operator=(const StateLock&) = delete;
};

static size_t EnsureDeviceState(const std::wstring& id)
{
	for (size_t i = 0; i < g_deviceStates.size(); i++)
	{
		if (g_deviceStates[i].id == id)
		{
			return i;
		}
	}

	DeviceState state;
	state.id = id;
	g_deviceStates.push_back(state);
	return g_deviceStates.size() - 1;
}

//	Called when a change comes from the panel. The user has just said what they want, so the arrival window must not be allowed to
//	revert it a moment later.
static void ClearEnforceWindow(const std::wstring& id)
{
	g_deviceStates[EnsureDeviceState(id)].enforceUntil = 0;
}

static bool FormatMatches(const FormatSpec& a, const FormatSpec& b)
{
	return a.channels == b.channels && a.sampleRate == b.sampleRate && a.containerBits == b.containerBits && a.bitsPerSample == b.bitsPerSample && a.channelMask == b.channelMask;
}

// ====================================================================================================
//	Device watcher
// ====================================================================================================

//	Every callback does nothing except wake the worker. These are delivered on a thread owned by the audio service, and blocking one
//	stalls the audio stack for every process on the machine, so no real work may happen here.
//
//	Applying a format itself raises OnPropertyValueChanged, so the mod hears its own writes. That is deliberately not guarded with a
//	re-entrancy flag: the reconcile pass only writes when the current format differs from the stored one, so a self-triggered pass
//	finds everything already correct and writes nothing. Idempotence, rather than a flag, is what stops the oscillation.
class DeviceWatcher : public IMMNotificationClient
{
public:
	virtual ~DeviceWatcher() = default;

	ULONG STDMETHODCALLTYPE AddRef() override
	{
		return InterlockedIncrement(&m_refCount);
	}

	ULONG STDMETHODCALLTYPE Release() override
	{
		const LONG remaining = InterlockedDecrement(&m_refCount);
		if (remaining == 0)
		{
			delete this;
		}

		return remaining;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
	{
		if (!ppvObject)
		{
			return E_POINTER;
		}

		if (IsEqualIID(riid, __uuidof(IUnknown)) || IsEqualIID(riid, __uuidof(IMMNotificationClient)))
		{
			*ppvObject = static_cast<IMMNotificationClient*>(this);
			AddRef();
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR deviceId, DWORD newState) override
	{
		(void)deviceId;
		(void)newState;
		Wake();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR deviceId) override
	{
		(void)deviceId;
		Wake();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR deviceId) override
	{
		(void)deviceId;
		Wake();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR deviceId) override
	{
		(void)flow;
		(void)role;
		(void)deviceId;
		Wake();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR deviceId, const PROPERTYKEY key) override
	{
		(void)deviceId;
		(void)key;
		Wake();
		return S_OK;
	}

private:
	static void Wake()
	{
		if (g_changeEvent)
		{
			SetEvent(g_changeEvent);
		}
	}

	LONG m_refCount = 1;
};

// ====================================================================================================
//	Reconciliation
// ====================================================================================================

static void LogProfiles()
{
	const std::vector<std::wstring> ids = LoadKnownEndpoints();
	const std::wstring preferred = LoadPreferredDefault();

	Wh_Log(L"Remembered %u profile(s)", static_cast<unsigned>(ids.size()));

	for (size_t i = 0; i < ids.size(); i++)
	{
		StoredProfile profile;
		if (!LoadProfile(ids[i], &profile))
		{
			Wh_Log(L"  ! %s (record missing or unreadable)", ids[i].c_str());
			continue;
		}

		const FormatSpec spec = ProfileToFormat(profile);
		Wh_Log(L"  %s %s", (ids[i] == preferred) ? L"*" : L"-", profile.friendlyName);
		Wh_Log(L"    %s", DescribeFormat(spec).c_str());
		Wh_Log(L"    speaker config: %s", DescribeSpeakerMask(profile.physicalSpeakers).c_str());
	}
}

static void ReconcileDefaultDevice(PolicyConfigWriter& writer, const std::vector<Endpoint>& endpoints, const std::wstring& currentDefault, ULONGLONG now)
{
	const std::wstring preferred = LoadPreferredDefault();
	if (preferred.empty() || currentDefault.empty() || currentDefault == preferred)
	{
		return;
	}

	const Endpoint* preferredEndpoint = nullptr;
	for (size_t i = 0; i < endpoints.size(); i++)
	{
		if (endpoints[i].id == preferred)
		{
			preferredEndpoint = &endpoints[i];
			break;
		}
	}

	//	The preferred device is not available, so Windows moved the default somewhere else on its own. That is not a choice the user
	//	made, so the preference is left alone and will be restored when the device comes back.
	if (!preferredEndpoint || preferredEndpoint->state != DEVICE_STATE_ACTIVE)
	{
		return;
	}

	const size_t index = EnsureDeviceState(preferred);
	const bool justArrived = now < g_deviceStates[index].enforceUntil;

	if (justArrived)
	{
		if (SUCCEEDED(writer.SetDefaultEndpoint(preferred)))
		{
			Wh_Log(L"Restored preferred default device: %s", preferredEndpoint->friendlyName.c_str());
		}

		return;
	}

	//	The preferred device was sitting there available and the default still moved, so the user chose this deliberately. Follow them.
	for (size_t i = 0; i < endpoints.size(); i++)
	{
		if (endpoints[i].id == currentDefault)
		{
			SavePreferredDefault(currentDefault);
			Wh_Log(L"Preferred default device is now %s", endpoints[i].friendlyName.c_str());
			break;
		}
	}
}

static void Reconcile(PolicyConfigWriter& writer, bool firstPass)
{
	StateLock lock;

	//	During a Remote Desktop session the local endpoints are typically absent and playback is routed through Remote Audio. Enforcing
	//	profiles here would fight the remote session, and the device changes it causes are exactly the ones to ignore.
	if (g_settings.skipInRemoteSession && GetSystemMetrics(SM_REMOTESESSION))
	{
		Wh_Log(L"Remote session active, leaving audio devices alone");
		return;
	}

	std::vector<Endpoint> endpoints;
	if (FAILED(EnumerateEndpoints(false, false, &endpoints)))
	{
		return;
	}

	const ULONGLONG now = GetTickCount64();
	std::wstring currentDefault;

	for (size_t i = 0; i < endpoints.size(); i++)
	{
		if (endpoints[i].isDefault)
		{
			currentDefault = endpoints[i].id;
		}
	}

	for (size_t i = 0; i < endpoints.size(); i++)
	{
		const Endpoint& endpoint = endpoints[i];
		const size_t index = EnsureDeviceState(endpoint.id);
		const bool active = endpoint.state == DEVICE_STATE_ACTIVE;
		const bool arrived = active && !g_deviceStates[index].wasActive;

		//	The first pass counts as an arrival for everything already present, which is what restores settings after a reboot, an
		//	explorer restart, or the mod being enabled.
		if (arrived || (active && firstPass))
		{
			g_deviceStates[index].enforceUntil = now + kEnforceWindowMs;

			if (arrived && !firstPass)
			{
				Wh_Log(L"Device arrived: %s", endpoint.friendlyName.c_str());
			}
		}

		g_deviceStates[index].wasActive = active;

		if (!active || !endpoint.hasCurrent)
		{
			continue;
		}

		StoredProfile profile;
		if (!LoadProfile(endpoint.id, &profile))
		{
			if (SaveProfile(ProfileUpdateFrom(endpoint)))
			{
				Wh_Log(L"Learned profile for %s: %s", endpoint.friendlyName.c_str(), DescribeFormat(endpoint.current).c_str());
			}

			continue;
		}

		const FormatSpec desired = ProfileToFormat(profile);
		const bool formatMatches = FormatMatches(desired, endpoint.current);

		//	Enabling a spatial format puts the endpoint into whatever format the spatial renderer requires, and writing a different one
		//	back switches the spatial format off again. Windows models this by greying out the format dropdown whenever spatial sound
		//	is on, and the mod has to respect the same rule or the two settings fight each other forever.
		//
		//	So the current spatial format decides what is enforced: with spatial on, only the spatial format is restored and the wave
		//	format is left alone; with it off, the wave format is restored as usual. The stored wave format is the one the user chose
		//	with spatial off, and survives untouched while spatial is on so that turning it back off restores it.
		const bool spatialRecorded = (profile.flags & kProfileFlagHasSpatial) != 0;
		const bool spatialRefused = g_deviceStates[index].hasSpatialRefused && IsEqualGUID(g_deviceStates[index].spatialRefused, profile.spatialFormat);
		const bool inWindow = now < g_deviceStates[index].enforceUntil;

		//	A locked profile is one the user chose in the panel while the lock setting is on. It is never relearned, so a change from
		//	anywhere else is put back however long after the device arrived it happens, which closes the gap where Windows alters a
		//	format at some arbitrary moment rather than at re-detection.
		const bool locked = g_settings.lockUserSettings && (profile.flags & kProfileFlagUserSet) != 0;
		const bool enforcing = inWindow || locked;

		GUID actualSpatial = {};
		bool spatialKnown = false;

		if (g_settings.restoreSpatial && (enforcing || !formatMatches))
		{
			SpatialConfig spatial;
			spatialKnown = spatial.Open(endpoint.id) && spatial.Read(&actualSpatial);
		}

		const bool spatialActive = spatialKnown && !IsEqualGUID(actualSpatial, kSpatialOff);
		const bool spatialMismatched = spatialKnown && spatialRecorded && !spatialRefused && !IsEqualGUID(actualSpatial, profile.spatialFormat);

		if (enforcing)
		{
			if (!g_settings.enforceProfiles)
			{
				continue;
			}

			bool nowSpatial = spatialActive;

			if (spatialMismatched)
			{
				SpatialConfig spatial;
				if (spatial.Open(endpoint.id) && spatial.Apply(profile.spatialFormat))
				{
					nowSpatial = !IsEqualGUID(profile.spatialFormat, kSpatialOff);
					Wh_Log(L"Restored %s spatial sound to %s", endpoint.friendlyName.c_str(), DescribeSpatial(profile.spatialFormat).c_str());
				}
				else
				{
					g_deviceStates[index].spatialRefused = profile.spatialFormat;
					g_deviceStates[index].hasSpatialRefused = true;
					Wh_Log(L"Could not set %s spatial sound to %s, giving up on it for this device", endpoint.friendlyName.c_str(), DescribeSpatial(profile.spatialFormat).c_str());
				}
			}

			//	With a spatial format in effect the wave format belongs to the renderer, so touching it here would undo the spatial
			//	format that was just restored.
			if (nowSpatial)
			{
				continue;
			}

			//	Turning spatial off leaves the endpoint in the renderer's format, so the stored one is written back even when it
			//	matched before the change.
			if (!formatMatches || spatialMismatched)
			{
				const HRESULT hr = writer.Apply(endpoint.id, desired);
				if (SUCCEEDED(hr))
				{
					Wh_Log(L"Restored %s to %s", endpoint.friendlyName.c_str(), DescribeFormat(desired).c_str());
				}
				else
				{
					Wh_Log(L"Failed to restore %s (0x%08X)", endpoint.friendlyName.c_str(), static_cast<unsigned>(hr));
				}
			}
		}
		else
		{
			//	Outside the arrival window, and with nothing locked, a change is the user reconfiguring the device, so it becomes the
			//	new profile. A format imposed by a spatial renderer is not a choice the user made, so it is deliberately not learned.
			if (spatialActive)
			{
				if (spatialKnown && (!spatialRecorded || !IsEqualGUID(actualSpatial, profile.spatialFormat)))
				{
					ProfileUpdate update = ProfileUpdateFrom(endpoint);
					update.spec = desired;
					update.physicalSpeakers = profile.physicalSpeakers;
					update.hasSpatial = true;
					update.spatialFormat = actualSpatial;

					if (SaveProfile(update))
					{
						Wh_Log(L"Learned spatial sound for %s: %s", endpoint.friendlyName.c_str(), DescribeSpatial(actualSpatial).c_str());
					}
				}

				continue;
			}

			if (!formatMatches || (spatialKnown && spatialRecorded && !IsEqualGUID(actualSpatial, profile.spatialFormat)))
			{
				ProfileUpdate update = ProfileUpdateFrom(endpoint);
				update.hasSpatial = spatialKnown;
				update.spatialFormat = actualSpatial;

				if (SaveProfile(update))
				{
					Wh_Log(L"Learned new settings for %s: %s", endpoint.friendlyName.c_str(), DescribeFormat(endpoint.current).c_str());
				}
			}
		}
	}

	if (g_settings.restorePreferredDefault)
	{
		ReconcileDefaultDevice(writer, endpoints, currentDefault, now);
	}

	PruneProfiles(endpoints);

	//	Seeding the preference on the very first run means a later temporary switch can be undone without the user ever having had to
	//	nominate a device.
	if (LoadPreferredDefault().empty() && !currentDefault.empty())
	{
		SavePreferredDefault(currentDefault);
	}
}

static void RunInitialPass(PolicyConfigWriter& writer)
{
	std::vector<Endpoint> endpoints;
	HRESULT hr = EnumerateEndpoints(g_settings.logCapabilities, false, &endpoints);
	if (FAILED(hr))
	{
		Wh_Log(L"Endpoint enumeration failed (0x%08X)", static_cast<unsigned>(hr));
		return;
	}

	switch (writer.Validate(endpoints))
	{
		case WriteSupport::Enabled:
			Wh_Log(L"Write support: enabled");
			break;
		case WriteSupport::LayoutMismatch:
			Wh_Log(L"Write support: disabled, IPolicyConfig layout did not match");
			break;
		default:
			Wh_Log(L"Write support: unavailable");
			break;
	}

	LogEndpoints(endpoints, g_settings.logCapabilities);

	Reconcile(writer, true);
	LogProfiles();
}

// ====================================================================================================
//	Settings panel
// ====================================================================================================

//	A frameless popup listing the active devices, the bit depths and sample rates they support, and the speaker layouts, all on one
//	surface. The point is that every current value is visible at once, rather than buried three dialogs deep in the legacy applet.
//
//	This runs on its own single-threaded apartment with its own message pump and its own IPolicyConfig instance, because the watcher
//	thread is multithreaded and marshalling an undocumented interface between apartments is not worth the risk.

enum class ItemKind
{
	None,
	Device,
	Depth,
	Rate,
	Speaker,
	Spatial,
};

struct PanelItem
{
	ItemKind kind = ItemKind::None;
	size_t index = 0;
	RECT rect = {};
	bool enabled = true;
	bool checked = false;

	//	Only used by the device column, to mark which device Windows is actually playing through. Normally that is also the selected
	//	row, but the two come apart while a device is being switched, and silently showing the wrong one would be worse than useless.
	bool isDefault = false;

	std::wstring text;
};

struct PanelTheme
{
	COLORREF back;
	COLORREF text;
	COLORREF dim;
	COLORREF hover;
	COLORREF accent;
	COLORREF accentText;
	COLORREF line;
};

static HWND g_panelWnd = nullptr;
static HANDLE g_panelThread = nullptr;
static PolicyConfigWriter* g_panelWriter = nullptr;
static std::vector<Endpoint> g_panelEndpoints;
static std::vector<PanelItem> g_panelItems;
static size_t g_panelDevice = 0;
static int g_panelHot = -1;
static UINT g_panelDpi = 96;
static SIZE g_panelSize = {};
static HFONT g_panelFont = nullptr;
static HFONT g_panelHeadingFont = nullptr;
static PanelTheme g_panelTheme = {};
static bool g_panelTracking = false;
static ULONGLONG g_panelHiddenAt = 0;

static constexpr ULONGLONG kPanelReopenGuardMs = 250;

static constexpr size_t kNoIndex = static_cast<size_t>(-1);
static constexpr int kPanelHotkeyId = 1;
static constexpr UINT kPanelColumns = 5;

static const wchar_t* const kPanelClassName = L"WindhawkAudioDeviceProfilesPanel";

static int PanelScale(int value)
{
	return MulDiv(value, static_cast<int>(g_panelDpi), 96);
}

static bool PanelIsDarkMode()
{
	DWORD value = 1;
	DWORD size = sizeof(value);

	if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS)
	{
		return false;
	}

	return value == 0;
}

static void PanelUpdateTheme()
{
	const COLORREF accent = GetSysColor(COLOR_HIGHLIGHT);

	if (PanelIsDarkMode())
	{
		g_panelTheme = { RGB(32, 32, 32), RGB(255, 255, 255), RGB(150, 150, 150), RGB(56, 56, 56), accent, RGB(255, 255, 255), RGB(64, 64, 64) };
	}
	else
	{
		g_panelTheme = { RGB(249, 249, 249), RGB(0, 0, 0), RGB(105, 105, 105), RGB(230, 230, 230), accent, RGB(255, 255, 255), RGB(220, 220, 220) };
	}
}

static void PanelCreateFonts()
{
	if (g_panelFont)
	{
		DeleteObject(g_panelFont);
		g_panelFont = nullptr;
	}

	if (g_panelHeadingFont)
	{
		DeleteObject(g_panelHeadingFont);
		g_panelHeadingFont = nullptr;
	}

	LOGFONTW logFont = {};
	logFont.lfHeight = -PanelScale(12);
	logFont.lfWeight = FW_NORMAL;
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfQuality = CLEARTYPE_QUALITY;
	wcscpy(logFont.lfFaceName, L"Segoe UI");

	g_panelFont = CreateFontIndirectW(&logFont);

	logFont.lfHeight = -PanelScale(11);
	logFont.lfWeight = FW_SEMIBOLD;
	g_panelHeadingFont = CreateFontIndirectW(&logFont);
}

// ====================================================================================================
//	Capability lookups
// ====================================================================================================

static size_t PanelFindDepth(const FormatSpec& spec)
{
	for (size_t i = 0; i < kDepthOptionCount; i++)
	{
		if (kDepthOptions[i].bitsPerSample == spec.bitsPerSample && kDepthOptions[i].containerBits == spec.containerBits)
		{
			return i;
		}
	}

	return kNoIndex;
}

static size_t PanelFindRate(DWORD sampleRate)
{
	for (size_t i = 0; i < kSampleRateCount; i++)
	{
		if (kSampleRates[i] == sampleRate)
		{
			return i;
		}
	}

	return kNoIndex;
}

static size_t PanelFindSpeaker(DWORD mask)
{
	for (size_t i = 0; i < kSpeakerConfigCount; i++)
	{
		if (kSpeakerConfigs[i].mask == mask)
		{
			return i;
		}
	}

	return kNoIndex;
}

//	Answers "could this device do anything at all with these choices", where kNoIndex means the dimension is unconstrained. That
//	fallback matters when a device is currently sitting in a format the mod has no name for, which would otherwise grey out the whole
//	panel and leave the user no way back.
static bool PanelAnySupported(const Endpoint& endpoint, size_t config, size_t depth, size_t rate)
{
	for (size_t c = 0; c < kSpeakerConfigCount; c++)
	{
		if (config != kNoIndex && c != config)
		{
			continue;
		}

		for (size_t d = 0; d < kDepthOptionCount; d++)
		{
			if (depth != kNoIndex && d != depth)
			{
				continue;
			}

			for (size_t r = 0; r < kSampleRateCount; r++)
			{
				if (rate != kNoIndex && r != rate)
				{
					continue;
				}

				if (endpoint.IsSupported(c, d, r))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// ====================================================================================================
//	Layout
// ====================================================================================================

static void PanelRefreshEndpoints(bool probeCapabilities)
{
	std::wstring keepId;
	if (g_panelDevice < g_panelEndpoints.size())
	{
		keepId = g_panelEndpoints[g_panelDevice].id;
	}

	std::vector<Endpoint> all;
	if (FAILED(EnumerateEndpoints(probeCapabilities, true, &all)))
	{
		return;
	}

	//	The capability matrix and the spatial support list are both gathered by the probe, and neither changes while the panel is open.
	//	Carrying them across an edit is what keeps a click from greying out the columns it did not touch.
	if (!probeCapabilities)
	{
		for (size_t i = 0; i < all.size(); i++)
		{
			for (size_t j = 0; j < g_panelEndpoints.size(); j++)
			{
				if (all[i].id == g_panelEndpoints[j].id)
				{
					all[i].supported = g_panelEndpoints[j].supported;
					all[i].spatialSupported = g_panelEndpoints[j].spatialSupported;
					break;
				}
			}
		}
	}

	g_panelEndpoints.clear();
	for (size_t i = 0; i < all.size(); i++)
	{
		if (all[i].state == DEVICE_STATE_ACTIVE)
		{
			g_panelEndpoints.push_back(all[i]);
		}
	}

	g_panelDevice = 0;
	bool found = false;

	for (size_t i = 0; i < g_panelEndpoints.size() && !keepId.empty(); i++)
	{
		if (g_panelEndpoints[i].id == keepId)
		{
			g_panelDevice = i;
			found = true;
			break;
		}
	}

	//	Either nothing was selected yet, or the selected device just went away. Fall back to whatever is currently playing audio.
	for (size_t i = 0; i < g_panelEndpoints.size() && !found; i++)
	{
		if (g_panelEndpoints[i].isDefault)
		{
			g_panelDevice = i;
			break;
		}
	}
}

static void
PanelAddColumn(int x, int y, int width, int rowHeight, ItemKind kind, size_t count, const std::vector<std::wstring>& labels, const std::vector<bool>& enabled, size_t checkedIndex, size_t defaultIndex)
{
	for (size_t i = 0; i < count; i++)
	{
		PanelItem item;
		item.kind = kind;
		item.index = i;
		item.rect = { x, y + static_cast<int>(i) * rowHeight, x + width, y + static_cast<int>(i + 1) * rowHeight };
		item.enabled = enabled[i];
		item.checked = i == checkedIndex;
		item.isDefault = i == defaultIndex;
		item.text = labels[i];
		g_panelItems.push_back(item);
	}
}

static void PanelBuildItems()
{
	g_panelItems.clear();

	const int pad = PanelScale(14);
	const int rowHeight = PanelScale(26);
	const int headingHeight = PanelScale(26);
	const int gap = PanelScale(16);

	const int widths[kPanelColumns] = { PanelScale(300), PanelScale(190), PanelScale(110), PanelScale(180), PanelScale(210) };

	const Endpoint* device = g_panelDevice < g_panelEndpoints.size() ? &g_panelEndpoints[g_panelDevice] : nullptr;

	const size_t currentSpatial = (device && device->hasSpatial) ? FindSpatialFormat(device->spatialFormat) : kNoIndex;

	//	With spatial sound on, the endpoint is in the renderer's format rather than the user's, and the format cannot be changed
	//	without switching spatial sound off. The columns are therefore filled in from the stored profile and shown disabled, which is
	//	what the Sound applet does, so the user still sees what the device will go back to.
	const bool spatialOwnsFormat = device != nullptr && device->hasSpatial && !IsEqualGUID(device->spatialFormat, kSpatialOff);

	FormatSpec shown = device ? device->current : FormatSpec();
	if (spatialOwnsFormat)
	{
		StoredProfile stored = {};
		if (LoadProfile(device->id, &stored))
		{
			shown = ProfileToFormat(stored);
		}
	}

	const size_t currentDepth = device ? PanelFindDepth(shown) : kNoIndex;
	const size_t currentRate = device ? PanelFindRate(shown.sampleRate) : kNoIndex;
	const size_t currentSpeaker = device ? PanelFindSpeaker(shown.channelMask) : kNoIndex;

	const int top = pad + headingHeight;
	int x = pad;

	{
		std::vector<std::wstring> labels;
		std::vector<bool> enabled;
		size_t defaultDevice = kNoIndex;

		for (size_t i = 0; i < g_panelEndpoints.size(); i++)
		{
			labels.push_back(g_panelEndpoints[i].friendlyName);
			enabled.push_back(true);

			if (g_panelEndpoints[i].isDefault)
			{
				defaultDevice = i;
			}
		}

		PanelAddColumn(x, top, widths[0], rowHeight, ItemKind::Device, labels.size(), labels, enabled, g_panelDevice, defaultDevice);
	}

	x += widths[0] + gap;

	{
		std::vector<std::wstring> labels;
		std::vector<bool> enabled;

		for (size_t i = 0; i < kDepthOptionCount; i++)
		{
			labels.push_back(kDepthOptions[i].name);
			enabled.push_back(device != nullptr && !spatialOwnsFormat && PanelAnySupported(*device, currentSpeaker, i, currentRate));
		}

		PanelAddColumn(x, top, widths[1], rowHeight, ItemKind::Depth, labels.size(), labels, enabled, currentDepth, kNoIndex);
	}

	x += widths[1] + gap;

	{
		std::vector<std::wstring> labels;
		std::vector<bool> enabled;

		for (size_t i = 0; i < kSampleRateCount; i++)
		{
			wchar_t buffer[32];
			swprintf(buffer, ARRAYSIZE(buffer), L"%u Hz", static_cast<unsigned>(kSampleRates[i]));
			labels.push_back(buffer);
			enabled.push_back(device != nullptr && !spatialOwnsFormat && PanelAnySupported(*device, currentSpeaker, currentDepth, i));
		}

		PanelAddColumn(x, top, widths[2], rowHeight, ItemKind::Rate, labels.size(), labels, enabled, currentRate, kNoIndex);
	}

	x += widths[2] + gap;

	{
		std::vector<std::wstring> labels;
		std::vector<bool> enabled;

		for (size_t i = 0; i < kSpeakerConfigCount; i++)
		{
			labels.push_back(kSpeakerConfigs[i].name);
			enabled.push_back(device != nullptr && !spatialOwnsFormat && PanelAnySupported(*device, i, currentDepth, currentRate));
		}

		PanelAddColumn(x, top, widths[3], rowHeight, ItemKind::Speaker, labels.size(), labels, enabled, currentSpeaker, kNoIndex);
	}

	x += widths[3] + gap;

	{
		const std::vector<SpatialFormat>& formats = SpatialFormats();
		std::vector<std::wstring> labels;
		std::vector<bool> enabled;

		for (size_t i = 0; i < formats.size(); i++)
		{
			labels.push_back(formats[i].name);
			enabled.push_back(device != nullptr && device->hasSpatial && device->IsSpatialSupported(i));
		}

		PanelAddColumn(x, top, widths[4], rowHeight, ItemKind::Spatial, labels.size(), labels, enabled, currentSpatial, kNoIndex);
	}

	size_t rows = g_panelEndpoints.size();
	rows = rows > kDepthOptionCount ? rows : kDepthOptionCount;
	rows = rows > kSampleRateCount ? rows : kSampleRateCount;
	rows = rows > kSpeakerConfigCount ? rows : kSpeakerConfigCount;
	rows = rows > SpatialFormats().size() ? rows : SpatialFormats().size();

	int totalWidth = pad;
	for (size_t i = 0; i < kPanelColumns; i++)
	{
		totalWidth += widths[i] + (i + 1 < kPanelColumns ? gap : pad);
	}

	g_panelSize.cx = totalWidth;
	g_panelSize.cy = top + static_cast<int>(rows) * rowHeight + pad;
}

// ====================================================================================================
//	Painting
// ====================================================================================================

static void PanelDrawHeading(HDC dc, const wchar_t* text, int x, int width)
{
	const int pad = PanelScale(14);
	RECT rect = { x + PanelScale(8), pad, x + width, pad + PanelScale(26) };

	SelectObject(dc, g_panelHeadingFont);
	SetTextColor(dc, g_panelTheme.dim);
	DrawTextW(dc, text, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
}

static void PanelPaint(HDC dc)
{
	RECT client;
	GetClientRect(g_panelWnd, &client);

	HBRUSH background = CreateSolidBrush(g_panelTheme.back);
	FillRect(dc, &client, background);
	DeleteObject(background);

	SetBkMode(dc, TRANSPARENT);

	const wchar_t* headings[kPanelColumns] = { L"DEVICE", L"BIT DEPTH", L"SAMPLE RATE", L"SPEAKERS", L"SPATIAL SOUND" };
	ItemKind kinds[kPanelColumns] = { ItemKind::Device, ItemKind::Depth, ItemKind::Rate, ItemKind::Speaker, ItemKind::Spatial };

	for (size_t c = 0; c < kPanelColumns; c++)
	{
		for (size_t i = 0; i < g_panelItems.size(); i++)
		{
			if (g_panelItems[i].kind == kinds[c])
			{
				PanelDrawHeading(dc, headings[c], g_panelItems[i].rect.left, g_panelItems[i].rect.right - g_panelItems[i].rect.left);
				break;
			}
		}
	}

	SelectObject(dc, g_panelFont);

	for (size_t i = 0; i < g_panelItems.size(); i++)
	{
		const PanelItem& item = g_panelItems[i];
		RECT rect = item.rect;

		//	The current value of each of the three settings is the one filled with the accent color, so all three are readable in a
		//	single glance without reading any text.
		if (item.checked)
		{
			HBRUSH brush = CreateSolidBrush(g_panelTheme.accent);
			FillRect(dc, &rect, brush);
			DeleteObject(brush);
		}
		else if (static_cast<int>(i) == g_panelHot && item.enabled)
		{
			HBRUSH brush = CreateSolidBrush(g_panelTheme.hover);
			FillRect(dc, &rect, brush);
			DeleteObject(brush);
		}

		COLORREF color = g_panelTheme.text;
		if (item.checked)
		{
			color = g_panelTheme.accentText;
		}
		else if (!item.enabled)
		{
			color = g_panelTheme.dim;
		}

		SetTextColor(dc, color);

		RECT textRect = rect;
		textRect.left += PanelScale(8);
		textRect.right -= PanelScale(8);

		//	A dot on the trailing edge marks the device Windows is currently playing through.
		if (item.isDefault)
		{
			const int size = PanelScale(6);
			const int cx = rect.right - PanelScale(12);
			const int cy = (rect.top + rect.bottom) / 2;

			HBRUSH brush = CreateSolidBrush(item.checked ? g_panelTheme.accentText : g_panelTheme.accent);
			HGDIOBJ oldBrush = SelectObject(dc, brush);
			HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
			Ellipse(dc, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
			SelectObject(dc, oldPen);
			SelectObject(dc, oldBrush);
			DeleteObject(brush);

			textRect.right -= PanelScale(14);
		}

		DrawTextW(dc, item.text.c_str(), -1, &textRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
	}
}

// ====================================================================================================
//	Actions
// ====================================================================================================

static void PanelRelayout(bool reposition)
{
	PanelBuildItems();

	if (reposition)
	{
		POINT cursor = {};
		GetCursorPos(&cursor);

		MONITORINFO monitor = {};
		monitor.cbSize = sizeof(monitor);
		GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY), &monitor);

		const int margin = PanelScale(12);
		const int x = monitor.rcWork.right - g_panelSize.cx - margin;
		const int y = monitor.rcWork.bottom - g_panelSize.cy - margin;

		SetWindowPos(g_panelWnd, HWND_TOPMOST, x, y, g_panelSize.cx, g_panelSize.cy, SWP_NOACTIVATE);
	}
	else
	{
		SetWindowPos(g_panelWnd, nullptr, 0, 0, g_panelSize.cx, g_panelSize.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	InvalidateRect(g_panelWnd, nullptr, FALSE);
}

//	Records the result of a panel edit. The spatial format is read back from the device rather than assumed, because it is the one
//	value a write is not guaranteed to have changed.
//
//	While a spatial format is in effect the endpoint's wave format belongs to the renderer, so keepFormat says to leave the stored one
//	alone. That is what lets the user's chosen speaker layout and sample rate survive a trip through Dolby Atmos and come back when
//	spatial sound is turned off again.
static void PanelStoreProfile(const std::wstring& id, const std::wstring& name, const FormatSpec& spec, bool keepFormat)
{
	ProfileUpdate update;
	update.endpointId = id;
	update.friendlyName = name;
	update.spec = spec;
	update.hasSpeakers = true;
	update.physicalSpeakers = spec.channelMask;
	update.userSet = true;

	SpatialConfig spatial;
	if (spatial.Open(id))
	{
		update.hasSpatial = spatial.Read(&update.spatialFormat);
	}

	StateLock lock;

	StoredProfile existing = {};
	if (keepFormat && LoadProfile(id, &existing))
	{
		update.spec = ProfileToFormat(existing);
		update.physicalSpeakers = existing.physicalSpeakers;
	}

	SaveProfile(update);
	ClearEnforceWindow(id);
}

static void PanelCommit(const std::wstring& id, const std::wstring& name, const FormatSpec& spec, bool keepFormat)
{
	PanelStoreProfile(id, name, spec, keepFormat);

	PanelRefreshEndpoints(false);
	PanelRelayout(false);
}

static void PanelApplySpec(const FormatSpec& spec)
{
	if (g_panelDevice >= g_panelEndpoints.size() || !g_panelWriter)
	{
		return;
	}

	const std::wstring id = g_panelEndpoints[g_panelDevice].id;
	const std::wstring name = g_panelEndpoints[g_panelDevice].friendlyName;

	HRESULT hr = S_OK;
	{
		//	Held across the write and the profile update together. The write itself wakes the reconcile pass, and with locking on that
		//	pass would otherwise see the new format next to the old profile and immediately undo the edit.
		StateLock lock;

		hr = g_panelWriter->Apply(id, spec);
		if (SUCCEEDED(hr))
		{
			PanelStoreProfile(id, name, spec, false);
		}
	}

	if (FAILED(hr))
	{
		Wh_Log(L"Panel failed to apply %s to %s (0x%08X)", DescribeFormat(spec).c_str(), name.c_str(), static_cast<unsigned>(hr));
		return;
	}

	Wh_Log(L"Panel set %s to %s", name.c_str(), DescribeFormat(spec).c_str());

	PanelRefreshEndpoints(false);
	PanelRelayout(false);
}

//	Turning a spatial format on hands the endpoint's wave format to the renderer, so nothing is written back afterwards; doing so is
//	what switches the spatial format straight off again. Turning it off leaves the endpoint in the renderer's format, so the format
//	the user chose is restored at that point instead.
static void PanelApplySpatial(size_t formatIndex)
{
	const std::vector<SpatialFormat>& formats = SpatialFormats();
	if (g_panelDevice >= g_panelEndpoints.size() || !g_panelWriter || formatIndex >= formats.size())
	{
		return;
	}

	const std::wstring id = g_panelEndpoints[g_panelDevice].id;
	const std::wstring name = g_panelEndpoints[g_panelDevice].friendlyName;
	const FormatSpec spec = g_panelEndpoints[g_panelDevice].current;
	const bool turningOn = !IsEqualGUID(formats[formatIndex].subtype, kSpatialOff);

	//	What to fall back to when spatial sound is switched off. The endpoint's current format is no use for that while a renderer owns
	//	it, so the stored profile is preferred.
	//
	//	The lock is held from here through the profile update, because every write below wakes the reconcile pass and, with locking on,
	//	that pass would see the half-finished change next to the old profile and undo it.
	FormatSpec restore = spec;
	StateLock lock;

	{
		StoredProfile existing = {};
		if (LoadProfile(id, &existing))
		{
			restore = ProfileToFormat(existing);
		}
	}

	SpatialConfig spatial;
	if (!spatial.Open(id) || !spatial.Apply(formats[formatIndex].subtype))
	{
		//	Reaching here usually means the format needs a license the machine does not have. The endpoint still reports it as
		//	supported, so the only honest thing to do is say so and leave the device alone.
		Wh_Log(L"Panel could not set %s spatial sound to %s; it is reported as supported but did not take effect", name.c_str(), formats[formatIndex].name);
		PanelRefreshEndpoints(false);
		PanelRelayout(false);
		return;
	}

	Wh_Log(L"Panel set %s spatial sound to %s", name.c_str(), formats[formatIndex].name);

	if (!turningOn)
	{
		const HRESULT hr = g_panelWriter->Apply(id, restore);
		if (SUCCEEDED(hr))
		{
			Wh_Log(L"Panel restored %s to %s", name.c_str(), DescribeFormat(restore).c_str());
		}
	}

	PanelCommit(id, name, restore, turningOn);
}

static void PanelActivate(const PanelItem& item)
{
	if (!item.enabled)
	{
		return;
	}

	if (item.kind == ItemKind::Device)
	{
		if (item.index >= g_panelEndpoints.size() || !g_panelWriter)
		{
			return;
		}

		g_panelDevice = item.index;

		//	Clicking a device both selects it for the other two columns and makes it the default, which is the whole point of having
		//	the list here rather than sending the user back to the Sound applet.
		if (!g_panelEndpoints[item.index].isDefault)
		{
			const std::wstring id = g_panelEndpoints[item.index].id;
			if (SUCCEEDED(g_panelWriter->SetDefaultEndpoint(id)))
			{
				StateLock lock;
				SavePreferredDefault(id);
				Wh_Log(L"Panel set default device to %s", g_panelEndpoints[item.index].friendlyName.c_str());
			}
		}

		PanelRefreshEndpoints(false);
		PanelRelayout(false);
		return;
	}

	if (g_panelDevice >= g_panelEndpoints.size())
	{
		return;
	}

	FormatSpec spec = g_panelEndpoints[g_panelDevice].current;
	spec.isFloat = false;

	switch (item.kind)
	{
		case ItemKind::Depth:
			spec.bitsPerSample = kDepthOptions[item.index].bitsPerSample;
			spec.containerBits = kDepthOptions[item.index].containerBits;
			break;
		case ItemKind::Rate:
			spec.sampleRate = kSampleRates[item.index];
			break;
		case ItemKind::Speaker:
			spec.channelMask = kSpeakerConfigs[item.index].mask;
			spec.channels = kSpeakerConfigs[item.index].channels;
			break;
		case ItemKind::Spatial:
			PanelApplySpatial(item.index);
			return;
		default:
			return;
	}

	PanelApplySpec(spec);
}

static int PanelHitTest(POINT point)
{
	for (size_t i = 0; i < g_panelItems.size(); i++)
	{
		if (PtInRect(&g_panelItems[i].rect, point))
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}

static void PanelShow()
{
	PanelUpdateTheme();
	PanelRefreshEndpoints(true);
	PanelRelayout(true);

	ShowWindow(g_panelWnd, SW_SHOW);
	SetForegroundWindow(g_panelWnd);
}

static void PanelToggle()
{
	if (IsWindowVisible(g_panelWnd))
	{
		ShowWindow(g_panelWnd, SW_HIDE);
		return;
	}

	//	Clicking the tray icon while the panel is open deactivates it first, which hides it, and the click would then reopen it so the
	//	panel would never appear to close. A click arriving immediately after a dismissal is treated as the dismissal it really is.
	if (GetTickCount64() - g_panelHiddenAt < kPanelReopenGuardMs)
	{
		return;
	}

	PanelShow();
}

// ====================================================================================================
//	Tray icon
// ====================================================================================================

//	The icon is drawn from a Segoe MDL2 Assets glyph rather than shipped as a resource, because a Windhawk mod is distributed as a
//	single source file and has nowhere to put one. Drawing it also means it can be rebuilt at the right size and in the right color
//	whenever the taskbar's DPI or theme changes.
static HICON g_trayIcon = nullptr;
static bool g_trayAdded = false;
static bool g_trayUsesGuid = true;
static bool g_trayLightTheme = false;
static int g_trayIconSize = 0;
static UINT g_taskbarCreatedMessage = 0;

static constexpr UINT kTrayCallbackMessage = WM_APP + 1;
static constexpr UINT kTrayIconId = 1;
static constexpr UINT kTrayCommandOpen = 100;
static constexpr UINT kTrayCommandSoundSettings = 101;
static constexpr UINT kTrayCommandLegacySound = 102;

//	Identifying the icon by GUID rather than by window and id is what gets it listed in the taskbar's notification area settings, and is
//	what makes the shell remember whether the user pinned it. The shell binds the GUID to the executable that registered it, which is
//	always explorer.exe here, so it stays valid across restarts.
static constexpr GUID kTrayIconGuid = { 0x8f3a2c41, 0x6d7b, 0x4e59, { 0x9a, 0x1c, 0x3b, 0x5e, 0x2d, 0x7f, 0x4a, 0x60 } };

//	The notification area settings page names every icon after the file description of the executable that owns it, and a Windhawk mod
//	runs inside explorer.exe, so this one is unavoidably listed as "Windows Explorer". There is no per-icon name override; an
//	application identity on the window is not consulted. The tooltip is what identifies it everywhere else.

//	The notification area follows the system theme rather than the app theme, so a light taskbar needs a dark glyph and the other way
//	round.
static bool PanelTrayUsesLightTheme()
{
	DWORD value = 0;
	DWORD size = sizeof(value);

	if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS)
	{
		return false;
	}

	return value != 0;
}

static HICON PanelBuildTrayIcon(int size, bool lightTheme)
{
	if (size <= 0)
	{
		size = 16;
	}

	HDC screenDc = GetDC(nullptr);
	if (!screenDc)
	{
		return nullptr;
	}

	ON_SCOPE_EXIT([&] { ReleaseDC(nullptr, screenDc); });

	//	A 32-bit top-down DIB with straight alpha, so the glyph can be drawn once and then given an alpha channel derived from its own
	//	coverage. Tray icons are composited over an unknown background, so anything without real alpha looks like a black box.
	BITMAPINFO info = {};
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth = size;
	info.bmiHeader.biHeight = -size;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;

	void* bits = nullptr;
	HBITMAP color = CreateDIBSection(screenDc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
	if (!color || !bits)
	{
		if (color)
		{
			DeleteObject(color);
		}

		return nullptr;
	}

	ON_SCOPE_EXIT([&] { DeleteObject(color); });

	//	Drawn in an inner scope, because GDI refuses to copy a bitmap that is still selected into a device context and the icon below
	//	would silently fail to be created.
	{
		HDC memoryDc = CreateCompatibleDC(screenDc);
		if (!memoryDc)
		{
			return nullptr;
		}

		ON_SCOPE_EXIT([&] { DeleteDC(memoryDc); });

		HGDIOBJ oldBitmap = SelectObject(memoryDc, color);
		ON_SCOPE_EXIT([&] { SelectObject(memoryDc, oldBitmap); });

		//	The glyph is drawn white on black so that each pixel's brightness is exactly its coverage, which then becomes its alpha.
		RECT bounds = { 0, 0, size, size };
		FillRect(memoryDc, &bounds, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
		SetBkMode(memoryDc, TRANSPARENT);
		SetTextColor(memoryDc, RGB(255, 255, 255));

		LOGFONTW font = {};
		font.lfHeight = -size;
		font.lfWeight = FW_NORMAL;
		font.lfCharSet = DEFAULT_CHARSET;
		font.lfQuality = CLEARTYPE_QUALITY;
		wcscpy_s(font.lfFaceName, ARRAYSIZE(font.lfFaceName), L"Segoe MDL2 Assets");

		bool drewGlyph = false;
		HFONT glyphFont = CreateFontIndirectW(&font);

		if (glyphFont)
		{
			HGDIOBJ oldFont = SelectObject(memoryDc, glyphFont);

			//	CreateFontIndirectW never fails, it substitutes, so the only way to know the icon font is actually installed is to ask
			//	what was really selected. A substituted font would draw a meaningless letter or an empty box.
			wchar_t face[LF_FACESIZE] = {};
			if (GetTextFaceW(memoryDc, ARRAYSIZE(face), face) > 0 && _wcsicmp(face, L"Segoe MDL2 Assets") == 0)
			{
				//	E767 is the speaker glyph the volume flyout uses.
				DrawTextW(memoryDc, L"\xE767", 1, &bounds, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
				drewGlyph = true;
			}

			SelectObject(memoryDc, oldFont);
			DeleteObject(glyphFont);
		}

		if (!drewGlyph)
		{
			//	Plain filled disc as a last resort, so there is always something clickable in the notification area.
			HGDIOBJ oldBrush = SelectObject(memoryDc, GetStockObject(WHITE_BRUSH));
			HGDIOBJ oldPen = SelectObject(memoryDc, GetStockObject(NULL_PEN));
			Ellipse(memoryDc, 1, 1, size - 1, size - 1);
			SelectObject(memoryDc, oldPen);
			SelectObject(memoryDc, oldBrush);
		}

		GdiFlush();
	}

	//	Coverage becomes alpha, and the visible color is then whichever of black or white contrasts with the taskbar. The channels are
	//	premultiplied because that is what CreateIconIndirect expects from a 32-bit DIB.
	const BYTE tint = lightTheme ? 0 : 255;
	BYTE* pixels = static_cast<BYTE*>(bits);

	for (int i = 0; i < size * size; ++i)
	{
		BYTE* pixel = pixels + static_cast<size_t>(i) * 4;
		const BYTE coverage = pixel[2];

		pixel[0] = static_cast<BYTE>(tint * coverage / 255);
		pixel[1] = pixel[0];
		pixel[2] = pixel[0];
		pixel[3] = coverage;
	}

	//	The mask is ignored for a 32-bit icon, but one still has to be supplied.
	HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
	if (!mask)
	{
		return nullptr;
	}

	ON_SCOPE_EXIT([&] { DeleteObject(mask); });

	ICONINFO iconInfo = {};
	iconInfo.fIcon = TRUE;
	iconInfo.hbmMask = mask;
	iconInfo.hbmColor = color;

	return CreateIconIndirect(&iconInfo);
}

static void PanelDestroyTrayIcon()
{
	if (g_trayIcon)
	{
		DestroyIcon(g_trayIcon);
		g_trayIcon = nullptr;
	}
}

static void PanelAddTrayIcon()
{
	const bool lightTheme = PanelTrayUsesLightTheme();
	const int iconSize = GetSystemMetrics(SM_CXSMICON);

	//	WM_SETTINGCHANGE arrives for all sorts of unrelated reasons, so the glyph is only redrawn when something that affects it
	//	actually changed. The icon still has to be re-registered after a shell restart, which clears g_trayAdded.
	if (g_trayIcon && g_trayAdded && lightTheme == g_trayLightTheme && iconSize == g_trayIconSize)
	{
		return;
	}

	PanelDestroyTrayIcon();
	g_trayIcon = PanelBuildTrayIcon(iconSize, lightTheme);
	g_trayLightTheme = lightTheme;
	g_trayIconSize = iconSize;

	if (!g_trayIcon)
	{
		Wh_Log(L"Tray icon could not be drawn");
		return;
	}

	NOTIFYICONDATAW data = {};
	data.cbSize = sizeof(data);
	data.hWnd = g_panelWnd;
	data.uID = kTrayIconId;
	data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
	data.uCallbackMessage = kTrayCallbackMessage;
	data.hIcon = g_trayIcon;
	wcscpy_s(data.szTip, ARRAYSIZE(data.szTip), L"Audio Device Profiles");

	if (g_trayUsesGuid)
	{
		data.uFlags |= NIF_GUID;
		data.guidItem = kTrayIconGuid;
	}

	if (!Shell_NotifyIconW(g_trayAdded ? NIM_MODIFY : NIM_ADD, &data))
	{
		//	A failed NIM_MODIFY means the icon is gone rather than stale, which is what happens when the shell restarts, so it is added
		//	back from scratch.
		g_trayAdded = false;

		if (!Shell_NotifyIconW(NIM_ADD, &data))
		{
			//	The shell rejects a GUID that it has already bound to a different executable, which is what happens if the mod is ever
			//	hosted somewhere other than explorer.exe. Falling back to the window and id form still gives a working icon, it just
			//	will not be listed in the notification area settings.
			if (!g_trayUsesGuid)
			{
				Wh_Log(L"Tray icon could not be added (%u)", static_cast<unsigned>(GetLastError()));
				return;
			}

			Wh_Log(L"Tray icon GUID was rejected (%u), falling back to an untracked icon", static_cast<unsigned>(GetLastError()));

			g_trayUsesGuid = false;
			data.uFlags &= ~static_cast<UINT>(NIF_GUID);
			data.guidItem = GUID{};

			if (!Shell_NotifyIconW(NIM_ADD, &data))
			{
				Wh_Log(L"Tray icon could not be added (%u)", static_cast<unsigned>(GetLastError()));
				return;
			}
		}
	}

	g_trayAdded = true;

	//	Version 4 is what makes the shell send screen coordinates with the callback, which is what places the context menu correctly on
	//	a high-DPI or secondary display.
	data.uVersion = NOTIFYICON_VERSION_4;
	Shell_NotifyIconW(NIM_SETVERSION, &data);

	Wh_Log(L"Tray icon added");
}

static void PanelRemoveTrayIcon()
{
	if (g_trayAdded)
	{
		NOTIFYICONDATAW data = {};
		data.cbSize = sizeof(data);
		data.hWnd = g_panelWnd;
		data.uID = kTrayIconId;

		if (g_trayUsesGuid)
		{
			data.uFlags = NIF_GUID;
			data.guidItem = kTrayIconGuid;
		}

		Shell_NotifyIconW(NIM_DELETE, &data);
		g_trayAdded = false;
	}

	PanelDestroyTrayIcon();
}

static void PanelShowTrayMenu(int x, int y)
{
	HMENU menu = CreatePopupMenu();
	if (!menu)
	{
		return;
	}

	ON_SCOPE_EXIT([&] { DestroyMenu(menu); });

	AppendMenuW(menu, MF_STRING, kTrayCommandOpen, L"Audio devices\tCtrl+Shift+Alt+A");
	AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
	AppendMenuW(menu, MF_STRING, kTrayCommandSoundSettings, L"Windows sound settings");
	AppendMenuW(menu, MF_STRING, kTrayCommandLegacySound, L"Legacy sound control panel");
	SetMenuDefaultItem(menu, kTrayCommandOpen, FALSE);

	//	Both of these are the documented workaround for tray menus, without which the menu refuses to dismiss when the user clicks away
	//	from it.
	SetForegroundWindow(g_panelWnd);

	const UINT command = static_cast<UINT>(TrackPopupMenuEx(menu, TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, x, y, g_panelWnd, nullptr));
	PostMessageW(g_panelWnd, WM_NULL, 0, 0);

	switch (command)
	{
		case kTrayCommandOpen:
			PanelShow();
			break;

		case kTrayCommandSoundSettings:
			ShellExecuteW(nullptr, L"open", L"ms-settings:sound", nullptr, nullptr, SW_SHOWNORMAL);
			break;

		case kTrayCommandLegacySound:
			ShellExecuteW(nullptr, L"open", L"rundll32.exe", L"shell32.dll,Control_RunDLL mmsys.cpl,,playback", nullptr, SW_SHOWNORMAL);
			break;

		default:
			break;
	}
}

// ====================================================================================================
//	Window
// ====================================================================================================

static LRESULT CALLBACK PanelWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//	The shell posts this to every top-level window when it restarts, and any tray icon has to be added back at that point. It is a
	//	registered message rather than a constant, so it cannot be a case label.
	if (message == g_taskbarCreatedMessage && g_taskbarCreatedMessage != 0)
	{
		g_trayAdded = false;
		PanelAddTrayIcon();
		return 0;
	}

	switch (message)
	{
		case kTrayCallbackMessage:
			switch (LOWORD(lParam))
			{
				case NIN_SELECT:
				case NIN_KEYSELECT:
					PanelToggle();
					break;

				case WM_CONTEXTMENU:
					//	Version 4 puts the anchor point in wParam rather than lParam, already in screen coordinates.
					PanelShowTrayMenu(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam));
					break;

				default:
					break;
			}

			return 0;

		case WM_HOTKEY:
			if (wParam == kPanelHotkeyId)
			{
				PanelToggle();
			}

			return 0;

		case WM_ACTIVATE:
			//	Dismiss as soon as focus goes elsewhere, the way the volume and network flyouts behave.
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				ShowWindow(hwnd, SW_HIDE);
				g_panelHiddenAt = GetTickCount64();
			}

			return 0;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				ShowWindow(hwnd, SW_HIDE);
			}

			return 0;

		case WM_MOUSEMOVE:
		{
			POINT point = { static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
			const int hot = PanelHitTest(point);

			if (hot != g_panelHot)
			{
				g_panelHot = hot;
				InvalidateRect(hwnd, nullptr, FALSE);
			}

			if (!g_panelTracking)
			{
				TRACKMOUSEEVENT track = {};
				track.cbSize = sizeof(track);
				track.dwFlags = TME_LEAVE;
				track.hwndTrack = hwnd;
				TrackMouseEvent(&track);
				g_panelTracking = true;
			}

			return 0;
		}

		case WM_MOUSELEAVE:
			g_panelTracking = false;
			g_panelHot = -1;
			InvalidateRect(hwnd, nullptr, FALSE);
			return 0;

		case WM_LBUTTONUP:
		{
			POINT point = { static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
			const int hit = PanelHitTest(point);

			if (hit >= 0)
			{
				PanelActivate(g_panelItems[static_cast<size_t>(hit)]);
			}

			return 0;
		}

		case WM_DPICHANGED:
			g_panelDpi = HIWORD(wParam);
			PanelCreateFonts();
			PanelAddTrayIcon();
			PanelRelayout(true);
			return 0;

		case WM_SETTINGCHANGE:
			//	Covers a switch between the light and dark system themes, which changes both the panel colors and the tray glyph.
			PanelUpdateTheme();
			PanelAddTrayIcon();
			InvalidateRect(hwnd, nullptr, FALSE);
			return 0;

		case WM_ERASEBKGND:
			return 1;

		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC dc = BeginPaint(hwnd, &paint);

			//	Painted into a memory bitmap first, because every row is filled and redrawn on hover and doing that directly on screen
			//	flickers badly.
			RECT client;
			GetClientRect(hwnd, &client);

			HDC memoryDc = CreateCompatibleDC(dc);
			HBITMAP bitmap = CreateCompatibleBitmap(dc, client.right, client.bottom);
			HGDIOBJ oldBitmap = SelectObject(memoryDc, bitmap);

			PanelPaint(memoryDc);
			BitBlt(dc, 0, 0, client.right, client.bottom, memoryDc, 0, 0, SRCCOPY);

			SelectObject(memoryDc, oldBitmap);
			DeleteObject(bitmap);
			DeleteDC(memoryDc);

			EndPaint(hwnd, &paint);
			return 0;
		}

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			break;
	}

	return DefWindowProcW(hwnd, message, wParam, lParam);
}

static DWORD WINAPI PanelThreadProc(LPVOID param)
{
	(void)param;

	//	Single-threaded apartment with a pump, which is what both the window and the tray icon need.
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		Wh_Log(L"Panel CoInitializeEx failed (0x%08X)", static_cast<unsigned>(hr));
		return 0;
	}

	ON_SCOPE_EXIT([] { CoUninitialize(); });

	PolicyConfigWriter writer;
	{
		std::vector<Endpoint> endpoints;
		if (FAILED(EnumerateEndpoints(false, false, &endpoints)) || writer.Validate(endpoints) != WriteSupport::Enabled)
		{
			Wh_Log(L"Panel disabled, no write support");
			return 0;
		}
	}

	g_panelWriter = &writer;
	ON_SCOPE_EXIT([] { g_panelWriter = nullptr; });

	WNDCLASSEXW windowClass = {};
	windowClass.cbSize = sizeof(windowClass);
	windowClass.lpfnWndProc = PanelWndProc;
	windowClass.hInstance = GetModuleHandleW(nullptr);
	windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	windowClass.lpszClassName = kPanelClassName;

	const ATOM atom = RegisterClassExW(&windowClass);
	if (!atom)
	{
		Wh_Log(L"Panel class registration failed (%u)", static_cast<unsigned>(GetLastError()));
		return 0;
	}

	ON_SCOPE_EXIT([&] { UnregisterClassW(kPanelClassName, windowClass.hInstance); });

	g_panelWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kPanelClassName, L"Audio Device Profiles", WS_POPUP | WS_BORDER, 0, 0, 100, 100, nullptr, nullptr, windowClass.hInstance, nullptr);

	if (!g_panelWnd)
	{
		Wh_Log(L"Panel window creation failed (%u)", static_cast<unsigned>(GetLastError()));
		return 0;
	}

	ON_SCOPE_EXIT([] { g_panelWnd = nullptr; });

	//	Resolved at runtime rather than linked, so the mod keeps building against the older headers Windhawk ships.
	using PfnGetDpiForWindow = UINT(WINAPI*)(HWND);
	if (HMODULE user32 = GetModuleHandleW(L"user32.dll"))
	{
		if (auto getDpiForWindow = reinterpret_cast<PfnGetDpiForWindow>(reinterpret_cast<void*>(GetProcAddress(user32, "GetDpiForWindow"))))
		{
			const UINT dpi = getDpiForWindow(g_panelWnd);
			if (dpi)
			{
				g_panelDpi = dpi;
			}
		}
	}

	PanelUpdateTheme();
	PanelCreateFonts();

	if (!RegisterHotKey(g_panelWnd, kPanelHotkeyId, MOD_CONTROL | MOD_SHIFT | MOD_ALT | MOD_NOREPEAT, 'A'))
	{
		Wh_Log(L"Panel hotkey Ctrl+Shift+Alt+A is already taken by something else");
	}

	g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
	PanelAddTrayIcon();

	Wh_Log(L"Panel ready, click the tray icon or press Ctrl+Shift+Alt+A");

	MSG message;
	while (GetMessageW(&message, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	PanelRemoveTrayIcon();
	UnregisterHotKey(g_panelWnd, kPanelHotkeyId);

	if (g_panelFont)
	{
		DeleteObject(g_panelFont);
		g_panelFont = nullptr;
	}

	if (g_panelHeadingFont)
	{
		DeleteObject(g_panelHeadingFont);
		g_panelHeadingFont = nullptr;
	}

	return 0;
}

// ====================================================================================================
//	Worker
// ====================================================================================================

//	Windows can run several explorer.exe processes at once: the shell, plus one per folder window when "Launch folder windows in a
//	separate process" is enabled. Only the shell process should run the mod, since it lives for the whole session and owns the taskbar
//	the tray icon will later attach to.
//
//	This cannot be checked in Wh_ModInit, because Windhawk injects early enough that the shell window often does not exist yet.
//	Waiting for it here also handles explorer restarting after a crash.
static bool WaitUntilShellProcess()
{
	for (;;)
	{
		HWND shell = GetShellWindow();
		if (shell)
		{
			DWORD shellPid = 0;
			GetWindowThreadProcessId(shell, &shellPid);
			return shellPid == GetCurrentProcessId();
		}

		//	The wait is interruptible so unloading the mod during explorer's startup does not stall Windhawk.
		if (WaitForSingleObject(g_stopEvent, 500) != WAIT_TIMEOUT)
		{
			return false;
		}
	}
}

static DWORD WINAPI WorkerThreadProc(LPVOID param)
{
	(void)param;

	if (!WaitUntilShellProcess())
	{
		Wh_Log(L"Not the shell process, standing down");
		return 0;
	}

	//	Multithreaded apartment: endpoint notifications are delivered on the audio service's own thread and need no message pump, and
	//	every interface used here is free-threaded. COM is initialized on this thread so the mod never disturbs explorer's own state.
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		Wh_Log(L"CoInitializeEx failed (0x%08X)", static_cast<unsigned>(hr));
		return 0;
	}

	ON_SCOPE_EXIT([] { CoUninitialize(); });

	PolicyConfigWriter writer;
	RunInitialPass(writer);

	//	Started only once this is known to be the shell process, so folder windows never put up a panel or steal the hotkey.
	g_panelThread = CreateThread(nullptr, 0, PanelThreadProc, nullptr, 0, nullptr);

	ComPtr<IMMDeviceEnumerator> enumerator;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));

	DeviceWatcher* watcher = nullptr;
	if (SUCCEEDED(hr))
	{
		watcher = new DeviceWatcher();
		if (FAILED(enumerator->RegisterEndpointNotificationCallback(watcher)))
		{
			Wh_Log(L"Failed to register for device notifications, profiles will not be restored automatically");
			watcher->Release();
			watcher = nullptr;
		}
	}

	//	Declared after the COM exiter so it runs first: the callback must be unregistered while COM is still initialized.
	ON_SCOPE_EXIT(
		[&]
		{
			if (watcher)
			{
				enumerator->UnregisterEndpointNotificationCallback(watcher);
				watcher->Release();
			}
		});

	HANDLE handles[2] = { g_stopEvent, g_changeEvent };

	for (;;)
	{
		if (WaitForMultipleObjects(2, handles, FALSE, INFINITE) != WAIT_OBJECT_0 + 1)
		{
			return 0;
		}

		//	Wait for the notification storm to go quiet. A device coming back produces a burst of state, property and default device
		//	changes, and acting on the first of them would apply a format that Windows is about to overwrite.
		bool settled = false;
		while (!settled)
		{
			switch (WaitForMultipleObjects(2, handles, FALSE, kSettleMs))
			{
				case WAIT_OBJECT_0:
					return 0;
				case WAIT_TIMEOUT:
					settled = true;
					break;
				default:
					break;
			}
		}

		Reconcile(writer, false);
	}
}

// ====================================================================================================
//	Entry points
// ====================================================================================================

static void LoadSettings()
{
	g_settings.logCapabilities = Wh_GetIntSetting(L"logCapabilities") != 0;
	g_settings.enforceProfiles = Wh_GetIntSetting(L"enforceProfiles") != 0;
	g_settings.restoreSpatial = Wh_GetIntSetting(L"restoreSpatial") != 0;
	g_settings.restorePreferredDefault = Wh_GetIntSetting(L"restorePreferredDefault") != 0;
	g_settings.lockUserSettings = Wh_GetIntSetting(L"lockUserSettings") != 0;
	g_settings.skipInRemoteSession = Wh_GetIntSetting(L"skipInRemoteSession") != 0;
}

BOOL Wh_ModInit()
{
	//	Logged unconditionally so the log shows which process Windhawk loaded the mod into. Whether this process is the one that will
	//	actually do the work is decided later, on the worker thread, once the shell window exists.
	Wh_Log(L"Init (pid %u)", static_cast<unsigned>(GetCurrentProcessId()));

	LoadSettings();

	InitializeCriticalSection(&g_stateLock);
	g_stateLockReady = true;

	InitializeCriticalSection(&g_capabilityLock);
	g_capabilityLockReady = true;

	g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	g_changeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

	if (!g_stopEvent || !g_changeEvent)
	{
		return FALSE;
	}

	g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
	if (!g_workerThread)
	{
		return FALSE;
	}

	return TRUE;
}

void Wh_ModBeforeUninit()
{
	if (g_stopEvent)
	{
		SetEvent(g_stopEvent);
	}

	//	The window must be destroyed by the thread that owns it, so the pump is asked to shut itself down rather than doing it here.
	if (g_panelWnd)
	{
		PostMessageW(g_panelWnd, WM_CLOSE, 0, 0);
	}

	if (g_panelThread)
	{
		WaitForSingleObject(g_panelThread, 5000);
	}

	if (g_workerThread)
	{
		WaitForSingleObject(g_workerThread, 5000);
	}
}

void Wh_ModUninit()
{
	if (g_panelThread)
	{
		CloseHandle(g_panelThread);
		g_panelThread = nullptr;
	}

	if (g_workerThread)
	{
		CloseHandle(g_workerThread);
		g_workerThread = nullptr;
	}

	if (g_stopEvent)
	{
		CloseHandle(g_stopEvent);
		g_stopEvent = nullptr;
	}

	if (g_changeEvent)
	{
		CloseHandle(g_changeEvent);
		g_changeEvent = nullptr;
	}

	if (g_stateLockReady)
	{
		g_stateLockReady = false;
		DeleteCriticalSection(&g_stateLock);
	}

	if (g_capabilityLockReady)
	{
		g_capabilityLockReady = false;
		DeleteCriticalSection(&g_capabilityLock);
	}

	Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged()
{
	LoadSettings();
}
