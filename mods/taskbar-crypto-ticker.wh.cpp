// ==WindhawkMod==
// @id              taskbar-crypto-ticker
// @name            Taskbar Crypto Ticker
// @description     Live Binance prices in a quiet, clock-style taskbar display
// @version         2.0.0
// @author          amy
// @github          https://github.com/4U0
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lwinhttp -lgdi32 -lcomctl32 -ladvapi32 -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Live Taskbar Crypto Ticker

![Live ticker appearance](https://github.com/user-attachments/assets/025756c2-d2d7-458d-b164-d88840005250)

Appearance preview: the actual text renderer with live Binance data in a preview
window. On the taskbar, only the two text lines are shown, with no window frame.

Small, monochrome, two-line text on a transparent background, like the clock.
The first line shows the asset and last traded price; the second shows the
quote currency and 24-hour change. No cards, charts, badges, or flashing colors.

Binance aggregate-trade WebSocket events update the price as trades arrive.
The separate 24-hour ticker stream updates statistics about once per second.
The display redraws at most ten times per second, always using the latest price.
There is no price polling interval and no API key.

Default symbols: BTCUSDT, ETHUSDT, SOLUSDT. These prices are in USDT, not USD.
Use comma-separated Binance Spot symbols (maximum 8). Symbols are validated
syntactically; an unlisted symbol shows "No data" after 15 seconds.
Click text to cycle symbols. Right-click text to reconnect. Hover for status.
Set rotation to 0 to stay on one symbol. All configured symbols stream together.

Place the overlay in an empty part of the primary horizontal taskbar using its
right-edge distance and width settings. It does not reserve space or move icons.
Designed for Windows 10/11. Runtime tested on x64; x86 and ARM64 are compile-tested only.
Secondary and vertical taskbars are not supported.
Automatic reconnection uses backoff; disabling cancels pending network I/O.
Binance availability depends on the user's network and region. A disconnection
shows "Offline" rather than presenting the last price as live. The tooltip keeps
the last successful price. Settings changes restart the ticker.
Runs in a dedicated Windhawk tool process, independently of Explorer. If Explorer
restarts, the ticker automatically follows the newly created taskbar.

https://github.com/binance/binance-spot-api-docs/blob/master/web-socket-streams.md
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- symbols: BTCUSDT,ETHUSDT,SOLUSDT
  $name: Binance Spot symbols
  $description: Comma-separated symbols, maximum 8. USDT is not USD.
- rotateSeconds: 5
  $name: Rotate symbol every (seconds)
  $description: Set 0 to keep the selected symbol. Otherwise 2–120 seconds.
- rightOffset: 165
  $name: Distance from the taskbar's right edge
  $description: Logical pixels. Increase to move left, away from the clock and tray.
- tickerWidth: 145
  $name: Text area width
  $description: Logical pixels, clamped to 110–300.
- textSize: 11
  $name: Text size
  $description: Logical pixels, clamped to 9–16. Regular Segoe UI, like the clock.
- theme: auto
  $name: Text color
  $options:
  - auto: Follow Windows
  - dark: White text for a dark taskbar
  - light: Dark text for a light taskbar
*/
// ==/WindhawkModSettings==
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <gdiplus.h>
#include <atomic>
#include <array>
#include <stdexcept>
#include <windhawk_api.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <locale>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace ticker {

// Bounded JSON parser for combined Binance stream envelopes. Handles nested
// objects, arrays, strings, numbers and literals; rejects duplicate keys.
struct Json {
    char kind = 0;
    std::string value;
    std::map<std::string, Json> fields;
    const Json& at(const char* key) const {
        auto it = fields.find(key);
        if (it == fields.end()) throw std::runtime_error("missing field");
        return it->second;
    }
};
struct Parser {
    const std::string& text;
    size_t p = 0;
    void ws() { while (p < text.size() && (text[p]==' ' || text[p]=='\r' || text[p]=='\n' || text[p]=='\t')) ++p; }
    bool take(char c) { ws(); if (p == text.size() || text[p] != c) return false; ++p; return true; }
    void need(char c) { if (!take(c)) throw std::runtime_error("invalid JSON"); }
    std::string str() {
        need('"'); std::string value;
        while (p < text.size()) {
            unsigned char c = text[p++];
            if (c == '"') return value;
            if (c < 32) throw std::runtime_error("control character");
            if (c == '\\') {
                if (p == text.size()) throw std::runtime_error("escape");
                c = text[p++];
                if (c == 'u') {
                    unsigned n = 0;
                    for (int i=0; i<4; ++i) {
                        if (p == text.size()) throw std::runtime_error("unicode");
                        char h = text[p++];
                        int v = h>='0' && h<='9' ? h-'0' : h>='a' && h<='f' ? h-'a'+10 : h>='A' && h<='F' ? h-'A'+10 : -1;
                        if (v < 0) throw std::runtime_error("unicode");
                        n = n*16 + v;
                    }
                    value += n < 128 ? static_cast<char>(n) : '?';
                    continue;
                }
                switch (c) {
                case '"': case '\\': case '/': break;
                case 'b': c='\b'; break; case 'f': c='\f'; break;
                case 'n': c='\n'; break; case 'r': c='\r'; break; case 't': c='\t'; break;
                default: throw std::runtime_error("escape");
                }
            }
            value += static_cast<char>(c);
        }
        throw std::runtime_error("unterminated string");
    }
    Json parse(int depth=0) {
        if (depth > 12) throw std::runtime_error("nesting");
        ws(); Json j;
        if (take('{')) {
            j.kind='o'; if (take('}')) return j;
            do { auto key=str(); need(':'); if (!j.fields.emplace(key, parse(depth+1)).second) throw std::runtime_error("duplicate"); } while (take(','));
            need('}'); return j;
        }
        if (take('[')) {
            j.kind='a'; if (take(']')) return j;
            do { parse(depth+1); } while (take(',')); need(']'); return j;
        }
        if (p < text.size() && text[p]=='"') { j.kind='s'; j.value=str(); return j; }
        for (auto literal : {"true","false","null"}) {
            size_t n = strlen(literal);
            if (text.compare(p,n,literal)==0) { p+=n; j.kind='l'; j.value=literal; return j; }
        }
        size_t start=p;
        if (p<text.size() && text[p]=='-') ++p;
        auto digits = [&] { size_t begin=p; while (p<text.size() && text[p]>='0' && text[p]<='9') ++p; if (begin==p) throw std::runtime_error("number"); };
        if (p<text.size() && text[p]=='0') ++p; else digits();
        if (p<text.size() && text[p]=='.') { ++p; digits(); }
        if (p<text.size() && (text[p]=='e' || text[p]=='E')) { ++p; if (p<text.size() && (text[p]=='+' || text[p]=='-')) ++p; digits(); }
        j.kind='n'; j.value=text.substr(start,p-start); return j;
    }
};
double Number(const Json& j) {
    if (j.kind!='s' && j.kind!='n') throw std::runtime_error("not numeric");
    // Reuse JSON's numeric grammar, including complete-consumption checking.
    Parser parser{j.value}; auto n=parser.parse(); parser.ws();
    if (n.kind!='n' || parser.p!=j.value.size()) throw std::runtime_error("bad number");
    std::istringstream in(j.value); in.imbue(std::locale::classic()); double v;
    in >> v;
    if (in.fail() || !in.eof() || !std::isfinite(v)) throw std::runtime_error("nonfinite");
    return v;
}
ULONGLONG Id(const Json& j) {
    if (j.kind!='n' || j.value.empty() || !std::all_of(j.value.begin(),j.value.end(),[](char c){return c>='0'&&c<='9';})) throw std::runtime_error("bad ID");
    size_t end=0; auto value=std::stoull(j.value,&end);
    if (end!=j.value.size()) throw std::runtime_error("bad ID");
    return value;
}
std::vector<std::wstring> ParseProducts(const std::wstring& value) {
    std::vector<std::wstring> result;
    std::wistringstream stream(value);
    std::wstring item;
    while (std::getline(stream, item, L',') && result.size() < 8) {
        auto start = item.find_first_not_of(L" \t\r\n");
        if (start == std::wstring::npos) continue;
        item = item.substr(start, item.find_last_not_of(L" \t\r\n") - start + 1);
        for (auto& c : item) if (c >= L'a' && c <= L'z') c -= L'a' - L'A';
        if (item.size() < 5 || item.size() > 30 ||
            !std::all_of(item.begin(), item.end(), [](wchar_t c) {
                return (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9');
            })) continue;
        if (std::find(result.begin(), result.end(), item) == result.end()) result.push_back(item);
    }
    return result;
}

std::wstring FormatPrice(double price) {
    std::wostringstream out;
    out.imbue(std::locale::classic());
    out << std::fixed << std::setprecision(price >= 1 ? 2 : price >= .01 ? 4 : 8) << price;
    auto value = out.str();
    auto dot = value.find(L'.');
    for (int i = static_cast<int>(dot) - 3; i > 0; i -= 3) value.insert(i, L",");
    return value;
}

struct Quote {
    std::wstring product;
    double price = 0, change = 0;
    ULONGLONG received = 0, statsReceived = 0, tradeId = 0, statsTime = 0;
    SYSTEMTIME localTime{};
};
struct State {
    HANDLE stop = nullptr, refresh = nullptr, ui = nullptr, worker = nullptr;
    HINSTANCE module = nullptr;
    std::mutex mutex;
    std::vector<Quote> quotes;
    bool connected = false;
    ULONGLONG connectedAt = 0;
    std::wstring error = L"Connecting";
    int rotateSeconds = 5, right = 165, width = 145, font = 11;
    std::wstring theme;
    HWND window = nullptr, tooltip = nullptr;
    std::wstring tipText, lastDraw;
    size_t selected = 0;
    ULONGLONG rotated = 0;
    UINT dpi = 96;
    bool dark = true;
};
State* g = nullptr;
constexpr wchar_t kClass[] = L"Windhawk.TaskbarCryptoTicker.2";
bool Stopping(State* s) { return WaitForSingleObject(s->stop,0)==WAIT_OBJECT_0; }

// All prices are ordered by Binance's trade IDs. A delayed 24h ticker cannot
// overwrite a more recent trade price, even when its event timestamp is newer.
bool Apply(State* s, const std::string& text) {
    if (text.size()>65536) return false;
    try {
        Parser parser{text}; auto root=parser.parse(); parser.ws();
        if (parser.p!=text.size()) return false;
        const auto& d=root.at("data");
        const auto& event=d.at("e");
        if (event.kind!='s') return false;
        bool stats=event.value=="24hrTicker";
        if (!stats && event.value!="aggTrade") return false;
        const auto& symbol=d.at("s");
        if (symbol.kind!='s') return false;
        double price=Number(d.at(stats?"c":"p"));
        if (price<=0) return false;
        auto id=Id(d.at(stats?"L":"l"));
        auto time=Id(d.at("E"));
        double change=stats?Number(d.at("P")):0;
        std::wstring product(symbol.value.begin(),symbol.value.end());
        std::lock_guard lock(s->mutex);
        for (auto& q:s->quotes) if (q.product==product) {
            auto now=GetTickCount64();
            if (!q.received || id>=q.tradeId) { q.price=price; q.tradeId=id; GetLocalTime(&q.localTime); }
            q.received=now;
            if (stats && time>=q.statsTime) { q.change=change; q.statsTime=time; q.statsReceived=now; }
            return true;
        }
    } catch (const std::exception&) {}
    return false;
}
std::wstring StreamPath(const State* s) {
    std::wstring path=L"/stream?streams=";
    for (const auto& q:s->quotes) {
        if (path.back()!=L'=') path+=L'/';
        std::wstring symbol=q.product;
        for (auto& c:symbol) if(c>=L'A'&&c<=L'Z') c+=L'a'-L'A';
        path+=symbol+L"@aggTrade/"+symbol+L"@ticker";
    }
    return path;
}
struct Internet {
    HINTERNET h=nullptr;
    explicit Internet(HINTERNET value=nullptr):h(value){}
    ~Internet(){ if(h) WinHttpCloseHandle(h); }
    Internet(const Internet&)=delete;
};
struct Io {
    HANDLE done=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    HANDLE closed=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    std::atomic<DWORD> status{0};
    DWORD error=0, bytes=0;
    WINHTTP_WEB_SOCKET_BUFFER_TYPE type=WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE;
    ~Io(){if(done)CloseHandle(done);if(closed)CloseHandle(closed);}
    void reset(){ResetEvent(done);status=0;error=0;bytes=0;}
};
void CALLBACK Callback(HINTERNET, DWORD_PTR context, DWORD status, void* data, DWORD size) {
    auto io=reinterpret_cast<Io*>(context); if(!io)return;
    if(status==WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING){SetEvent(io->closed);return;}
    if(status==WINHTTP_CALLBACK_STATUS_REQUEST_ERROR){
        io->error=size>=sizeof(WINHTTP_ASYNC_RESULT)?static_cast<WINHTTP_ASYNC_RESULT*>(data)->dwError:ERROR_INVALID_DATA;
    } else if(status==WINHTTP_CALLBACK_STATUS_READ_COMPLETE){
        if(size<sizeof(WINHTTP_WEB_SOCKET_STATUS)){io->error=ERROR_INVALID_DATA;}
        else {auto result=static_cast<WINHTTP_WEB_SOCKET_STATUS*>(data);io->bytes=result->dwBytesTransferred;io->type=result->eBufferType;}
    } else if(status!=WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE && status!=WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE) return;
    io->status.store(status,std::memory_order_release); SetEvent(io->done);
}
struct Async {
    Io io;
    HINTERNET h=nullptr;
    bool watched=false;
    bool watch() {
        if(!h || !io.done || !io.closed)return false;
        DWORD_PTR context=reinterpret_cast<DWORD_PTR>(&io);
        if(!WinHttpSetOption(h,WINHTTP_OPTION_CONTEXT_VALUE,&context,sizeof(context)))return false;
        watched=WinHttpSetStatusCallback(h,Callback,WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS|WINHTTP_CALLBACK_FLAG_HANDLES,0)!=WINHTTP_INVALID_STATUS_CALLBACK;
        return watched;
    }
    void close(){
        if(!h)return;
        WinHttpCloseHandle(h);h=nullptr;
        // Keep callback context and receive buffer alive until the last callback.
        if(watched)WaitForSingleObject(io.closed,INFINITE);
    }
    ~Async(){close();}
};
bool Await(State* s, Io& io, DWORD expected, DWORD timeout, std::wstring& error) {
    HANDLE events[]={s->stop,s->refresh,io.done};
    DWORD result=WaitForMultipleObjects(3,events,FALSE,timeout);
    if(result!=WAIT_OBJECT_0+2){error=result==WAIT_TIMEOUT?L"Stream timeout":L"Reconnecting";return false;}
    auto status=io.status.load(std::memory_order_acquire);
    if(status!=expected || io.error){error=L"Network error "+std::to_wstring(io.error);return false;}
    return true;
}
void Connection(State* s, bool connected, const std::wstring& error) {
    std::lock_guard lock(s->mutex);
    s->connected=connected;s->error=error;
    if(connected)s->connectedAt=GetTickCount64();
}
bool Stream(State* s) {
    std::wstring error=L"Connection failed";
    Connection(s,false,L"Connecting");
    struct Finish {
        State* state; std::wstring& message;
        ~Finish(){
            std::lock_guard lock(state->mutex);
            state->connected=false;state->error.swap(message);
        }
    } finish{s,error};
    Internet session(WinHttpOpen(L"WindhawkCryptoTicker/2.0",WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,nullptr,nullptr,WINHTTP_FLAG_ASYNC));
    if(!session.h)return false;
    WinHttpSetTimeouts(session.h,5000,5000,5000,10000);
    Internet connection(WinHttpConnect(session.h,L"stream.binance.com",443,0));
    if(!connection.h)return false;
    Async request;
    auto path=StreamPath(s);
    request.h=WinHttpOpenRequest(connection.h,L"GET",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE);
    if(!request.watch())return false;
    DWORD redirect=WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    WinHttpSetOption(request.h,WINHTTP_OPTION_REDIRECT_POLICY,&redirect,sizeof(redirect));
    if(!WinHttpSetOption(request.h,WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET,nullptr,0))return false;
    auto run=[&]()->bool {
        request.io.reset();
        if(!WinHttpSendRequest(request.h,nullptr,0,nullptr,0,0,reinterpret_cast<DWORD_PTR>(&request.io)) && GetLastError()!=ERROR_IO_PENDING)return false;
        if(!Await(s,request.io,WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE,15000,error))return false;
        request.io.reset();
        if(!WinHttpReceiveResponse(request.h,nullptr) && GetLastError()!=ERROR_IO_PENDING)return false;
        if(!Await(s,request.io,WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE,15000,error))return false;
        DWORD status=0,size=sizeof(status);
        if(!WinHttpQueryHeaders(request.h,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,nullptr,&status,&size,nullptr))return false;
        if(status!=101){error=L"Binance HTTP "+std::to_wstring(status);return false;}
        // Declared before socket so cancellation completes before buffer dies.
        std::array<char,8192> buffer{};
        Async socket;
        if(!socket.io.done || !socket.io.closed)return false;
        socket.h=WinHttpWebSocketCompleteUpgrade(request.h,reinterpret_cast<DWORD_PTR>(&socket.io));
        if(!socket.h)return false;
        socket.watched=true; // Callback inherited from request, context supplied above.
        request.close();
        Connection(s,true,L"");
        std::string message;
        auto started=GetTickCount64();
        bool received=false;
        while(!Stopping(s)){
            socket.io.reset();
            DWORD code=WinHttpWebSocketReceive(socket.h,buffer.data(),static_cast<DWORD>(buffer.size()),nullptr,nullptr);
            if(code!=NO_ERROR && code!=ERROR_IO_PENDING){error=L"Socket error "+std::to_wstring(code);break;}
            if(!Await(s,socket.io,WINHTTP_CALLBACK_STATUS_READ_COMPLETE,45000,error))break;
            auto type=socket.io.type;
            if(type==WINHTTP_WEB_SOCKET_CLOSE_BUFFER_TYPE){error=L"Server disconnected";break;}
            if(type!=WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE && type!=WINHTTP_WEB_SOCKET_UTF8_FRAGMENT_BUFFER_TYPE){error=L"Unexpected binary data";break;}
            if(socket.io.bytes>buffer.size() || message.size()+socket.io.bytes>65536){error=L"Message too large";break;}
            message.append(buffer.data(),socket.io.bytes);
            if(type==WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE){received=Apply(s,message)||received;message.clear();}
            // Proactively renew before Binance's 24-hour connection limit.
            if(GetTickCount64()-started>23ULL*60*60*1000){error=L"Renewing connection";break;}
        }
        return received;
    };
    bool received=run();
    return received;
}
DWORD WINAPI Worker(void* context) {
    auto s=static_cast<State*>(context);DWORD retry=1000;
    while(!Stopping(s)){
        ResetEvent(s->refresh);
        bool received=false;
        try {
            received=Stream(s);
        } catch(...) {
            Wh_Log(L"Stream exception; retrying with backoff");
            // Avoid allocating an error string while recovering from bad_alloc.
            std::lock_guard lock(s->mutex);
            s->connected=false;s->error.clear();
        }
        if(Stopping(s))break;
        bool manual=WaitForSingleObject(s->refresh,0)==WAIT_OBJECT_0;
        ResetEvent(s->refresh);
        if(received)retry=1000;
        HANDLE events[]={s->stop,s->refresh};
        if(WaitForMultipleObjects(2,events,FALSE,manual?250:retry)==WAIT_OBJECT_0)break;
        retry=std::min<DWORD>(retry*2,30000);
    }
    return 0;
}
bool Dark(State* s) {
    if (s->theme == L"dark") return true;
    if (s->theme == L"light") return false;
    DWORD light = 0, size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    return !light;
}
struct View {
    Quote q;
    bool connected;
    ULONGLONG connectedAt;
    std::wstring error;
};
View Current(State* s) {
    std::lock_guard lock(s->mutex);
    return {s->quotes[s->selected],s->connected,s->connectedAt,s->error};
}
bool Stale(const View& v) {return !v.connected || !v.q.received || v.q.received<v.connectedAt || GetTickCount64()-v.q.received>8000;}
std::pair<std::wstring,std::wstring> SplitSymbol(const std::wstring& symbol) {
    for(auto suffix:{L"FDUSD",L"USDT",L"USDC",L"TUSD",L"BUSD",L"USDP",L"DAI",L"BTC",L"ETH",L"BNB",L"EUR",L"TRY",L"BRL",L"USD",L"JPY",L"PLN",L"ARS",L"ZAR",L"AUD",L"GBP",L"RUB",L"UAH"}) {
        size_t n=wcslen(suffix);
        if(symbol.size()>n && symbol.compare(symbol.size()-n,n,suffix)==0)return {symbol.substr(0,symbol.size()-n),suffix};
    }
    return {symbol,L""};
}
std::pair<std::wstring,std::wstring> Lines(const View& v) {
    auto [base,quote]=SplitSymbol(v.q.product);
    if(Stale(v)) {
        std::wstring status=!v.connected?(v.error==L"Connecting"?L"Connecting":L"Offline"):
            GetTickCount64()-v.connectedAt>15000?L"No data":L"Waiting";
        return {base+L"  "+status,quote.empty()?v.q.product:quote};
    }
    std::wstring bottom=quote;
    if(v.q.statsReceived && GetTickCount64()-v.q.statsReceived<=8000) {
        wchar_t percent[48];swprintf_s(percent,L"%+.2f%%",v.q.change);
        if(!bottom.empty())bottom+=L"  ";bottom+=percent;
    } else {if(!bottom.empty())bottom+=L"  ";bottom+=L"24h --";}
    return {base+L"  "+FormatPrice(v.q.price),bottom};
}
void DrawTextLayer(Gdiplus::Graphics& graphics, int width, int height, float scale,
                   int textSize, bool dark, const std::pair<std::wstring,std::wstring>& lines) {
    using namespace Gdiplus;
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    SolidBrush ink(dark?Color(255,250,250,250):Color(255,28,28,28));
    StringFormat format;
    format.SetAlignment(StringAlignmentFar);format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);
    format.SetTrimming(StringTrimmingEllipsisCharacter);
    float lineHeight=(textSize+3)*scale;
    float top=(height-lineHeight*2)/2;
    Font font(L"Segoe UI",textSize*scale,FontStyleRegular,UnitPixel);
    RectF first(3*scale,top,width-7*scale,lineHeight);
    RectF second(3*scale,top+lineHeight,width-7*scale,lineHeight);
    graphics.DrawString(lines.first.c_str(),-1,&font,first,&format,&ink);
    graphics.DrawString(lines.second.c_str(),-1,&font,second,&format,&ink);
}
void Render(State* s) {
    if(!IsWindowVisible(s->window))return;
    RECT r{};GetWindowRect(s->window,&r);int w=r.right-r.left,h=r.bottom-r.top;
    if(w<=0 || h<=0)return;
    auto lines=Lines(Current(s));bool dark=s->dark;
    auto key=lines.first+L"\n"+lines.second+std::to_wstring(w)+L":"+std::to_wstring(h)+(dark?L"d":L"l");
    if(key==s->lastDraw)return;
    BITMAPINFO bi{};bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=w;
    bi.bmiHeader.biHeight=-h;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;bi.bmiHeader.biCompression=BI_RGB;
    void* bits=nullptr;HDC screen=GetDC(nullptr);HDC memory=CreateCompatibleDC(screen);
    HBITMAP bitmap=CreateDIBSection(screen,&bi,DIB_RGB_COLORS,&bits,nullptr,0);
    if(bitmap && memory) {
        auto old=SelectObject(memory,bitmap);
        memset(bits,0,static_cast<size_t>(w)*h*4);
        {
            Gdiplus::Bitmap surface(w,h,w*4,PixelFormat32bppPARGB,static_cast<BYTE*>(bits));
            Gdiplus::Graphics graphics(&surface);
            DrawTextLayer(graphics,w,h,s->dpi/96.0f,s->font,dark,lines);
            graphics.Flush(Gdiplus::FlushIntentionSync);
        }
        POINT destination{r.left,r.top},origin{0,0};SIZE size{w,h};BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
        if(UpdateLayeredWindow(s->window,screen,&destination,&size,memory,&origin,0,&blend,ULW_ALPHA))s->lastDraw=key;
        SelectObject(memory,old);
    }
    if(bitmap)DeleteObject(bitmap);if(memory)DeleteDC(memory);ReleaseDC(nullptr,screen);
}
void Position(State* s, HWND bar) {
    RECT r{};
    if (!bar || !IsWindowVisible(bar) || !GetWindowRect(bar, &r)) {
        if(IsWindowVisible(s->window))ShowWindow(s->window, SW_HIDE); return;
    }
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromWindow(bar, MONITOR_DEFAULTTOPRIMARY), &mi);
    RECT visible{};
    IntersectRect(&visible, &r, &mi.rcMonitor);
    if (r.bottom - r.top > r.right - r.left || visible.bottom - visible.top < 20) {
        if(IsWindowVisible(s->window))ShowWindow(s->window, SW_HIDE); return;
    }
    UINT dpi = GetDpiForWindow(bar);
    if (!dpi) dpi = 96;
    if(s->dpi!=dpi){s->dpi=dpi;s->lastDraw.clear();}
    int width = std::min(MulDiv(s->width, dpi, 96), static_cast<int>(r.right - r.left));
    int height = std::min(MulDiv(38, dpi, 96), static_cast<int>(visible.bottom - visible.top));
    int x = std::clamp(static_cast<int>(r.right) - MulDiv(s->right, dpi, 96) - width,
        static_cast<int>(r.left), static_cast<int>(r.right) - width);
    int y = visible.top + (visible.bottom - visible.top - height) / 2;
    // Follow the shell's z-order, including its fullscreen handling. Do not own
    // the overlay with the foreign taskbar window: it must survive shell restarts.
    HWND previous=GetWindow(bar,GW_HWNDPREV);
    bool barTopmost=(GetWindowLongPtrW(bar,GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
    bool topmost=(GetWindowLongPtrW(s->window,GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
    bool sameOrder=previous==s->window && topmost==barTopmost;
    HWND after=previous==s->window?GetWindow(s->window,GW_HWNDPREV):previous;
    if(!after)after=barTopmost?HWND_TOPMOST:HWND_TOP;
    // Explicitly leave the topmost band before following a non-topmost taskbar.
    if(topmost && !barTopmost) {
        SetWindowPos(s->window,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        previous=GetWindow(bar,GW_HWNDPREV);
        after=previous==s->window?GetWindow(s->window,GW_HWNDPREV):previous;
        if(!after)after=HWND_TOP;
        sameOrder=previous==s->window;
    }
    RECT current{};GetWindowRect(s->window,&current);
    bool sameRect=current.left==x && current.top==y && current.right==x+width && current.bottom==y+height;
    bool shown=IsWindowVisible(s->window)!=FALSE;
    if(sameRect && sameOrder && shown)return;
    UINT flags=SWP_NOACTIVATE;
    if(sameRect)flags|=SWP_NOMOVE|SWP_NOSIZE;
    if(sameOrder)flags|=SWP_NOZORDER;
    if(!shown)flags|=SWP_SHOWWINDOW;
    SetWindowPos(s->window,after,x,y,width,height,flags);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    auto s = reinterpret_cast<State*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        s = static_cast<State*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s));
    }
    if (!s) return DefWindowProcW(window, message, wp, lp);
    switch (message) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_SETTINGCHANGE:
    case WM_THEMECHANGED:
        s->dark=Dark(s);s->lastDraw.clear();Render(s);return 0;
    case WM_LBUTTONUP:
        s->selected = (s->selected + 1) % s->quotes.size();
        s->rotated = GetTickCount64();
        InvalidateRect(window, nullptr, FALSE);
        return 0;
    case WM_RBUTTONUP: SetEvent(s->refresh); return 0;
    case WM_TIMER:
        if (s->rotateSeconds && GetTickCount64() - s->rotated >= static_cast<ULONGLONG>(s->rotateSeconds) * 1000) {
            s->selected = (s->selected + 1) % s->quotes.size();
            s->rotated = GetTickCount64();
        }
        Position(s,FindWindowW(L"Shell_TrayWnd",nullptr));
        Render(s);
        return 0;
    case WM_NOTIFY:
        if (reinterpret_cast<NMHDR*>(lp)->code == TTN_GETDISPINFOW) {
            auto v=Current(s);auto& q=v.q;
            s->tipText=q.product+L" | Binance Spot WebSocket\n";
            if(q.received) {
                s->tipText+=L"Last price: "+FormatPrice(q.price)+L"\n";
                s->tipText+=L"Last event "+std::to_wstring((GetTickCount64()-q.received)/1000)+L"s ago\n";
            }
            s->tipText+=v.connected?(Stale(v)?L"Waiting for symbol data\n":L"Live aggregate trades; 24h statistics ~1s\n"):v.error+L"\n";
            s->tipText+=L"Click text: next symbol | Right-click: reconnect";
            reinterpret_cast<NMTTDISPINFOW*>(lp)->lpszText = s->tipText.data();
        }
        return 0;
    case WM_PAINT: { PAINTSTRUCT ps;BeginPaint(window,&ps);EndPaint(window,&ps);Render(s);return 0; }
    }
    return DefWindowProcW(window, message, wp, lp);
}

DWORD WINAPI UiThread(void* context) {
    auto s = static_cast<State*>(context);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ULONG_PTR gdiplus=0;Gdiplus::GdiplusStartupInput startup;
    if(Gdiplus::GdiplusStartup(&gdiplus,&startup,nullptr)!=Gdiplus::Ok)return 1;
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&controls);
    WNDCLASSEXW cls{};
    cls.cbSize = sizeof(cls);
    cls.lpfnWndProc = WindowProc; cls.hInstance = s->module;
    cls.lpszClassName = kClass; cls.hCursor = LoadCursorW(nullptr, IDC_HAND);
    if (!RegisterClassExW(&cls)) { Wh_Log(L"Cannot register ticker window"); Gdiplus::GdiplusShutdown(gdiplus); return 1; }
    s->window = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        kClass, L"Crypto ticker", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, s->module, s);
    if (s->window) {
        s->tooltip = CreateWindowExW(WS_EX_TOPMOST | WS_EX_NOACTIVATE, TOOLTIPS_CLASSW,
            nullptr, WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, s->window, nullptr, s->module, nullptr);
        if (s->tooltip) {
            TOOLINFOW ti{};
            ti.cbSize = sizeof(ti);
            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
            ti.hwnd = s->window; ti.uId = reinterpret_cast<UINT_PTR>(s->window);
            ti.lpszText = LPSTR_TEXTCALLBACKW;
            SendMessageW(s->tooltip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
            SendMessageW(s->tooltip, TTM_SETMAXTIPWIDTH, 0, 450);
        }
        s->rotated = GetTickCount64();
        s->dark = Dark(s);
        Position(s,FindWindowW(L"Shell_TrayWnd",nullptr));
        if (SetTimer(s->window, 1, 100, nullptr)) {
            while (!Stopping(s) && IsWindow(s->window)) {
                DWORD result = MsgWaitForMultipleObjects(1, &s->stop, FALSE, INFINITE, QS_ALLINPUT);
                if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
                MSG message;
                while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&message); DispatchMessageW(&message);
                }
            }
        }
        if (IsWindow(s->tooltip)) DestroyWindow(s->tooltip);
        if (IsWindow(s->window)) DestroyWindow(s->window);
    } else Wh_Log(L"Cannot create ticker window");
    Gdiplus::GdiplusShutdown(gdiplus);
    UnregisterClassW(kClass, s->module);
    return 0;
}

void Stop() {
    if (!g) return;
    if (g->stop) SetEvent(g->stop);
    // Never unload code until both threads have returned.
    if (g->ui) { WaitForSingleObject(g->ui, INFINITE); CloseHandle(g->ui); }
    if (g->worker) { WaitForSingleObject(g->worker, INFINITE); CloseHandle(g->worker); }
    if (g->refresh) CloseHandle(g->refresh);
    if (g->stop) CloseHandle(g->stop);
    delete g; g = nullptr;
}
std::wstring Setting(const wchar_t* name) {
    return WindhawkUtils::StringSetting::make(name).get();
}
} // namespace ticker

BOOL WhTool_ModInit() {
    using namespace ticker;
    try {
        g = new State;
        auto products = ParseProducts(Setting(L"symbols"));
        if (products.empty()) { Wh_Log(L"No valid Binance symbols configured"); Stop(); return FALSE; }
        for (const auto& product : products) { Quote q; q.product = product; g->quotes.push_back(q); }

        g->rotateSeconds = Wh_GetIntSetting(L"rotateSeconds");
        if(g->rotateSeconds)g->rotateSeconds=std::clamp(g->rotateSeconds,2,120);
        g->right = std::clamp(Wh_GetIntSetting(L"rightOffset"), 0, 10000);
        g->width = std::clamp(Wh_GetIntSetting(L"tickerWidth"), 110, 300);
        g->font = std::clamp(Wh_GetIntSetting(L"textSize"), 9, 16);
        g->theme = Setting(L"theme");
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&WhTool_ModInit), &g->module);
        g->stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g->refresh = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g->stop || !g->refresh) { Stop(); return FALSE; }
        g->worker = CreateThread(nullptr, 0, Worker, g, 0, nullptr);
        g->ui = CreateThread(nullptr, 0, UiThread, g, 0, nullptr);
        if (!g->worker || !g->ui) { Stop(); return FALSE; }
        return TRUE;
    } catch (...) { Stop(); return FALSE; }
}
void WhTool_ModUninit() { ticker::Stop(); }
void WhTool_ModSettingsChanged() {
    ticker::Stop();
    if(!WhTool_ModInit())Wh_Log(L"Ticker restart failed; correct settings or toggle the mod to retry");
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
