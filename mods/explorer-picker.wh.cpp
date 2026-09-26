// ==WindhawkMod==
// @id              explorer-picker
// @name            Explorer Picker
// @description     Replaces the Open, Save As and folder picker dialogs of every program with a real Explorer window that has a File name / Files of type bar at the bottom
// @name:ru         Проводник вместо окон выбора файла
// @description:ru  Заменяет окна «Открыть», «Сохранить как» и выбора папки во всех программах настоящим окном Проводника с полями «Имя файла» и «Тип файлов» внизу
// @version         1.1.0
// @author          appEW
// @github          https://github.com/appEW
// @include         *
// @exclude         windhawk.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -ladvapi32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Picker

The Open, Save As and folder picker dialogs of every program are replaced with
a real Explorer window (`explorer.exe`) with a File name / Files of type / Open /
Cancel bar at the bottom. The folder tree, the toolbars, the address bar and
the status bar are the same as in any other Explorer window, along with
whatever other mods change them.

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

## How it works

The one mod plays both parts:

* in `explorer.exe` it opens the picker window, builds the bar at the bottom
  and reads the current folder and the selection itself;
* in every other process it intercepts `GetOpenFileName` / `GetSaveFileName`
  (Unicode and ANSI) and `IFileOpenDialog` / `IFileSaveDialog`, passes the
  request to Explorer, and hands the result back to the program in the form it
  expects.

Supported: opening one or several files, saving with a confirmation before
replacing a file, picking a folder, file type filters, `IFileDialogEvents`
events, the extra controls of `IFileDialogCustomize` (check boxes, combo boxes,
radio buttons, edit boxes, buttons, labels, groups, menus), and the templates
and hook procedures of old-style dialogs with their check boxes. Programs
running as administrator are supported too.

If a program asks for something Explorer cannot do, or Explorer is not
available, the normal dialog is shown - the program never breaks.

Services, system accounts, critical processes and AppContainer processes are
skipped. Single programs can be excluded on the *Advanced* tab.

The texts of the bar and of its messages follow the Windows display language:
Russian on a Russian Windows, the wording of the English common dialog
everywhere else.

---

## По-русски

Окна «Открыть», «Сохранить как» и «Выбор папки» всех программ заменяются
настоящим окном Проводника (explorer.exe) с нижней панелью «Имя файла» /
«Тип файлов» / «Открыть» / «Отмена». Дерево папок, панели инструментов,
адресная строка и строка состояния — те же, что в обычном Проводнике.

Один мод выполняет обе роли:

* в explorer.exe — открывает окно выбора, встраивает нижнюю панель и сам
  читает текущую папку и выделение;
* в остальных процессах — перехватывает `GetOpenFileName`/`GetSaveFileName`
  (Unicode и ANSI) и `IFileOpenDialog`/`IFileSaveDialog`, передаёт запрос в
  Проводник и возвращает программе результат в привычной для неё форме.

Поддерживаются: один и несколько файлов, сохранение с подтверждением замены,
выбор папки, фильтры типов, события `IFileDialogEvents`, дополнительные
элементы `IFileDialogCustomize` (флажки, списки, переключатели, поля, кнопки,
надписи, группы, меню), шаблоны/обработчики старых диалогов с флажками.
Программы с правами администратора тоже поддерживаются.

Если программа запрашивает то, что нельзя повторить в Проводнике, или
Проводник недоступен, показывается обычное окно — программа не ломается.

Службы, системные учётные записи, критические процессы и AppContainer
пропускаются. Отдельные программы можно исключить на вкладке «Дополнительно».

Надписи на нижней панели и тексты сообщений следуют языку интерфейса Windows:
в русской Windows - по-русски, в остальных - как в английском стандартном окне
выбора файла.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- explorerDialogs: true
  $name: Dialogs opened by Explorer itself
  $name:ru: Окна выбора самого Проводника
  $description: Also replace the dialogs explorer.exe opens itself, such as Browse... in the Run box.
  $description:ru: Заменять и окна выбора, которые открывает explorer.exe (например, «Обзор» в окне «Выполнить»).
- elevatedApps: true
  $name: Programs running as administrator
  $name:ru: Программы с правами администратора
  $description: Let the Explorer window, which runs with normal rights, hand the choice back to programs running as administrator.
  $description:ru: Разрешить окну Проводника (обычные права) возвращать выбор программам, запущенным от имени администратора.
- ansiApps: true
  $name: Old ANSI programs
  $name:ru: Старые ANSI-программы
  $description: Also replace the dialogs of programs that call GetOpenFileNameA / GetSaveFileNameA.
  $description:ru: Заменять и окна программ, которые вызывают GetOpenFileNameA / GetSaveFileNameA.
- rememberFolders: true
  $name: Remember the folder
  $name:ru: Запоминать папку
  $description: When a program does not say which folder to open, open the one last chosen in that program.
  $description:ru: Если программа не указала папку, открывать последнюю выбранную в этой программе.
- logDirectory: ''
  $name: Log folder
  $name:ru: Папка журналов
  $description: If set, every process that shows a picker writes explorer-picker-PID.log there. Leave empty for no logs.
  $description:ru: Если указана, процессы с открытым окном выбора пишут туда explorer-picker-PID.log. Пусто - без журналов.
*/
// ==/WindhawkModSettings==

#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <dlgs.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <windhawk_api.h>
#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <new>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

std::atomic<bool> g_unloading=false;
HMODULE g_module=nullptr;
bool g_pinned=false;
std::mutex g_pinLock;

// Once any callback, window class or COM object of this DLL can outlive the
// mod (a retained dialog proxy, an open picker), keep the code mapped.
void PinModule(){
    std::lock_guard lock(g_pinLock);if(g_pinned)return;
    HMODULE pinned=nullptr;
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&PinModule),&pinned))g_pinned=true;
}

// The texts the picker shows itself follow the Windows display language:
// Russian on a Russian Windows, the wording of the English common dialog
// everywhere else.
const wchar_t* UiText(const wchar_t* english,const wchar_t* russian){
    static const bool isRussian=PRIMARYLANGID(GetUserDefaultUILanguage())==LANG_RUSSIAN;
    return isRussian?russian:english;
}

// ---------------------------------------------------------------- logging
std::wstring g_logDirectory;
std::mutex g_logLock;
void Trace(const wchar_t* format,...){
    wchar_t text[2048];va_list args;va_start(args,format);
    int n=_vsnwprintf(text,2047,format,args);va_end(args);
    if(n<0)n=2047;text[n]=0;
    Wh_Log(L"%ls",text);
    if(g_logDirectory.empty()||g_unloading)return;
    std::lock_guard lock(g_logLock);
    std::wstring name=g_logDirectory+L"\\explorer-picker-"+std::to_wstring(GetCurrentProcessId())+L".log";
    FILE* file=_wfopen(name.c_str(),L"a, ccs=UTF-8");if(!file)return;
    SYSTEMTIME t;GetLocalTime(&t);
    fwprintf(file,L"%02u:%02u:%02u.%03u [%lu] %ls\n",t.wHour,t.wMinute,t.wSecond,t.wMilliseconds,GetCurrentThreadId(),text);fclose(file);
}

std::wstring Setting(const wchar_t* name){
    PCWSTR value=Wh_GetStringSetting(name);std::wstring out=value?value:L"";if(value)Wh_FreeStringSetting(value);return out;
}

}  // namespace

// ================================================================ protocol
// Bounded wire format shared by the explorer.exe side (host) and the
// application side (controller) of this one DLL. Both sides always come from
// the same build, but every message is still validated: the host accepts
// requests from any process of the session.
namespace Picker {
constexpr DWORD Magic=0x45585036,MaxBytes=4*1024*1024,MaxFiles=4096,MaxControls=64,MaxItems=256,MaxFilters=128;
constexpr DWORD None=0xFFFFFFFF;
// WM_COPYDATA tags.
enum Tag:ULONG_PTR {
    Open=Magic+1,      // controller -> service: Request
    Ready,             // footer -> controller: frame
    Failed,            // service -> controller
    State,             // footer -> controller: index, folder, selection
    Command,           // footer -> controller: accept
    ControlChange,     // footer -> controller
    Cancel,            // footer -> controller
    Ui,                // controller -> footer: Request (labels/controls)
    Navigate,          // controller -> footer: folder
    Prompt,            // controller -> footer: message box, returns answer
    Close,             // controller -> footer: detach and close the window
    ShowMenu,          // controller -> footer: control id
    Abandon,           // controller -> service: request id no longer wanted
    Name,              // controller -> footer: text for the name box
    Claim,             // explorer process owning a new window -> originating service
    Assign,            // originating service -> service of the window's process
};
enum Mode:DWORD {OpenFile=0,Multi=1,Save=2,Folder=3};
enum Flags:DWORD {SameDirectory=1,StrictTypes=2,AllFlags=3};
enum Kind:DWORD {Check=1,Combo,Radio,Edit,Text,Push,Separator,Group,Menu,OpenDrop,LastKind=OpenDrop};
struct Filter {std::wstring name,pattern;};
struct Item {DWORD id=0,state=3;std::wstring label;};
struct Control {
    DWORD id=0,kind=0,state=3,value=None,parent=0,prominent=0;
    std::wstring label,text;
    std::vector<Item> items;
    const Item* FindItem(DWORD item)const{for(auto& i:items)if(i.id==item)return &i;return nullptr;}
    Item* FindItem(DWORD item){for(auto& i:items)if(i.id==item)return &i;return nullptr;}
};
struct Request {
    DWORD mode=OpenFile,flags=0,index=1;
    std::wstring folder,name,extension,title,okLabel,nameLabel,cancelLabel;
    std::vector<Filter> filters;
    std::vector<Control> controls;
    Control* Find(DWORD id){for(auto& c:controls)if(c.id==id)return &c;return nullptr;}
    const Control* Find(DWORD id)const{for(auto& c:controls)if(c.id==id)return &c;return nullptr;}
};
struct Writer {
    std::vector<BYTE> data;bool good=true;
    void number(DWORD n){if(data.size()+4>MaxBytes){good=false;return;}size_t at=data.size();data.resize(at+4);memcpy(data.data()+at,&n,4);}
    void string(const std::wstring& s){
        if(s.size()>32767||s.find(L'\0')!=std::wstring::npos||data.size()+4+s.size()*2>MaxBytes){good=false;return;}
        number(static_cast<DWORD>(s.size()));size_t at=data.size();data.resize(at+s.size()*2);memcpy(data.data()+at,s.data(),s.size()*2);
    }
    void strings(const std::vector<std::wstring>& list){
        if(list.size()>MaxFiles){good=false;return;}number(static_cast<DWORD>(list.size()));for(auto& s:list)string(s);
    }
};
struct Reader {
    const BYTE* data;size_t size,at=0;bool good=true;
    Reader(const void* p,size_t n):data(static_cast<const BYTE*>(p)),size(n){if(!p||n>MaxBytes)good=false;}
    DWORD number(){DWORD n=0;if(!good||at+4>size){good=false;return 0;}memcpy(&n,data+at,4);at+=4;return n;}
    std::wstring string(){DWORD n=number();if(!good||n>32767||at+size_t(n)*2>size){good=false;return {};}
        std::wstring s(n,L'\0');memcpy(s.data(),data+at,n*2);at+=n*2;if(s.find(L'\0')!=std::wstring::npos)good=false;return s;}
    std::vector<std::wstring> strings(){
        std::vector<std::wstring> list;DWORD n=number();if(n>MaxFiles){good=false;return list;}
        for(DWORD i=0;i<n&&good;i++)list.push_back(string());return list;
    }
    bool end()const{return good&&at==size;}
};
inline void Put(Writer& w,const Request& q){
    w.number(Magic);w.number(q.mode);w.number(q.flags);w.number(q.index);
    for(auto* s:{&q.folder,&q.name,&q.extension,&q.title,&q.okLabel,&q.nameLabel,&q.cancelLabel})w.string(*s);
    if(q.filters.size()>MaxFilters||q.controls.size()>MaxControls){w.good=false;return;}
    w.number(static_cast<DWORD>(q.filters.size()));for(auto& f:q.filters){w.string(f.name);w.string(f.pattern);}
    w.number(static_cast<DWORD>(q.controls.size()));
    for(auto& c:q.controls){
        if(c.items.size()>MaxItems){w.good=false;return;}
        w.number(c.id);w.number(c.kind);w.number(c.state);w.number(c.value);w.number(c.parent);w.number(c.prominent);
        w.string(c.label);w.string(c.text);w.number(static_cast<DWORD>(c.items.size()));
        for(auto& i:c.items){w.number(i.id);w.number(i.state);w.string(i.label);}
    }
}
inline Writer Encode(const Request& q){Writer w;Put(w,q);return w;}
inline bool Get(Reader& r,Request& q){
    if(r.number()!=Magic)return false;
    q.mode=r.number();q.flags=r.number();q.index=r.number();
    for(auto* s:{&q.folder,&q.name,&q.extension,&q.title,&q.okLabel,&q.nameLabel,&q.cancelLabel})*s=r.string();
    DWORD filters=r.number();if(!r.good||filters>MaxFilters||q.mode>Folder||(q.flags&~AllFlags))return false;
    q.filters.clear();for(DWORD i=0;i<filters&&r.good;i++){Filter f;f.name=r.string();f.pattern=r.string();q.filters.push_back(f);}
    DWORD controls=r.number();if(!r.good||controls>MaxControls)return false;
    q.controls.clear();
    for(DWORD i=0;i<controls&&r.good;i++){
        Control c;c.id=r.number();c.kind=r.number();c.state=r.number();c.value=r.number();c.parent=r.number();c.prominent=r.number();
        c.label=r.string();c.text=r.string();DWORD items=r.number();
        if(!r.good||c.kind<Check||c.kind>LastKind||(c.state&~3u)||items>MaxItems)return false;
        for(auto& previous:q.controls)if(previous.id==c.id)return false;
        for(DWORD j=0;j<items&&r.good;j++){Item item;item.id=r.number();item.state=r.number();item.label=r.string();
            if(item.state&~3u)return false;c.items.push_back(item);}
        q.controls.push_back(std::move(c));
    }
    return r.good&&q.index>=1&&q.index<=(filters?filters:1);
}
inline bool Decode(const void* p,size_t n,Request& q){Reader r(p,n);return Get(r,q)&&r.end();}
inline std::wstring Leaf(const std::wstring& p){auto i=p.find_last_of(L"\\/");return i==std::wstring::npos?p:p.substr(i+1);}
inline std::wstring Parent(const std::wstring& p){auto i=p.find_last_of(L"\\/");if(i==std::wstring::npos)return {};return p.substr(0,i==2&&p[1]==L':'?3:i);}
inline bool SamePath(const std::wstring& a,const std::wstring& b){
    auto trim=[](std::wstring s){while(s.size()>3&&(s.back()==L'\\'||s.back()==L'/'))s.pop_back();return s;};
    return !_wcsicmp(trim(a).c_str(),trim(b).c_str());
}
// Send a tagged WM_COPYDATA. Sender HWND travels in wParam and is checked by
// every receiver against the peer it was paired with.
inline bool Post(HWND to,HWND from,ULONG_PTR tag,const Writer& w,UINT timeout=2000,DWORD_PTR* result=nullptr,bool noTimeoutIfAlive=false){
    if(!w.good||!IsWindow(to))return false;
    COPYDATASTRUCT copy={tag,static_cast<DWORD>(w.data.size()),w.data.empty()?nullptr:const_cast<BYTE*>(w.data.data())};
    DWORD_PTR ignored=0;
    return SendMessageTimeoutW(to,WM_COPYDATA,reinterpret_cast<WPARAM>(from),reinterpret_cast<LPARAM>(&copy),
        SMTO_ABORTIFHUNG|(noTimeoutIfAlive?SMTO_NOTIMEOUTIFNOTHUNG:0),timeout,result?result:&ignored)!=0;
}
constexpr wchar_t ServiceClass[]=L"ExplorerPickerServiceV6";
constexpr wchar_t ControllerClass[]=L"ExplorerPickerControllerV6";
constexpr wchar_t FooterClass[]=L"ExplorerPickerFooterV6";
inline HWND ToWindow(DWORD v){return reinterpret_cast<HWND>(static_cast<LONG_PTR>(static_cast<LONG>(v)));}
inline DWORD FromWindow(HWND h){return static_cast<DWORD>(reinterpret_cast<UINT_PTR>(h));}
}

// ================================================================ host
// Runs inside explorer.exe. Opens the picker window itself, recognises it,
// embeds the footer and reads the current folder/selection in-process through
// IShellBrowser. The application side never needs ShellWindows, so elevated
// applications work as well.
namespace Host {
constexpr wchar_t STATE[]=L"ExplorerPicker.State.V6";
constexpr wchar_t BODY[]=L"ExplorerPicker.Body.V6";
constexpr wchar_t STATUSBAR[]=L"ExplorerPicker.StatusBar.V6";
constexpr UINT_PTR PollTimer=2,LiveTimer=1;
constexpr int OpenId=IDOK,CancelId=IDCANCEL,NameId=101,TypesId=102,DropId=103,FirstWidgetId=4000;

UINT probeMessage,layoutMessage,detachMessage,menuMessage;
HWND service=nullptr;HANDLE serviceThread=nullptr;
std::mutex lock;
struct Pending {DWORD id=0;HWND controller=nullptr;Picker::Request request;std::set<HWND> baseline;ULONGLONG since=0;bool launched=false,taken=false,assigned=false;};
std::vector<Pending> pending;
std::vector<HWND> frames;
using DispatchFn=LRESULT(WINAPI*)(const MSG*);
DispatchFn originalDispatch=nullptr;
thread_local bool routingFooter=false;

struct Widget {HWND hwnd=nullptr;size_t control=0;DWORD item=Picker::None;bool caption=false;};
struct State {
    HWND frame=nullptr,tab=nullptr,controller=nullptr,footer=nullptr,name=nullptr,types=nullptr,open=nullptr,cancel=nullptr,label=nullptr,typeLabel=nullptr,drop=nullptr;
    HWND body=nullptr,status=nullptr;
    RECT nativeBody={};
    bool nativeBodyValid=false,layoutQueued=false,restoringBody=false;
    HANDLE process=nullptr;HFONT font=nullptr;int height=0;DWORD requestId=0;
    Picker::Request request;
    std::vector<Widget> widgets;
    std::wstring lastFolder,lastDisplay;std::vector<std::wstring> lastPaths;DWORD lastIndex=0;bool stateSent=false;
    bool layingOut=false,detaching=false,updating=false,edited=false,closing=false;
};

int Scale(State* s,int n){
    using DpiFn=UINT(WINAPI*)(HWND);static auto dpiFn=reinterpret_cast<DpiFn>(GetProcAddress(GetModuleHandleW(L"user32.dll"),"GetDpiForWindow"));
    UINT dpi=dpiFn?dpiFn(s->frame):96;return MulDiv(n,dpi?dpi:96,96);
}
bool ClassIs(HWND h,const wchar_t* wanted){wchar_t c[128]={};GetClassNameW(h,c,128);return !wcscmp(c,wanted);}
State* GetState(HWND frame){return frame?reinterpret_cast<State*>(GetPropW(frame,STATE)):nullptr;}
std::wstring WindowText(HWND h){
    int n=GetWindowTextLengthW(h);if(n<=0||n>32767)return {};
    std::wstring value(n+1,L'\0');value.resize(GetWindowTextW(h,value.data(),n+1));return value;
}

// ---------------------------------------------------------------- shell access
// All of these run on the frame's own thread (window procedures, dispatch hook).
IShellBrowser* Browser(HWND frame){
    constexpr UINT CWM_GETISHELLBROWSER=WM_USER+7;
    HWND tab=FindWindowExW(frame,nullptr,L"ShellTabWindowClass",nullptr);
    for(HWND h:{tab,frame}){
        if(!h)continue;DWORD_PTR r=0;
        if(SendMessageTimeoutW(h,CWM_GETISHELLBROWSER,0,0,SMTO_ABORTIFHUNG,500,&r)&&r)return reinterpret_cast<IShellBrowser*>(r);
    }return nullptr;
}
IFolderView2* FolderView(HWND frame){
    IShellBrowser* browser=Browser(frame);if(!browser)return nullptr;
    browser->AddRef();IShellView* view=nullptr;IFolderView2* folder=nullptr;
    if(SUCCEEDED(browser->QueryActiveShellView(&view))&&view){view->QueryInterface(IID_PPV_ARGS(&folder));view->Release();}
    browser->Release();return folder;
}
std::wstring PidlPath(PCIDLIST_ABSOLUTE pidl){
    PWSTR name=nullptr;std::wstring out;
    if(pidl&&SUCCEEDED(SHGetNameFromIDList(pidl,SIGDN_FILESYSPATH,&name))&&name){out=name;CoTaskMemFree(name);}
    return out;
}
bool ReadShell(HWND frame,std::wstring& folder,std::vector<std::wstring>* paths){
    IFolderView2* view=FolderView(frame);if(!view)return false;
    folder.clear();bool ok=false;
    IPersistFolder2* persist=nullptr;
    if(SUCCEEDED(view->GetFolder(IID_PPV_ARGS(&persist)))&&persist){
        PIDLIST_ABSOLUTE pidl=nullptr;if(SUCCEEDED(persist->GetCurFolder(&pidl))&&pidl){folder=PidlPath(pidl);ok=true;CoTaskMemFree(pidl);}
        persist->Release();
    }
    if(ok&&paths){
        paths->clear();IShellItemArray* items=nullptr;DWORD count=0;
        if(SUCCEEDED(view->GetSelection(FALSE,&items))&&items){
            if(SUCCEEDED(items->GetCount(&count))){
                for(DWORD i=0;i<count&&i<Picker::MaxFiles;i++){
                    IShellItem* item=nullptr;PWSTR path=nullptr;std::wstring value;
                    if(SUCCEEDED(items->GetItemAt(i,&item))&&item){
                        if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH,&path))&&path){value=path;CoTaskMemFree(path);}
                        item->Release();
                    }
                    paths->push_back(value); // empty: not a file system item
                }
            }
            items->Release();
        }
    }
    view->Release();return ok;
}
void NavigateTo(HWND frame,const std::wstring& path,UINT extra=0){
    IShellBrowser* browser=Browser(frame);if(!browser)return;
    PIDLIST_ABSOLUTE pidl=nullptr;
    if(SUCCEEDED(SHParseDisplayName(path.c_str(),nullptr,&pidl,0,nullptr))&&pidl){
        browser->AddRef();browser->BrowseObject(pidl,SBSP_SAMEBROWSER|SBSP_ABSOLUTE|extra);browser->Release();CoTaskMemFree(pidl);
    }
}

// ---------------------------------------------------------------- footer model
std::wstring FilterLabel(const Picker::Filter& filter){
    if(filter.name.empty())return filter.pattern;
    if(filter.name.find(filter.pattern)!=std::wstring::npos)return filter.name;
    return filter.name+L" ("+filter.pattern+L")";
}
std::wstring PreferredExtension(const Picker::Filter& filter){
    auto pattern=filter.pattern.substr(0,filter.pattern.find(L';'));
    if(pattern.rfind(L"*.",0)!=0)return {};
    auto ext=pattern.substr(2);if(ext.empty()||ext.find_first_of(L"*?\\/:; \t")!=std::wstring::npos)return {};
    return ext;
}
void SendState(State* s,bool force);
void TypeChanged(State* s){
    LRESULT selected=SendMessageW(s->types,CB_GETCURSEL,0,0);
    if(selected<0||static_cast<size_t>(selected)>=s->request.filters.size())return;
    DWORD previous=s->request.index;s->request.index=static_cast<DWORD>(selected+1);
    if(s->request.mode==Picker::Save&&previous>=1&&previous<=s->request.filters.size()&&previous!=s->request.index){
        auto before=PreferredExtension(s->request.filters[previous-1]);
        auto after=PreferredExtension(s->request.filters[s->request.index-1]);
        std::wstring text=WindowText(s->name);
        // Only update an unquoted conventional filename whose suffix is the
        // previous format's default extension (or that has none).
        if(!after.empty()&&!text.empty()&&text.find(L'"')==std::wstring::npos){
            auto slash=text.find_last_of(L"\\/"),dot=text.find_last_of(L'.');
            bool extension=dot!=std::wstring::npos&&(slash==std::wstring::npos||dot>slash);
            if(!extension||(!before.empty()&&!_wcsicmp(text.substr(dot+1).c_str(),before.c_str()))){
                if(extension)text.resize(dot);text+=L"."+after;
                s->updating=true;SetWindowTextW(s->name,text.c_str());s->updating=false;s->edited=true;EnableWindow(s->open,TRUE);
            }
        }
    }
    SendState(s,true);
}
HFONT Font(State* s){return s->font?s->font:static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));}
int TextWidth(State* s,const std::wstring& text){
    HDC dc=GetDC(s->footer);HGDIOBJ old=SelectObject(dc,Font(s));SIZE size={};
    std::wstring shown;for(size_t i=0;i<text.size();i++){if(text[i]==L'&'&&i+1<text.size())++i;shown.push_back(text[i]);}
    GetTextExtentPoint32W(dc,shown.c_str(),static_cast<int>(shown.size()),&size);SelectObject(dc,old);ReleaseDC(s->footer,dc);return size.cx;
}
void DestroyWidgets(State* s){for(auto& w:s->widgets)if(IsWindow(w.hwnd))DestroyWindow(w.hwnd);s->widgets.clear();}
bool SameStructure(const Picker::Request& a,const Picker::Request& b){
    if(a.controls.size()!=b.controls.size())return false;
    for(size_t i=0;i<a.controls.size();i++){
        auto& x=a.controls[i];auto& y=b.controls[i];
        if(x.id!=y.id||x.kind!=y.kind||x.parent!=y.parent||x.items.size()!=y.items.size())return false;
        for(size_t j=0;j<x.items.size();j++)if(x.items[j].id!=y.items[j].id)return false;
    }return true;
}
HWND MakeWidget(State* s,const wchar_t* cls,const wchar_t* text,DWORD style,DWORD exStyle,size_t control,DWORD item,bool caption){
    int id=FirstWidgetId+static_cast<int>(s->widgets.size());
    HWND h=CreateWindowExW(exStyle,cls,text,WS_CHILD|style,0,0,0,0,s->footer,reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),g_module,nullptr);
    if(h){SendMessageW(h,WM_SETFONT,reinterpret_cast<WPARAM>(Font(s)),FALSE);s->widgets.push_back({h,control,item,caption});}
    return h;
}
void BuildWidgets(State* s){
    DestroyWidgets(s);
    for(size_t i=0;i<s->request.controls.size();i++){
        auto& c=s->request.controls[i];
        switch(c.kind){
        case Picker::Check:MakeWidget(s,L"BUTTON",c.label.c_str(),WS_TABSTOP|WS_GROUP|BS_AUTOCHECKBOX,0,i,Picker::None,false);break;
        case Picker::Combo:MakeWidget(s,L"COMBOBOX",L"",WS_TABSTOP|WS_GROUP|WS_VSCROLL|CBS_DROPDOWNLIST,0,i,Picker::None,false);break;
        case Picker::Radio:{bool first=true;
            for(auto& item:c.items){MakeWidget(s,L"BUTTON",item.label.c_str(),WS_TABSTOP|(first?WS_GROUP:0)|BS_AUTORADIOBUTTON,0,i,item.id,false);first=false;}
            break;}
        case Picker::Edit:{HWND h=MakeWidget(s,L"EDIT",c.text.c_str(),WS_TABSTOP|WS_GROUP|ES_AUTOHSCROLL,WS_EX_CLIENTEDGE,i,Picker::None,false);
            if(h)SendMessageW(h,EM_SETLIMITTEXT,32767,0);break;}
        case Picker::Text:MakeWidget(s,L"STATIC",c.label.c_str(),SS_LEFTNOWORDWRAP|WS_GROUP,0,i,Picker::None,false);break;
        case Picker::Push:MakeWidget(s,L"BUTTON",c.label.c_str(),WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON,0,i,Picker::None,false);break;
        case Picker::Menu:MakeWidget(s,L"BUTTON",(c.label+L" ▼").c_str(),WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON,0,i,Picker::None,false);break;
        case Picker::Separator:MakeWidget(s,L"STATIC",L"",SS_ETCHEDHORZ,0,i,Picker::None,false);break;
        case Picker::Group:MakeWidget(s,L"STATIC",c.label.c_str(),SS_LEFTNOWORDWRAP|WS_GROUP,0,i,Picker::None,true);break;
        default:break; // OpenDrop lives on the Open button
        }
    }
}
const Picker::Control* OpenDropControl(State* s){for(auto& c:s->request.controls)if(c.kind==Picker::OpenDrop)return &c;return nullptr;}
bool Visible(State* s,const Picker::Control& c){
    if(!(c.state&2))return false;
    if(c.parent){auto group=s->request.Find(c.parent);if(group&&!(group->state&2))return false;}
    return true;
}
void SyncWidgets(State* s){
    s->updating=true;
    for(auto& w:s->widgets){
        auto& c=s->request.controls[w.control];bool visible=Visible(s,c),enabled=(c.state&1)!=0;
        switch(c.kind){
        case Picker::Check:SetWindowTextW(w.hwnd,c.label.c_str());SendMessageW(w.hwnd,BM_SETCHECK,c.value==1?BST_CHECKED:BST_UNCHECKED,0);break;
        case Picker::Combo:{
            SendMessageW(w.hwnd,CB_RESETCONTENT,0,0);int selected=-1;
            for(auto& item:c.items){if(!(item.state&2))continue;
                LRESULT at=SendMessageW(w.hwnd,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(item.label.c_str()));
                if(at>=0){SendMessageW(w.hwnd,CB_SETITEMDATA,at,item.id);if(item.id==c.value)selected=static_cast<int>(at);}
            }
            SendMessageW(w.hwnd,CB_SETCURSEL,selected,0);break;}
        case Picker::Radio:{auto item=c.FindItem(w.item);
            if(item){SetWindowTextW(w.hwnd,item->label.c_str());visible=visible&&(item->state&2);enabled=enabled&&(item->state&1);}
            SendMessageW(w.hwnd,BM_SETCHECK,c.value==w.item?BST_CHECKED:BST_UNCHECKED,0);break;}
        case Picker::Edit:if(WindowText(w.hwnd)!=c.text)SetWindowTextW(w.hwnd,c.text.c_str());break;
        case Picker::Menu:SetWindowTextW(w.hwnd,(c.label+L" ▼").c_str());break;
        case Picker::Separator:break;
        default:SetWindowTextW(w.hwnd,c.label.c_str());break;
        }
        EnableWindow(w.hwnd,enabled);ShowWindow(w.hwnd,visible?SW_SHOWNA:SW_HIDE);
    }
    auto drop=OpenDropControl(s);
    if(drop&&!s->drop){
        s->drop=CreateWindowW(L"BUTTON",L"▼",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,0,0,0,0,s->footer,reinterpret_cast<HMENU>(DropId),g_module,nullptr);
        if(s->drop)SendMessageW(s->drop,WM_SETFONT,reinterpret_cast<WPARAM>(Font(s)),FALSE);
    }else if(!drop&&s->drop){DestroyWindow(s->drop);s->drop=nullptr;}
    if(s->drop){EnableWindow(s->drop,(drop->state&1)!=0);ShowWindow(s->drop,(drop->state&2)?SW_SHOWNA:SW_HIDE);}
    const bool folder=s->request.mode==Picker::Folder;
    SetWindowTextW(s->open,!s->request.okLabel.empty()?s->request.okLabel.c_str():folder?UiText(L"Select Folder",L"Выбор папки"):s->request.mode==Picker::Save?UiText(L"Save",L"Сохранить"):UiText(L"Open",L"Открыть"));
    SetWindowTextW(s->cancel,!s->request.cancelLabel.empty()?s->request.cancelLabel.c_str():UiText(L"Cancel",L"Отмена"));
    SetWindowTextW(s->label,!s->request.nameLabel.empty()?s->request.nameLabel.c_str():folder?UiText(L"Folder:",L"Папка:"):s->request.mode==Picker::Multi?UiText(L"File name:",L"Имена файлов:"):UiText(L"File name:",L"Имя файла:"));
    ShowWindow(s->types,folder?SW_HIDE:SW_SHOWNA);ShowWindow(s->typeLabel,folder?SW_HIDE:SW_SHOWNA);
    s->updating=false;
}
// Lays out the fixed rows and the application's controls. With apply=false it
// only measures, so the frame can size the footer before moving anything.
int Arrange(State* s,int width,bool apply){
    // One grid for every row: the pitch and margins of "File name"/"Files of type",
    // so extra rows look like part of the same built-in dialog panel.
    int p=Scale(s,12),gap=Scale(s,10),h=Scale(s,25),button=Scale(s,100),label=Scale(s,110),line=h+gap,spacing=Scale(s,8);
    // The caption column fits the longest caption, within a third of the width.
    // The caption column fits the longest caption. The confirmed 110 px minimum
    // applies with the file type row; a folder picker has only its short label.
    {const bool folder=s->request.mode==Picker::Folder;
        int widest=TextWidth(s,WindowText(s->label));if(!folder)widest=std::max(widest,TextWidth(s,WindowText(s->typeLabel)));
        for(auto& w:s->widgets)if(w.caption)widest=std::max(widest,TextWidth(s,s->request.controls[w.control].label));
        int minimum=folder?Scale(s,50):label;
        label=std::min(std::max(minimum,widest+Scale(s,16)),std::max(minimum,width/3));}
    // Buttons fit their captions with native-like side padding.
    button=std::max(button,std::max(TextWidth(s,WindowText(s->open)),TextWidth(s,WindowText(s->cancel)))+Scale(s,32));
    int right=std::max<int>(p,width-p-button),input=p+label,inputWidth=std::max(Scale(s,35),right-gap-input);
    if(apply){
        MoveWindow(s->label,p,p+Scale(s,4),label-Scale(s,6),h,TRUE);
        MoveWindow(s->name,input,p,inputWidth,h,TRUE);
        int openWidth=s->drop&&IsWindowVisible(s->drop)?button-Scale(s,18):button;
        MoveWindow(s->open,right,p,openWidth,h,TRUE);
        if(s->drop)MoveWindow(s->drop,right+openWidth,p,button-openWidth,h,TRUE);
        MoveWindow(s->typeLabel,p,p+h+gap+Scale(s,4),label-Scale(s,6),h,TRUE);
        MoveWindow(s->types,input,p+h+gap,inputWidth,Scale(s,250),TRUE);
        MoveWindow(s->cancel,right,p+h+gap,button,h,TRUE);
    }
    int y=Scale(s,82);bool any=false;
    auto natural=[&](const Widget& w)->int{
        auto& c=s->request.controls[w.control];
        switch(c.kind){
        case Picker::Check:return TextWidth(s,c.label)+Scale(s,24);
        case Picker::Radio:{auto item=c.FindItem(w.item);return (item?TextWidth(s,item->label):0)+Scale(s,24);}
        case Picker::Push:case Picker::Menu:return std::max(TextWidth(s,c.label)+Scale(s,c.kind==Picker::Menu?40:24),Scale(s,75));
        case Picker::Combo:{int widest=Scale(s,80);for(auto& i:c.items)widest=std::max(widest,TextWidth(s,i.label));return widest+Scale(s,40);}
        case Picker::Edit:return Scale(s,160);
        default:return TextWidth(s,c.label)+Scale(s,4);
        }
    };
    auto shown=[&](const Widget& w){
        auto& c=s->request.controls[w.control];if(!Visible(s,c))return false;
        if(c.kind==Picker::Radio){auto item=c.FindItem(w.item);return item&&(item->state&2);}return true;
    };
    auto emit=[&](const Widget* caption,const std::vector<const Widget*>& members){
        // Captions and texts stay inside their line: taller statics covered the footer's bottom edge.
        if(caption&&apply)MoveWindow(caption->hwnd,p,y+Scale(s,4),label-Scale(s,6),Scale(s,20),TRUE);
        // Flow the members into lines; the last edit or list of a line takes the rest of it.
        std::vector<std::vector<std::pair<const Widget*,int>>> lines(1);int x=input;
        for(auto w:members){
            auto& c=s->request.controls[w->control];
            bool fill=members.size()==1&&(c.kind==Picker::Combo||c.kind==Picker::Edit||c.kind==Picker::Text||c.kind==Picker::Separator);
            int width=fill?inputWidth:std::min(natural(*w),inputWidth);
            if(!lines.back().empty()&&x+width>input+inputWidth){lines.emplace_back();x=input;}
            lines.back().push_back({w,width});x+=width+spacing;
        }
        for(auto& row:lines){
            if(row.empty())continue;
            auto& last=row.back();auto kind=s->request.controls[last.first->control].kind;
            if(kind==Picker::Edit||kind==Picker::Combo){int used=0;for(auto& m:row)used+=m.second+spacing;last.second+=std::max(0,inputWidth-(used-spacing));}
            x=input;
            for(auto& m:row){
                auto& c=s->request.controls[m.first->control];int width=m.second;
                if(apply){
                    int top=c.kind==Picker::Text?y+Scale(s,4):y,height=c.kind==Picker::Combo?Scale(s,250):c.kind==Picker::Check||c.kind==Picker::Radio?Scale(s,24):c.kind==Picker::Text?Scale(s,20):h;
                    if(c.kind==Picker::Separator){top=y+h/2-1;height=2;}
                    MoveWindow(m.first->hwnd,x,top,width,height,TRUE);
                }
                x+=width+spacing;
            }
            y+=line;
        }
        if(members.empty())y+=line; // a caption without members still takes its line
        any=true;
    };
    for(size_t i=0;i<s->request.controls.size();i++){
        auto& c=s->request.controls[i];
        if(c.parent&&s->request.Find(c.parent)&&s->request.Find(c.parent)->kind==Picker::Group)continue;
        if(!Visible(s,c)||c.kind==Picker::OpenDrop)continue;
        const Widget* caption=nullptr;std::vector<const Widget*> members;
        if(c.kind==Picker::Group){
            for(auto& w:s->widgets)if(w.control==i&&w.caption)caption=&w;
            for(size_t j=0;j<s->request.controls.size();j++)if(s->request.controls[j].parent==c.id)
                for(auto& w:s->widgets)if(w.control==j&&!w.caption&&shown(w))members.push_back(&w);
            if(!members.empty()||(caption&&!c.label.empty()))emit(caption,members);
        }else{
            for(auto& w:s->widgets)if(w.control==i&&shown(w))members.push_back(&w);
            if(!members.empty())emit(nullptr,members);
        }
    }
    return any?y-gap+p:p+h+gap+h+p;
}
void QueueLayout(State* s);
void ApplyUi(State* s,const Picker::Request& q){
    bool rebuild=!SameStructure(s->request,q)||s->widgets.empty()!=q.controls.empty();
    s->request.controls=q.controls;s->request.okLabel=q.okLabel;s->request.nameLabel=q.nameLabel;s->request.cancelLabel=q.cancelLabel;
    if(!q.extension.empty())s->request.extension=q.extension;
    if(rebuild)BuildWidgets(s);
    SyncWidgets(s);
    RECT r={};GetClientRect(s->footer,&r);
    RECT tab={};GetClientRect(s->tab,&tab);
    int height=Arrange(s,tab.right-tab.left,false);
    if(height!=s->height){s->height=height;QueueLayout(s);}
    Arrange(s,r.right-r.left,true);InvalidateRect(s->footer,nullptr,TRUE);
}

// ---------------------------------------------------------------- messages to the controller
bool ControllerAlive(State* s){
    return IsWindow(s->controller)&&(!s->process||WaitForSingleObject(s->process,0)==WAIT_TIMEOUT);
}
void SendCancel(State* s){if(!s->closing){s->closing=true;Picker::Writer w;Picker::Post(s->controller,s->footer,Picker::Cancel,w,1000);}}
void SendCommand(State* s,DWORD kind,DWORD dropItem=Picker::None){
    if(s->detaching)return;
    std::wstring folder,typed=WindowText(s->name);std::vector<std::wstring> paths;
    if(!ReadShell(s->frame,folder,&paths)){folder=s->lastFolder;paths=s->lastPaths;}
    LRESULT index=SendMessageW(s->types,CB_GETCURSEL,0,0);
    Picker::Writer w;w.number(kind);w.number(kind==3?0:(s->edited||s->request.mode==Picker::Save)?1:0);
    w.number(index<0?1:static_cast<DWORD>(index+1));w.string(typed);w.string(folder);w.strings(paths);w.number(dropItem);
    Picker::Post(s->controller,s->footer,Picker::Command,w,2000);
}
void SendControl(State* s,DWORD id,DWORD action,DWORD value,const std::wstring& text){
    Picker::Writer w;w.number(id);w.number(action);w.number(value);w.string(text);
    Picker::Post(s->controller,s->footer,Picker::ControlChange,w,2000);
}
std::wstring Display(State* s,const std::wstring& folder,const std::vector<std::wstring>& paths){
    const bool folderMode=s->request.mode==Picker::Folder;
    if(paths.size()>512)return {};
    std::vector<std::wstring> valid;
    for(auto& p:paths){
        if(p.empty())continue;DWORD attr=GetFileAttributesW(p.c_str());
        if(attr!=INVALID_FILE_ATTRIBUTES&&bool(attr&FILE_ATTRIBUTE_DIRECTORY)==folderMode)valid.push_back(p);
    }
    if(folderMode&&paths.empty())return folder;
    if(valid.empty()||(s->request.mode!=Picker::Multi&&valid.size()!=1))return {};
    if(folderMode)return valid[0];
    if(valid.size()==1)return Picker::Leaf(valid[0]);
    std::wstring text;for(auto& p:valid){if(!text.empty())text+=L" ";text+=L"\""+Picker::Leaf(p)+L"\"";}return text;
}
void SendState(State* s,bool force){
    if(s->detaching||s->closing)return;
    std::wstring folder;std::vector<std::wstring> paths;
    if(!ReadShell(s->frame,folder,&paths))return;
    LRESULT selected=SendMessageW(s->types,CB_GETCURSEL,0,0);
    DWORD index=selected<0?1:static_cast<DWORD>(selected+1);
    bool changed=force||!s->stateSent||folder!=s->lastFolder||paths!=s->lastPaths||index!=s->lastIndex;
    if(!changed)return;
    if(folder!=s->lastFolder||paths!=s->lastPaths||!s->stateSent){
        auto display=Display(s,folder,paths);
        if(display!=s->lastDisplay){
            s->lastDisplay=display;
            // Keep typed/prefilled names when changing folders or clearing the selection.
            if(!(display.empty()&&(s->request.mode==Picker::Save||s->edited))){
                s->updating=true;SetWindowTextW(s->name,display.c_str());s->updating=false;s->edited=false;
                EnableWindow(s->open,!display.empty()||s->request.mode==Picker::Folder);
            }
        }
    }
    s->lastFolder=folder;s->lastPaths=paths;s->lastIndex=index;s->stateSent=true;
    Picker::Writer w;w.number(index);w.string(folder);w.strings(paths);
    Picker::Post(s->controller,s->footer,Picker::State,w,2000);
}
void ShowControlMenu(State* s,DWORD id){
    auto c=s->request.Find(id);if(!c||(c->kind!=Picker::Menu&&c->kind!=Picker::OpenDrop))return;
    HWND anchor=c->kind==Picker::OpenDrop?s->drop:nullptr;
    for(auto& w:s->widgets)if(s->request.controls[w.control].id==id&&!w.caption)anchor=w.hwnd;
    if(!anchor)return;
    HMENU menu=CreatePopupMenu();if(!menu)return;
    for(auto& item:c->items)if(item.state&2)
        AppendMenuW(menu,MF_STRING|((item.state&1)?0:MF_GRAYED),item.id+1,item.label.c_str());
    RECT r;GetWindowRect(anchor,&r);
    UINT chosen=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY,r.left,r.bottom,0,s->footer,nullptr);
    DestroyMenu(menu);
    if(!chosen||!GetState(s->frame))return;
    if(c->kind==Picker::OpenDrop)SendCommand(s,1,chosen-1);
    else SendControl(s,id,0,chosen-1,{});
}

// ---------------------------------------------------------------- layout (host 0.5.5)
// Keep the real status bar, its parent, styles and nonclient edge intact.
// Classic Explorer can move its replacement status bar while resizing DUIView.
// Anchor that bar to its parent's bottom and clamp OUR body resize after the
// native subclasses, so their status-height deduction isn't applied twice.
LRESULT CALLBACK BodyProc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
LRESULT CALLBACK StatusProc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
void QueueLayout(State* s){
    if(s->detaching||s->layingOut||s->layoutQueued)return;
    s->layoutQueued=true;
    if(!PostMessageW(s->frame,layoutMessage,reinterpret_cast<WPARAM>(s->footer),0))s->layoutQueued=false;
}
RECT InParent(HWND window,HWND parent){
    RECT rect={};GetWindowRect(window,&rect);
    MapWindowPoints(nullptr,parent,reinterpret_cast<POINT*>(&rect),2);return rect;
}
struct Parts {
    HWND frame=nullptr,tab=nullptr,body=nullptr,status=nullptr;
    bool ambiguousBody=false,ambiguousStatus=false,nestedStatus=false;
};
BOOL CALLBACK FindPartsProc(HWND h,LPARAM param){
    auto parts=reinterpret_cast<Parts*>(param);
    // The file area is the tab's DirectUI container (tree + items). It lives as
    // long as the tab, unlike SHELLDLL_DefView, which is replaced on every
    // navigation and is hidden when the window is too narrow for it.
    if(GetParent(h)==parts->tab&&ClassIs(h,L"DUIViewWndClassName")&&IsWindowVisible(h)){
        if(parts->body&&parts->body!=h)parts->ambiguousBody=true;
        else parts->body=h;
    }
    if(ClassIs(h,L"msctls_statusbar32")&&IsWindowVisible(h)){
        HWND parent=GetParent(h);
        if(parent==parts->tab||parent==parts->frame){
            if(parts->status&&parts->status!=h)parts->ambiguousStatus=true;
            else parts->status=h;
        }else parts->nestedStatus=true;
    }
    return TRUE;
}
bool FindParts(State* s,HWND& body,HWND& status){
    Parts parts;parts.frame=s->frame;parts.tab=s->tab;
    EnumChildWindows(s->frame,FindPartsProc,reinterpret_cast<LPARAM>(&parts));
    if(parts.ambiguousBody||parts.ambiguousStatus||!parts.body)return false;
    // A hidden original control can precede Classic Explorer's visible bar.
    if(!parts.status&&parts.nestedStatus)return false;
    body=parts.body;status=parts.status;
    // Unknown hierarchies must not shrink a branch which contains the status bar.
    if(body==status||(status&&IsChild(body,status)))return false;
    if(GetWindowThreadProcessId(body,nullptr)!=GetCurrentThreadId()||
       (status&&GetWindowThreadProcessId(status,nullptr)!=GetCurrentThreadId()))return false;
    return true;
}
void ReleaseBody(State* s,bool restore){
    HWND body=s->body;RECT original=s->nativeBody;bool valid=s->nativeBodyValid;
    s->body=nullptr;s->nativeBodyValid=false;RemovePropW(s->frame,BODY);
    if(IsWindow(body)){
        if(restore&&valid&&GetParent(body)==s->tab){
            s->restoringBody=true;
            SetWindowPos(body,nullptr,original.left,original.top,original.right-original.left,
                original.bottom-original.top,SWP_NOZORDER|SWP_NOACTIVATE);
            s->restoringBody=false;
        }
        RemoveWindowSubclass(body,BodyProc,1);
    }
}
bool BindParts(State* s){
    HWND body=nullptr,status=nullptr;if(!FindParts(s,body,status))return false;
    if(body!=s->body){
        ReleaseBody(s,true);
        RECT original=InParent(body,s->tab);
        if(!SetWindowSubclass(body,BodyProc,1,reinterpret_cast<DWORD_PTR>(s)))return false;
        s->body=body;s->nativeBody=original;s->nativeBodyValid=true;SetPropW(s->frame,BODY,body);
    }
    if(status!=s->status){
        if(IsWindow(s->status))RemoveWindowSubclass(s->status,StatusProc,1);
        s->status=nullptr;RemovePropW(s->frame,STATUSBAR);
        if(status){
            if(!SetWindowSubclass(status,StatusProc,1,reinterpret_cast<DWORD_PTR>(s)))return false;
            s->status=status;SetPropW(s->frame,STATUSBAR,status);
        }
    }
    return true;
}
RECT StatusBounds(State* s){
    HWND parent=GetParent(s->status);
    RECT bar=InParent(s->status,parent),client={};GetClientRect(parent,&client);
    LONG height=bar.bottom-bar.top;bar.bottom=client.bottom;bar.top=bar.bottom-height;
    MapWindowPoints(parent,s->frame,reinterpret_cast<POINT*>(&bar),2);return bar;
}
void DockStatus(State* s){
    if(!IsWindow(s->status)||!IsWindowVisible(s->status))return;
    RECT dock=StatusBounds(s),current=InParent(s->status,s->frame);
    if(EqualRect(&dock,&current))return;
    MapWindowPoints(s->frame,GetParent(s->status),reinterpret_cast<POINT*>(&dock),2);
    SetWindowPos(s->status,nullptr,dock.left,dock.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}
RECT FooterBounds(State* s){
    RECT rect={};GetClientRect(s->tab,&rect);
    MapWindowPoints(s->tab,s->frame,reinterpret_cast<POINT*>(&rect),2);
    if(IsWindow(s->status)&&IsWindowVisible(s->status)){
        RECT status=StatusBounds(s);
        if(status.top>=rect.top&&status.top<=rect.bottom&&status.bottom>status.top)rect.bottom=status.top;
    }
    rect.top=std::max(rect.top,rect.bottom-s->height);return rect;
}
int BodyHeight(State* s,const RECT& native){
    RECT dock=FooterBounds(s);POINT top={dock.left,dock.top};
    MapWindowPoints(s->frame,s->tab,&top,1);
    return std::max(0L,std::min(native.bottom,top.y)-native.top);
}
void QueueLayoutLater(State* s){if(!s->detaching)SetTimer(s->footer,3,100,nullptr);}
void Layout(State* s){
    if(s->layingOut||s->detaching||!IsWindow(s->tab)||IsIconic(s->frame))return;
    s->layingOut=true;
    RECT tab={};GetClientRect(s->tab,&tab);s->height=Arrange(s,tab.right-tab.left,false);
    // The footer stays where it is while the parts are being rebuilt; hiding it
    // made it (and the tree beside it) flicker on every navigation.
    if(BindParts(s)){
        RECT body=s->nativeBody;body.bottom=body.top+BodyHeight(s,body);
        RECT current=InParent(s->body,s->tab);
        if(!EqualRect(&body,&current))
            SetWindowPos(s->body,nullptr,body.left,body.top,body.right-body.left,body.bottom-body.top,SWP_NOZORDER|SWP_NOACTIVATE);
        DockStatus(s);
    }else QueueLayoutLater(s);
    RECT dock=FooterBounds(s),current=InParent(s->footer,s->frame);
    if(!EqualRect(&dock,&current)||!IsWindowVisible(s->footer))
        SetWindowPos(s->footer,HWND_TOP,dock.left,dock.top,dock.right-dock.left,dock.bottom-dock.top,SWP_NOACTIVATE|SWP_SHOWWINDOW);
    s->layingOut=false;
}
LRESULT CALLBACK BodyProc(HWND h,UINT m,WPARAM w,LPARAM l,UINT_PTR,DWORD_PTR data){
    auto s=reinterpret_cast<State*>(data);HWND frame=s->frame;
    bool ownPosition=m==WM_WINDOWPOSCHANGING&&(s->layingOut||s->restoringBody);
    WINDOWPOS requested={};if(ownPosition)requested=*reinterpret_cast<WINDOWPOS*>(l);
    if(m==WM_NCDESTROY){
        RemoveWindowSubclass(h,BodyProc,1);
        if(s->body==h){s->body=nullptr;s->nativeBodyValid=false;RemovePropW(frame,BODY);QueueLayout(s);}
        return DefSubclassProc(h,m,w,l);
    }
    LRESULT result=DefSubclassProc(h,m,w,l);
    if(ownPosition){
        // Older subclasses may have subtracted the status bar again. Keep their
        // other processing, but restore our exact dimensions afterwards.
        auto pos=reinterpret_cast<WINDOWPOS*>(l);
        if(!(requested.flags&SWP_NOSIZE)){pos->cx=requested.cx;pos->cy=requested.cy;pos->flags&=~SWP_NOSIZE;}
        if(!(requested.flags&SWP_NOMOVE)){pos->x=requested.x;pos->y=requested.y;pos->flags&=~SWP_NOMOVE;}
        return result;
    }
    if(GetState(frame)!=s||s->detaching)return result;
    if(m==WM_WINDOWPOSCHANGING&&!s->layingOut&&s->nativeBodyValid){
        auto pos=reinterpret_cast<WINDOWPOS*>(l);RECT native=s->nativeBody;
        int x=(pos->flags&SWP_NOMOVE)?native.left:pos->x;
        int y=(pos->flags&SWP_NOMOVE)?native.top:pos->y;
        int width=(pos->flags&SWP_NOSIZE)?native.right-native.left:pos->cx;
        int height=(pos->flags&SWP_NOSIZE)?native.bottom-native.top:pos->cy;
        s->nativeBody={x,y,x+width,y+height};
        int limited=BodyHeight(s,s->nativeBody);
        RECT current=InParent(h,s->tab);
        if(!(pos->flags&SWP_NOSIZE)||limited!=current.bottom-current.top){
            pos->cx=width;pos->cy=limited;pos->flags&=~SWP_NOSIZE;
        }
    }
    if(m==WM_WINDOWPOSCHANGED||m==WM_SHOWWINDOW)QueueLayout(s);
    return result;
}
LRESULT CALLBACK StatusProc(HWND h,UINT m,WPARAM w,LPARAM l,UINT_PTR,DWORD_PTR data){
    auto s=reinterpret_cast<State*>(data);HWND frame=s->frame;
    if(m==WM_NCDESTROY){
        RemoveWindowSubclass(h,StatusProc,1);
        if(s->status==h){s->status=nullptr;RemovePropW(frame,STATUSBAR);QueueLayout(s);}
        return DefSubclassProc(h,m,w,l);
    }
    LRESULT result=DefSubclassProc(h,m,w,l);
    if(GetState(frame)!=s)return result;
    if(m==WM_WINDOWPOSCHANGING&&IsWindowVisible(h)){
        auto pos=reinterpret_cast<WINDOWPOS*>(l);HWND parent=GetParent(h);
        if(parent==s->tab||parent==frame){
            RECT current=InParent(h,parent),client={};GetClientRect(parent,&client);
            LONG height=(pos->flags&SWP_NOSIZE)?current.bottom-current.top:pos->cy;
            if(height>0){
                if(pos->flags&SWP_NOMOVE)pos->x=current.left;
                pos->y=std::max(client.top,client.bottom-height);pos->flags&=~SWP_NOMOVE;
            }
        }
    }
    if(m==WM_WINDOWPOSCHANGED||m==WM_SHOWWINDOW||m==WM_SETFONT)QueueLayout(s);
    return result;
}
LRESULT CALLBACK TabProc(HWND h,UINT m,WPARAM w,LPARAM l,UINT_PTR,DWORD_PTR data){
    auto s=reinterpret_cast<State*>(data);HWND frame=s->frame;
    if(m==WM_NCDESTROY){RemoveWindowSubclass(h,TabProc,1);s->tab=nullptr;return DefSubclassProc(h,m,w,l);}
    LRESULT result=DefSubclassProc(h,m,w,l);
    if(GetState(frame)==s&&(m==WM_SIZE||m==WM_WINDOWPOSCHANGED||m==WM_PARENTNOTIFY))QueueLayout(s);
    return result;
}
// Native-style separators measured from the user's Explorer reference (host 0.5.5).
// Like every part of the classic Explorer window, the panel is its own closed
// frame: the file list above is an etched box, the status bar below a sunken
// pane, each separated by two face-coloured lines. The panel uses the list's
// etched style across the full width with the same two-line gaps, so no line
// meets or crosses a neighbour's.
void PaintFooter(HDC dc,RECT r,int pixel){
    FillRect(dc,&r,GetSysColorBrush(COLOR_BTNFACE));
    if(pixel<1)pixel=1;
    // The status bar window carries its own two-line top margin.
    RECT box={r.left,r.top+2*pixel,r.right,r.bottom};
    if(box.bottom-box.top>4)DrawEdge(dc,&box,EDGE_ETCHED,BF_RECT);
}
void Detach(State* s,bool destroying);
LRESULT CALLBACK FooterProc(HWND h,UINT m,WPARAM w,LPARAM l){
    auto s=reinterpret_cast<State*>(GetWindowLongPtrW(h,GWLP_USERDATA));
    if(m==WM_NCCREATE){s=reinterpret_cast<State*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams);SetWindowLongPtrW(h,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(s));}
    if(!s||s->detaching)return DefWindowProcW(h,m,w,l);
    switch(m){
    case WM_TIMER:
        if(!ControllerAlive(s)){Trace(L"Host: controller gone, closing picker window");PostMessageW(s->frame,detachMessage,reinterpret_cast<WPARAM>(s->footer),1);return 0;}
        if(w==3){KillTimer(h,3);QueueLayout(s);return 0;}
        if(w==PollTimer)SendState(s,false);else QueueLayout(s);
        return 0;
    case WM_SIZE:Arrange(s,LOWORD(l),true);InvalidateRect(h,nullptr,TRUE);return 0;
    case WM_COMMAND:{
        WORD id=LOWORD(w),code=HIWORD(w);
        if(id==OpenId&&code==BN_CLICKED){SendCommand(s,1);return 0;}
        if(id==CancelId){SendCancel(s);return 0;}
        if(id==DropId&&code==BN_CLICKED){if(auto c=OpenDropControl(s))SendControl(s,c->id,1,0,{});return 0;}
        if(id==NameId&&code==EN_CHANGE&&!s->updating){s->edited=true;EnableWindow(s->open,GetWindowTextLengthW(s->name)>0||s->request.mode==Picker::Folder);return 0;}
        if(id==TypesId&&code==CBN_SELCHANGE&&!s->updating){TypeChanged(s);return 0;}
        if(s->updating)return 0;
        for(auto& widget:s->widgets){
            if(widget.hwnd!=reinterpret_cast<HWND>(l))continue;
            auto& c=s->request.controls[widget.control];
            switch(c.kind){
            case Picker::Check:if(code==BN_CLICKED){c.value=SendMessageW(widget.hwnd,BM_GETCHECK,0,0)==BST_CHECKED;SendControl(s,c.id,0,c.value,{});}break;
            case Picker::Radio:if(code==BN_CLICKED&&c.value!=widget.item){c.value=widget.item;SendControl(s,c.id,0,c.value,{});}break;
            case Picker::Combo:if(code==CBN_SELCHANGE){LRESULT at=SendMessageW(widget.hwnd,CB_GETCURSEL,0,0);
                if(at>=0){c.value=static_cast<DWORD>(SendMessageW(widget.hwnd,CB_GETITEMDATA,at,0));SendControl(s,c.id,0,c.value,{});}}break;
            case Picker::Edit:if(code==EN_CHANGE){c.text=WindowText(widget.hwnd);SendControl(s,c.id,0,0,c.text);}break;
            case Picker::Push:case Picker::Menu:if(code==BN_CLICKED)SendControl(s,c.id,1,0,{});break;
            default:break;
            }
            return 0;
        }
        return 0;}
    case WM_APP+7:ShowControlMenu(s,static_cast<DWORD>(w));return 0;
    case WM_APP+8:SendState(s,false);return 0;
    case DM_GETDEFID:return MAKELRESULT(OpenId,DC_HASDEFID);
    case WM_CTLCOLORSTATIC:{
        // Keep statics on the face colour even when a theme would paint white.
        HDC dc=reinterpret_cast<HDC>(w);SetBkColor(dc,GetSysColor(COLOR_BTNFACE));SetTextColor(dc,GetSysColor(COLOR_BTNTEXT));
        return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_BTNFACE));}
    case WM_COPYDATA:{
        if(reinterpret_cast<HWND>(w)!=s->controller)return FALSE;
        auto copy=reinterpret_cast<COPYDATASTRUCT*>(l);if(!copy||copy->cbData>Picker::MaxBytes)return FALSE;
        switch(copy->dwData){
        case Picker::Ui:{Picker::Request q;if(!Picker::Decode(copy->lpData,copy->cbData,q))return FALSE;ApplyUi(s,q);return TRUE;}
        case Picker::Navigate:{Picker::Reader r(copy->lpData,copy->cbData);auto path=r.string();DWORD clear=r.number();if(!r.end())return FALSE;
            // A typed folder is entered and the name box emptied, as in the native dialog.
            if(clear){s->updating=true;SetWindowTextW(s->name,L"");s->updating=false;s->edited=false;s->lastDisplay.clear();
                EnableWindow(s->open,s->request.mode==Picker::Folder);}
            NavigateTo(s->frame,path);return TRUE;}
        case Picker::Prompt:{Picker::Reader r(copy->lpData,copy->cbData);DWORD kind=r.number();auto text=r.string(),caption=r.string();
            if(!r.end())return FALSE;
            // Owned by the picker frame, like the native dialog's own warnings.
            return MessageBoxW(s->frame,text.c_str(),caption.c_str(),kind==1?MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2:MB_OK|MB_ICONWARNING);}
        case Picker::ShowMenu:{Picker::Reader r(copy->lpData,copy->cbData);DWORD id=r.number();if(!r.end())return FALSE;
            PostMessageW(h,WM_APP+7,id,0);return TRUE;}
        case Picker::Close:s->closing=true;PostMessageW(s->frame,detachMessage,reinterpret_cast<WPARAM>(s->footer),1);return TRUE;
        }
        return FALSE;}
    case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);RECT r;GetClientRect(h,&r);PaintFooter(dc,r,Scale(s,1));EndPaint(h,&ps);return 0;}
    }
    return DefWindowProcW(h,m,w,l);
}
LRESULT CALLBACK FrameProc(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
void Detach(State* s,bool destroying){
    if(s->detaching)return;s->detaching=true;
    ReleaseBody(s,!destroying);
    if(!destroying)DockStatus(s);
    RemovePropW(s->frame,STATE);
    if(IsWindow(s->status))RemoveWindowSubclass(s->status,StatusProc,1);
    s->status=nullptr;RemovePropW(s->frame,STATUSBAR);
    s->widgets.clear();
    if(s->footer)DestroyWindow(s->footer);
    if(s->tab)RemoveWindowSubclass(s->tab,TabProc,1);
    RemoveWindowSubclass(s->frame,FrameProc,1);
    {std::lock_guard guard(lock);frames.erase(std::remove(frames.begin(),frames.end(),s->frame),frames.end());}
    if(s->process)CloseHandle(s->process);if(s->font)DeleteObject(s->font);delete s;
}
LRESULT CALLBACK FrameProc(HWND h,UINT m,WPARAM w,LPARAM l,UINT_PTR,DWORD_PTR data){
    auto s=reinterpret_cast<State*>(data);
    if(m==layoutMessage&&reinterpret_cast<HWND>(w)==s->footer){s->layoutQueued=false;Layout(s);return 0;}
    if(m==detachMessage&&reinterpret_cast<HWND>(w)==s->footer){
        bool close=l!=0;Detach(s,false);
        // The window was opened for this picker only.
        if(close)PostMessageW(h,WM_CLOSE,0,0);
        return 1;
    }
    if(m==WM_NCDESTROY){SendCancel(s);Detach(s,true);return DefSubclassProc(h,m,w,l);}
    if(m==WM_GETMINMAXINFO){LRESULT v=DefSubclassProc(h,m,w,l);auto mm=reinterpret_cast<MINMAXINFO*>(l);
        // Leave room for the tree and the file list above the footer.
        mm->ptMinTrackSize.x=std::max(mm->ptMinTrackSize.x,LONG(Scale(s,640)));
        mm->ptMinTrackSize.y=std::max(mm->ptMinTrackSize.y,LONG(Scale(s,330)+s->height));return v;}
    LRESULT result=DefSubclassProc(h,m,w,l);
    if((m==WM_SIZE||m==WM_DPICHANGED||m==WM_EXITSIZEMOVE)&&GetState(h)==s)Layout(s);
    return result;
}
bool Attach(HWND frame,const Pending& p){
    if(g_unloading||GetState(frame)||!ClassIs(frame,L"CabinetWClass")||!IsWindow(p.controller))return false;
    DWORD pid=0;GetWindowThreadProcessId(p.controller,&pid);
    HWND tab=FindWindowExW(frame,nullptr,L"ShellTabWindowClass",nullptr);if(!tab)return false;
    PinModule();
    auto s=new State{};s->frame=frame;s->tab=tab;s->controller=p.controller;s->requestId=p.id;
    s->process=OpenProcess(SYNCHRONIZE,FALSE,pid);s->request=p.request;s->height=Scale(s,84);
    s->footer=CreateWindowExW(WS_EX_CONTROLPARENT,Picker::FooterClass,L"",WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,0,0,0,frame,nullptr,g_module,s);
    if(!s->footer){if(s->process)CloseHandle(s->process);delete s;return false;}
    NONCLIENTMETRICSW metrics={sizeof(metrics)};
    if(SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(metrics),&metrics,0))s->font=CreateFontIndirectW(&metrics.lfMessageFont);
    s->label=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE|SS_LEFTNOWORDWRAP,0,0,0,0,s->footer,nullptr,g_module,nullptr);
    s->name=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,0,0,0,0,s->footer,reinterpret_cast<HMENU>(NameId),g_module,nullptr);
    SendMessageW(s->name,EM_SETLIMITTEXT,32767,0);
    s->typeLabel=CreateWindowW(L"STATIC",UiText(L"Files of type:",L"Тип файлов:"),WS_CHILD|WS_VISIBLE|SS_LEFTNOWORDWRAP,0,0,0,0,s->footer,nullptr,g_module,nullptr);
    s->types=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST,0,0,0,0,s->footer,reinterpret_cast<HMENU>(TypesId),g_module,nullptr);
    s->open=CreateWindowW(L"BUTTON",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,0,0,0,0,s->footer,reinterpret_cast<HMENU>(OpenId),g_module,nullptr);
    s->cancel=CreateWindowW(L"BUTTON",L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,s->footer,reinterpret_cast<HMENU>(CancelId),g_module,nullptr);
    for(HWND h:{s->label,s->name,s->typeLabel,s->types,s->open,s->cancel})SendMessageW(h,WM_SETFONT,reinterpret_cast<WPARAM>(Font(s)),TRUE);
    auto& q=s->request;
    if(q.filters.empty())SendMessageW(s->types,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(UiText(L"All Files (*.*)",L"Все файлы (*.*)")));
    else for(auto& f:q.filters){auto text=FilterLabel(f);SendMessageW(s->types,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(text.c_str()));}
    SendMessageW(s->types,CB_SETCURSEL,q.index-1,0);
    s->updating=true;SetWindowTextW(s->name,q.name.c_str());s->updating=false;s->edited=!q.name.empty();
    EnableWindow(s->open,!q.name.empty()||q.mode==Picker::Folder);
    BuildWidgets(s);SyncWidgets(s);
    if(!q.title.empty())SetWindowTextW(frame,q.title.c_str());
    SetPropW(frame,STATE,s);
    if(!SetWindowSubclass(tab,TabProc,1,reinterpret_cast<DWORD_PTR>(s))||!SetWindowSubclass(frame,FrameProc,1,reinterpret_cast<DWORD_PTR>(s))){Detach(s,false);return false;}
    if(!BindParts(s)){Detach(s,false);return false;}
    {std::lock_guard guard(lock);frames.push_back(frame);}
    Layout(s);
    RECT r={};GetClientRect(s->footer,&r);Arrange(s,r.right-r.left,true);
    SetTimer(s->footer,LiveTimer,1000,nullptr);SetTimer(s->footer,PollTimer,150,nullptr);
    // Tell the application which window is its picker, then deliver the first state.
    Picker::Writer w;w.number(p.id);
    w.number(static_cast<DWORD>(reinterpret_cast<UINT_PTR>(frame)&0xFFFFFFFF));
    Picker::Post(s->controller,s->footer,Picker::Ready,w,2000);
    SendState(s,true);
    SetForegroundWindow(frame);SetFocus(s->name);
    return true;
}
// Windows 11 opens folder windows in a separate "explorer.exe /factory"
// process. A probe is therefore posted to every new CabinetWClass of any
// Explorer process; the process owning the window claims it from the
// originating service, which hands the request over (Assign), and the window
// is embedded on its own thread, where IShellBrowser is available.
bool Matches(const Pending& p,HWND frame,const std::wstring& folder){
    if(p.taken||!p.launched||p.assigned||p.baseline.count(frame))return false;
    if(Picker::SamePath(folder,p.request.folder))return true;
    // A junction or a redirected known folder may open under another name;
    // after a grace period the only new window is accepted.
    size_t own=0;for(auto& q:pending)if(!q.assigned&&!q.taken)++own;
    return own==1&&GetTickCount64()-p.since>2500;
}
void Bind(HWND frame,const Pending& chosen,const std::wstring& folder){
    Trace(L"Host: window %p bound to request %lu (%ls)",frame,chosen.id,folder.c_str());
    if(!Attach(frame,chosen)){
        Trace(L"Host: attach failed");
        Picker::Writer w;w.number(chosen.id);Picker::Post(chosen.controller,service,Picker::Failed,w,1000);
        PostMessageW(frame,WM_CLOSE,0,0);
    }
}
void Probe(HWND frame,HWND origin,DWORD id){
    if(GetState(frame)||!ClassIs(frame,L"CabinetWClass")||!IsWindow(origin))return;
    std::wstring folder;if(!ReadShell(frame,folder,nullptr))return;
    Pending chosen;bool found=false;
    if(origin==service){
        std::lock_guard guard(lock);
        for(auto i=pending.begin();i!=pending.end();++i)if(i->id==id&&Matches(*i,frame,folder)){chosen=*i;pending.erase(i);found=true;break;}
    }else{
        Picker::Writer w;w.number(id);w.number(Picker::FromWindow(frame));w.string(folder);DWORD_PTR granted=0;
        if(!Picker::Post(origin,frame,Picker::Claim,w,3000,&granted)||granted!=TRUE)return;
        std::lock_guard guard(lock);
        for(auto i=pending.begin();i!=pending.end();++i)if(i->id==id&&i->assigned){chosen=*i;pending.erase(i);found=true;break;}
    }
    if(found)Bind(frame,chosen,folder);
}
HWND ServiceOf(DWORD pid){
    for(HWND h=nullptr;(h=FindWindowExW(HWND_MESSAGE,h,Picker::ServiceClass,nullptr))!=nullptr;){
        DWORD owner=0;GetWindowThreadProcessId(h,&owner);if(owner==pid)return h;
    }return nullptr;
}
LRESULT WINAPI DispatchHook(const MSG* msg){
    if(!g_unloading&&msg&&msg->hwnd){
        if(msg->message==probeMessage){Probe(msg->hwnd,reinterpret_cast<HWND>(msg->wParam),static_cast<DWORD>(msg->lParam));return 0;}
        HWND root=GetAncestor(msg->hwnd,GA_ROOT);State* s=GetState(root);
        if(s&&!s->detaching){
            bool list=ClassIs(msg->hwnd,L"SysListView32")&&ClassIs(GetParent(msg->hwnd),L"SHELLDLL_DefView");
            bool items=list||ClassIs(msg->hwnd,L"DirectUIHWND");
            if(items&&msg->message==WM_LBUTTONDOWN&&s->request.mode!=Picker::Save)s->edited=false;
            if(msg->message==WM_KEYDOWN&&msg->wParam==VK_ESCAPE&&!IsChild(s->footer,msg->hwnd)){SendCancel(s);return 0;}
            if(items&&((msg->message==WM_KEYDOWN&&msg->wParam==VK_RETURN)||msg->message==WM_LBUTTONDBLCLK)&&s->request.mode!=Picker::Folder){
                bool onItem=true;
                if(list&&msg->message==WM_LBUTTONDBLCLK){LVHITTESTINFO hit={};hit.pt={static_cast<short>(LOWORD(msg->lParam)),static_cast<short>(HIWORD(msg->lParam))};
                    SendMessageW(msg->hwnd,LVM_HITTEST,0,reinterpret_cast<LPARAM>(&hit));onItem=(hit.flags&LVHT_ONITEM)!=0;}
                std::wstring folder;std::vector<std::wstring> paths;
                // Files are accepted; folders keep Explorer's own navigation.
                if(onItem&&ReadShell(root,folder,&paths)&&!paths.empty()&&(s->request.mode==Picker::Multi||paths.size()==1)){
                    bool files=true;for(auto& p:paths){DWORD a=p.empty()?INVALID_FILE_ATTRIBUTES:GetFileAttributesW(p.c_str());
                        if(a==INVALID_FILE_ATTRIBUTES||(a&FILE_ATTRIBUTE_DIRECTORY)){files=false;break;}}
                    if(files){s->edited=false;SendCommand(s,3);return 0;}
                }
            }
            if(list&&msg->message==WM_KEYDOWN&&msg->wParam==VK_TAB&&!(GetKeyState(VK_SHIFT)&0x8000)){SetFocus(s->name);return 0;}
            if(!routingFooter&&(msg->hwnd==s->footer||IsChild(s->footer,msg->hwnd))&&msg->message>=WM_KEYFIRST&&msg->message<=WM_KEYLAST){
                if(msg->message==WM_KEYDOWN&&msg->wParam==VK_ESCAPE){
                    // A dropped-down list closes first, as in a dialog.
                    wchar_t cls[32]={};GetClassNameW(msg->hwnd,cls,32);
                    if(!(_wcsicmp(cls,L"COMBOBOX")==0&&SendMessageW(msg->hwnd,CB_GETDROPPEDSTATE,0,0))){SendCancel(s);return 0;}
                }
                routingFooter=true;bool handled=IsDialogMessageW(s->footer,const_cast<MSG*>(msg));routingFooter=false;if(handled)return 0;
            }
        }
    }
    return originalDispatch(msg);
}

// ---------------------------------------------------------------- service
BOOL CALLBACK CollectCabinets(HWND h,LPARAM l){
    if(ClassIs(h,L"CabinetWClass"))reinterpret_cast<std::set<HWND>*>(l)->insert(h);
    return TRUE;
}
void Launch(const std::wstring& folder){
    std::wstring args=L"/n,\""+folder+L"\"";
    SHELLEXECUTEINFOW si={sizeof(si)};si.fMask=SEE_MASK_FLAG_NO_UI;si.lpFile=L"explorer.exe";si.lpParameters=args.c_str();si.nShow=SW_SHOWNORMAL;
    if(!ShellExecuteExW(&si))Trace(L"Host: explorer launch failed %lu",GetLastError());
}
LRESULT CALLBACK ServiceProc(HWND h,UINT m,WPARAM w,LPARAM l){
    switch(m){
    case WM_COPYDATA:{
        HWND sender=reinterpret_cast<HWND>(w);auto copy=reinterpret_cast<COPYDATASTRUCT*>(l);
        if(g_unloading||!copy||!IsWindow(sender))return FALSE;
        Picker::Reader r(copy->lpData,copy->cbData);DWORD id=r.number();
        if(copy->dwData==Picker::Claim){
            DWORD frameValue=r.number();auto folder=r.string();HWND frame=Picker::ToWindow(frameValue);
            if(!r.end()||frame!=sender||!ClassIs(frame,L"CabinetWClass"))return FALSE;
            Pending chosen;bool found=false;
            {std::lock_guard guard(lock);
                for(auto& p:pending)if(p.id==id&&Matches(p,frame,folder)){p.taken=true;chosen=p;found=true;break;}}
            if(!found)return FALSE;
            DWORD pid=0;GetWindowThreadProcessId(frame,&pid);HWND target=ServiceOf(pid);
            Picker::Writer out;out.number(id);out.number(Picker::FromWindow(chosen.controller));Picker::Put(out,chosen.request);
            DWORD_PTR accepted=0;bool ok=target&&Picker::Post(target,h,Picker::Assign,out,3000,&accepted)&&accepted;
            std::lock_guard guard(lock);
            for(auto i=pending.begin();i!=pending.end();++i)if(i->id==id&&!i->assigned){if(ok)pending.erase(i);else i->taken=false;break;}
            if(ok)Trace(L"Host: request %lu handed to Explorer process %lu",id,pid);
            return ok?TRUE:FALSE;
        }
        if(copy->dwData==Picker::Assign){
            if(!ClassIs(sender,Picker::ServiceClass))return FALSE;
            Pending p;p.id=id;p.controller=Picker::ToWindow(r.number());p.assigned=true;p.launched=true;p.since=GetTickCount64();
            if(!Picker::Get(r,p.request)||!r.end()||!IsWindow(p.controller)||!ClassIs(p.controller,Picker::ControllerClass))return FALSE;
            std::lock_guard guard(lock);if(pending.size()>=32)return FALSE;pending.push_back(std::move(p));return TRUE;
        }
        HWND controller=sender;
        if(!ClassIs(controller,Picker::ControllerClass))return FALSE;
        if(copy->dwData==Picker::Abandon){
            if(!r.end())return FALSE;std::lock_guard guard(lock);
            pending.erase(std::remove_if(pending.begin(),pending.end(),[&](const Pending& p){return p.id==id&&p.controller==controller;}),pending.end());
            return TRUE;
        }
        if(copy->dwData!=Picker::Open)return FALSE;
        Pending p;p.id=id;p.controller=controller;
        if(!Picker::Get(r,p.request)||!r.end())return FALSE;
        DWORD attr=GetFileAttributesW(p.request.folder.c_str());
        if(attr==INVALID_FILE_ATTRIBUTES||!(attr&FILE_ATTRIBUTE_DIRECTORY)||p.request.folder.find(L'"')!=std::wstring::npos)return FALSE;
        EnumWindows(CollectCabinets,reinterpret_cast<LPARAM>(&p.baseline));
        p.since=GetTickCount64();
        {std::lock_guard guard(lock);if(pending.size()>=16)return FALSE;pending.push_back(std::move(p));}
        PostMessageW(h,WM_APP+1,0,0);
        return TRUE;}
    case WM_APP+1:{
        std::vector<std::wstring> folders;
        {std::lock_guard guard(lock);for(auto& p:pending)if(!p.launched){p.launched=true;p.since=GetTickCount64();folders.push_back(p.request.folder);}}
        for(auto& f:folders){Trace(L"Host: opening picker window at %ls",f.c_str());Launch(f);}
        return 0;}
    case WM_TIMER:{
        std::vector<std::pair<DWORD,HWND>> expired;std::vector<std::pair<HWND,DWORD>> probes;
        {
            std::lock_guard guard(lock);ULONGLONG now=GetTickCount64();std::set<HWND> current;bool enumerated=false;
            for(auto i=pending.begin();i!=pending.end();){
                if(now-i->since>20000||!IsWindow(i->controller)){
                    if(!i->assigned&&!i->taken)expired.push_back({i->id,i->controller});
                    i=pending.erase(i);continue;
                }
                if(i->launched&&!i->taken&&!i->assigned){
                    if(!enumerated){EnumWindows(CollectCabinets,reinterpret_cast<LPARAM>(&current));enumerated=true;}
                    for(HWND f:current)if(!i->baseline.count(f))probes.push_back({f,i->id});
                }
                ++i;
            }
        }
        for(auto& e:expired){Trace(L"Host: request %lu timed out",e.first);Picker::Writer out;out.number(e.first);Picker::Post(e.second,h,Picker::Failed,out,1000);}
        for(auto& p:probes)if(!GetState(p.first))PostMessageW(p.first,probeMessage,reinterpret_cast<WPARAM>(h),p.second);
        return 0;}
    case WM_CLOSE:DestroyWindow(h);return 0;
    case WM_DESTROY:PostQuitMessage(0);return 0;
    }
    return DefWindowProcW(h,m,w,l);
}
DWORD WINAPI ServiceThread(LPVOID ready){
    HRESULT co=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
    WNDCLASSW wc={};wc.lpfnWndProc=ServiceProc;wc.hInstance=g_module;wc.lpszClassName=Picker::ServiceClass;
    RegisterClassW(&wc);
    service=CreateWindowExW(0,Picker::ServiceClass,L"",0,0,0,0,0,HWND_MESSAGE,nullptr,g_module,nullptr);
    if(service){
        // Elevated controllers can talk to us anyway (higher to lower is allowed).
        SetTimer(service,1,100,nullptr);
    }
    SetEvent(static_cast<HANDLE>(ready));
    if(service){MSG msg;while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}}
    service=nullptr;UnregisterClassW(Picker::ServiceClass,g_module);
    if(SUCCEEDED(co))CoUninitialize();
    return 0;
}
bool Init(){
    probeMessage=RegisterWindowMessageW(L"ExplorerPicker.Probe.V6");layoutMessage=RegisterWindowMessageW(L"ExplorerPicker.Layout.V6");
    detachMessage=RegisterWindowMessageW(L"ExplorerPicker.Detach.V6");menuMessage=RegisterWindowMessageW(L"ExplorerPicker.Menu.V6");
    WNDCLASSW wc={};wc.style=CS_HREDRAW|CS_VREDRAW;wc.lpfnWndProc=FooterProc;wc.hInstance=g_module;wc.lpszClassName=Picker::FooterClass;
    wc.hbrBackground=GetSysColorBrush(COLOR_BTNFACE);wc.hCursor=LoadCursor(nullptr,IDC_ARROW);
    if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return false;
    if(!Wh_SetFunctionHook(reinterpret_cast<void*>(DispatchMessageW),reinterpret_cast<void*>(DispatchHook),reinterpret_cast<void**>(&originalDispatch)))return false;
    HANDLE ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);if(!ready)return false;
    serviceThread=CreateThread(nullptr,0,ServiceThread,ready,0,nullptr);
    if(serviceThread)WaitForSingleObject(ready,5000);
    CloseHandle(ready);
    return serviceThread!=nullptr;
}
void BeforeUninit(){
    std::vector<HWND> copy;{std::lock_guard guard(lock);copy=frames;}
    for(HWND h:copy){
        State* s=GetState(h);if(!s)continue;
        SendCancel(s);DWORD_PTR ignored;
        SendMessageTimeoutW(h,detachMessage,reinterpret_cast<WPARAM>(s->footer),0,SMTO_ABORTIFHUNG|SMTO_BLOCK,1500,&ignored);
    }
    if(service)PostMessageW(service,WM_CLOSE,0,0);
}
void Uninit(){
    if(serviceThread){WaitForSingleObject(serviceThread,3000);CloseHandle(serviceThread);serviceThread=nullptr;}
    UnregisterClassW(Picker::FooterClass,g_module);
}
}

// ================================================================ controller
// Runs on its own thread inside the application. Talks to the host through
// window messages only; application callbacks are delivered on the
// application's thread through a message-only receiver window.
namespace Ctl {
enum EventKind:DWORD {StateEvent=2,FileOkEvent=3,ControlEvent=4,RefreshEvent=5};
struct Event {
    DWORD kind=StateEvent,index=1;
    std::wstring folder;std::vector<std::wstring> paths;
    DWORD control=0,action=0,value=Picker::None,dropItem=Picker::None;std::wstring text;
    Picker::Request ui;bool hasUi=false;HWND frame=nullptr;
};
struct Target {
    HWND controller=nullptr,frame=nullptr;
    virtual HRESULT Notify(Event& e)=0;
    virtual const Picker::Request* UiState()const{return nullptr;}
    virtual ~Target()=default;
};
constexpr UINT EventMessage=WM_APP+0x51;
struct Result {int status=2;DWORD index=1;std::vector<std::wstring> paths;};

bool Elevated(){
    HANDLE token=nullptr;if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token))return false;
    TOKEN_ELEVATION elevation={};DWORD bytes=0;
    bool elevated=GetTokenInformation(token,TokenElevation,&elevation,sizeof(elevation),&bytes)&&elevation.TokenIsElevated;
    CloseHandle(token);return elevated;
}
HWND FindService(){
    DWORD shellPid=0;GetWindowThreadProcessId(GetShellWindow(),&shellPid);HWND any=nullptr;
    for(HWND h=nullptr;(h=FindWindowExW(HWND_MESSAGE,h,Picker::ServiceClass,nullptr))!=nullptr;){
        DWORD pid=0;GetWindowThreadProcessId(h,&pid);if(pid==shellPid)return h;if(!any)any=h;
    }return any;
}

struct Controller {
    Picker::Request request;HWND receiver=nullptr;UINT testMode=0;bool allowElevated=true;
    Result result;
    HWND panel=nullptr,footer=nullptr,frame=nullptr,service=nullptr;DWORD requestId=0;
    std::wstring folder,eventFolder;std::vector<std::wstring> paths;
    bool done=false,busy=false,accepting=false,hasState=false,bound=false,closedByHost=false,testStarted=false;
    ULONGLONG started=0;
    std::vector<BYTE> lastUi;
    struct Cmd {DWORD kind=1,edited=0,index=1,drop=Picker::None;std::wstring typed,folder;std::vector<std::wstring> paths;};
    struct Change {DWORD id=0,action=0,value=0;std::wstring text;};
    std::vector<Cmd> commands;std::vector<Change> changes;bool stateQueued=false;
    DWORD pendingDrop=Picker::None;

    const wchar_t* Caption()const{return request.mode==Picker::Save?UiText(L"Save As",L"Сохранить как"):request.mode==Picker::Folder?UiText(L"Select Folder",L"Выбор папки"):UiText(L"Open",L"Открыть");}
    DWORD_PTR Ask(DWORD kind,const std::wstring& text){
        if(testMode==6||testMode==7){Trace(L"Test prompt: %ls",text.c_str());return kind==1?(testMode==7?IDYES:IDNO):IDOK;}
        Picker::Writer w;w.number(kind);w.string(text);w.string(kind==1?UiText(L"Confirm Save As",L"Подтверждение сохранения"):Caption());
        DWORD_PTR answer=0;if(!Picker::Post(footer,panel,Picker::Prompt,w,60000,&answer,true))return kind==1?IDNO:IDOK;return answer;
    }
    void Warn(const std::wstring& text){Ask(0,text);}
    void Navigate(const std::wstring& path,bool clearName=true){Picker::Writer w;w.string(path);w.number(clearName);Picker::Post(footer,panel,Picker::Navigate,w,2000);}
    void PushUi(const Picker::Request& ui){
        request.controls=ui.controls;request.okLabel=ui.okLabel;request.nameLabel=ui.nameLabel;request.cancelLabel=ui.cancelLabel;
        if(!ui.extension.empty())request.extension=ui.extension;
        auto bytes=Picker::Encode(request);if(!bytes.good||bytes.data==lastUi||!footer)return;
        lastUi=bytes.data;Picker::Post(footer,panel,Picker::Ui,bytes,2000);
    }
    HRESULT Notify(Event& e){
        if(!receiver)return S_OK;
        if(busy||!IsWindow(receiver))return E_ABORT;
        e.index=request.index;e.frame=frame;busy=true;DWORD_PTR response=static_cast<DWORD_PTR>(E_FAIL);
        BOOL sent=SendMessageTimeoutW(receiver,EventMessage,reinterpret_cast<WPARAM>(panel),reinterpret_cast<LPARAM>(&e),
            SMTO_ABORTIFHUNG|SMTO_NOTIMEOUTIFNOTHUNG,30000,&response);
        busy=false;
        HRESULT hr=sent?static_cast<HRESULT>(static_cast<DWORD>(response)):E_ABORT;
        if(e.hasUi)PushUi(e.ui);
        if(hr!=S_OK)Trace(L"Event %lu -> 0x%08lX",e.kind,static_cast<unsigned long>(hr));
        return hr;
    }
    void Finish(int status){
        if(done)return;done=true;result.status=status;
        if(panel)DestroyWindow(panel);
    }
    void Complete(const std::vector<std::wstring>& chosen){
        if(receiver){
            Event e;e.kind=FileOkEvent;e.folder=folder;e.paths=chosen;e.dropItem=pendingDrop;
            if(Notify(e)!=S_OK){Trace(L"Validation declined by the application; picker stays open");pendingDrop=Picker::None;return;}
        }
        result.paths=chosen;result.index=request.index;
        for(auto& p:chosen)Trace(L"Selected: %ls",p.c_str());
        Finish(0);
    }
    std::wstring Extension()const{
        if(!request.filters.empty()&&request.index<=request.filters.size()){
            auto pattern=request.filters[request.index-1].pattern;auto semi=pattern.find(L';');pattern.resize(semi==std::wstring::npos?pattern.size():semi);
            if(pattern.rfind(L"*.",0)==0&&pattern.size()>2&&pattern.substr(2).find_first_of(L"*?\\/: ")==std::wstring::npos)return pattern.substr(2);
        }
        std::wstring ext=request.extension;while(!ext.empty()&&ext.front()==L'.')ext.erase(0,1);
        return ext.find_first_of(L"*?\\/: ")==std::wstring::npos?ext:L"";
    }
    static bool ValidSaveName(const std::wstring& path){
        auto leaf=Picker::Leaf(path);
        if(leaf.empty()||leaf.size()>255||leaf.back()==L'.'||leaf.back()==L' '||leaf.find_first_of(L"<>:\"/\\|?*")!=std::wstring::npos)return false;
        for(wchar_t c:leaf)if(c<32)return false;
        if(path.rfind(L"\\\\?\\",0)==0||path.rfind(L"\\\\.\\",0)==0||path.find(L':',2)!=std::wstring::npos)return false;
        auto stem=leaf.substr(0,leaf.find(L'.'));while(!stem.empty()&&(stem.back()==L' '||stem.back()==L'.'))stem.pop_back();
        CharUpperBuffW(stem.data(),static_cast<DWORD>(stem.size()));
        if(stem==L"CON"||stem==L"PRN"||stem==L"AUX"||stem==L"NUL")return false;
        if(stem.size()==4&&(stem.substr(0,3)==L"COM"||stem.substr(0,3)==L"LPT")&&
           ((stem[3]>=L'1'&&stem[3]<=L'9')||stem[3]==L'\u00B9'||stem[3]==L'\u00B2'||stem[3]==L'\u00B3'))return false;
        return true;
    }
    static bool ParseNames(const std::wstring& text,std::vector<std::wstring>& names){
        size_t pos=text.find_first_not_of(L" \t");if(pos==std::wstring::npos)return false;
        if(text[pos]!=L'"'){auto name=text.substr(pos);while(!name.empty()&&name.back()==L' ')name.pop_back();names.push_back(name);return true;}
        while(pos<text.size()){
            if(text[pos]!=L'"')return false;size_t end=text.find(L'"',pos+1);if(end==std::wstring::npos||end==pos+1)return false;
            names.push_back(text.substr(pos+1,end-pos-1));if(names.size()>Picker::MaxFiles)return false;
            pos=text.find_first_not_of(L" \t",end+1);if(pos==std::wstring::npos)return true;
        }return !names.empty();
    }
    void AcceptPaths(std::vector<std::wstring> list){
        if(list.empty()||list.size()>Picker::MaxFiles||(request.mode!=Picker::Multi&&list.size()!=1)){
            Warn(request.mode==Picker::Multi||list.size()<=1?UiText(L"Select a file.",L"Выберите файл."):UiText(L"Select only one file.",L"Выберите только один файл."));return;
        }
        for(auto& path:list){
            bool absolute=(path.size()>2&&path[1]==L':'&&(path[2]==L'\\'||path[2]==L'/'))||path.rfind(L"\\\\",0)==0;
            if(!absolute){
                if(folder.empty()){Warn(UiText(L"This folder is not part of the file system. Choose another folder.",L"Эта папка не находится в файловой системе. Выберите другую папку."));return;}
                path=folder+(folder.back()==L'\\'?L"":L"\\")+path;
            }
            wchar_t full[32768];DWORD n=GetFullPathNameW(path.c_str(),32768,full,nullptr);
            if(!n||n>=32768){Warn(UiText(L"The path is not valid.",L"Некорректный путь."));return;}path=full;
        }
        if(request.mode==Picker::Folder){
            DWORD attr=GetFileAttributesW(list[0].c_str());
            if(attr==INVALID_FILE_ATTRIBUTES||!(attr&FILE_ATTRIBUTE_DIRECTORY)){Warn(UiText(L"Select an existing folder.",L"Выберите существующую папку."));return;}
            Complete(list);return;
        }
        // A typed or selected folder is entered, as in the native dialog.
        if(list.size()==1){DWORD a=GetFileAttributesW(list[0].c_str());if(a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)){Navigate(list[0]);return;}}
        if(request.mode==Picker::Save){
            auto& path=list[0];auto leaf=Picker::Leaf(path);auto ext=Extension();
            if(leaf.find(L'.')==std::wstring::npos&&!ext.empty())path+=L"."+ext;
            if(!ValidSaveName(path)){Warn(UiText(L"The file name is not valid. Check it for invalid characters, and for dots or spaces at the end of the name.",L"Недопустимое имя файла. Проверьте символы, точки и пробелы в конце имени."));return;}
            auto parent=Picker::Parent(path);DWORD pa=GetFileAttributesW(parent.c_str());
            if(pa==INVALID_FILE_ATTRIBUTES||!(pa&FILE_ATTRIBUTE_DIRECTORY)){Warn(UiText(L"The folder to save the file in was not found.",L"Папка для сохранения не найдена."));return;}
        }
        for(auto& path:list){
            DWORD attr=GetFileAttributesW(path.c_str());
            if(attr!=INVALID_FILE_ATTRIBUTES&&(attr&FILE_ATTRIBUTE_DIRECTORY)){Warn(UiText(L"Select files, not folders.",L"Выберите файлы, а не папки."));return;}
            if(request.mode!=Picker::Save&&attr==INVALID_FILE_ATTRIBUTES){
                Warn(UiText(L"",L"«")+Picker::Leaf(path)+UiText(L"\nFile not found.\nPlease verify the correct file name was given.",L"»\nФайл не найден. Проверьте правильность имени файла."));return;
            }
            if((request.flags&Picker::SameDirectory)&&!Picker::SamePath(Picker::Parent(path),Picker::Parent(list[0]))){
                Warn(UiText(L"Select files from a single folder.",L"Выберите файлы из одной папки."));return;
            }
            if((request.flags&Picker::StrictTypes)&&!request.filters.empty()&&
               !PathMatchSpecW(Picker::Leaf(path).c_str(),request.filters[request.index-1].pattern.c_str())){
                Warn(UiText(L"The file name extension does not match the selected file type.",L"Расширение файла не соответствует выбранному типу."));return;
            }
            if(request.mode==Picker::Save&&attr!=INVALID_FILE_ATTRIBUTES){
                if(attr&FILE_ATTRIBUTE_READONLY){Warn(UiText(L"",L"Файл «")+Picker::Leaf(path)+UiText(L"\nThis file is set to read-only.\nTry again with a different file name.",L"» доступен только для чтения.\nВыберите другое имя."));return;}
                if(Ask(1,UiText(L"",L"Файл «")+Picker::Leaf(path)+UiText(L" already exists.\nDo you want to replace it?",L"» уже существует.\nЗаменить его?"))!=IDYES){Trace(L"Overwrite declined");if(testMode==6)PostMessageW(panel,WM_APP+4,1,0);return;}
                Trace(L"Overwrite confirmed");
            }
        }
        Complete(list);
    }
    void Accept(const Cmd& c){
        if(accepting)return;accepting=true;
        struct Reset{bool& b;~Reset(){b=false;}} reset{accepting};
        folder=c.folder;request.index=c.index;pendingDrop=c.drop;
        std::vector<std::wstring> list;
        if(c.edited){
            if(!ParseNames(c.typed,list)){
                if(request.mode==Picker::Folder){Complete({folder});return;}
                Warn(UiText(L"Type a file name.",L"Введите имя файла."));return;
            }
            AcceptPaths(list);return;
        }
        if(request.mode==Picker::Folder){
            if(c.paths.empty()){if(folder.empty()){Warn(UiText(L"Select a file system folder.",L"Выберите папку файловой системы."));return;}AcceptPaths({folder});return;}
            AcceptPaths(c.paths);return;
        }
        if(c.paths.size()==1&&!c.paths[0].empty()){
            DWORD a=GetFileAttributesW(c.paths[0].c_str());
            if(a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)){Navigate(c.paths[0]);return;}
        }
        for(auto& p:c.paths)if(p.empty()){Warn(UiText(L"The selected item is not a file.",L"Выбранный объект не является файлом."));return;}
        if(c.paths.empty()){Warn(request.mode==Picker::Save?UiText(L"Type a file name.",L"Введите имя файла."):UiText(L"Select a file.",L"Выделите файл."));return;}
        AcceptPaths(c.paths);
    }
    void ProcessState(){
        stateQueued=false;if(done||!receiver)return;
        Event e;e.kind=StateEvent;e.folder=folder;e.paths=paths;
        HRESULT hr=Notify(e);
        if(hr==S_OK){eventFolder=folder;return;}
        if(!Picker::SamePath(folder,eventFolder)){
            // Explorer owns navigation; restore the last accepted folder on veto.
            if(eventFolder.empty()){Trace(L"Initial folder rejected -> native dialog");Finish(2);return;}
            Navigate(eventFolder,false);
        }
    }
    void ProcessChanges(){
        auto list=std::move(changes);changes.clear();
        for(auto& c:list){
            if(done)return;
            auto control=request.Find(c.id);if(!control)continue;
            if(c.action==0){
                if(control->kind==Picker::Edit)control->text=c.text;else control->value=c.value;
            }
            Event e;e.kind=ControlEvent;e.folder=folder;e.paths=paths;e.control=c.id;e.action=c.action;e.value=c.value;e.text=c.text;
            if(receiver)Notify(e);
            control=request.Find(c.id);
            if(control&&c.action==1&&(control->kind==Picker::Menu||control->kind==Picker::OpenDrop)){
                Picker::Writer w;w.number(c.id);Picker::Post(footer,panel,Picker::ShowMenu,w,2000);
            }
        }
    }
    void StartTest(){
        if(testStarted||!testMode||!hasState)return;testStarted=true;
        if(testMode==2){Trace(L"Test: cancel");Finish(1);return;}
        Cmd c;c.kind=1;c.edited=1;c.index=request.index;c.folder=folder;
        if(request.mode==Picker::Save)c.typed=request.name;
        else if(request.mode==Picker::Multi)c.typed=L"\"README.md\" \"тест два файла.txt\"";
        else if(request.mode==Picker::Folder)c.typed=folder;
        else c.typed=L"README.md";
        commands.push_back(c);PostMessageW(panel,WM_APP+1,0,0);
    }
    static LRESULT CALLBACK Proc(HWND h,UINT m,WPARAM w,LPARAM l){
        if(m==WM_NCCREATE){SetWindowLongPtrW(h,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams));return TRUE;}
        auto self=reinterpret_cast<Controller*>(GetWindowLongPtrW(h,GWLP_USERDATA));
        if(!self)return DefWindowProcW(h,m,w,l);
        return self->Handle(h,m,w,l);
    }
    LRESULT Handle(HWND h,UINT m,WPARAM w,LPARAM l){
        switch(m){
        case WM_COPYDATA:{
            auto copy=reinterpret_cast<COPYDATASTRUCT*>(l);HWND sender=reinterpret_cast<HWND>(w);
            if(!copy||done)return FALSE;
            Picker::Reader r(copy->lpData,copy->cbData);
            if(copy->dwData==Picker::Failed){
                wchar_t cls[64]={};GetClassNameW(sender,cls,64);
                if(wcscmp(cls,Picker::ServiceClass)||r.number()!=requestId||!r.end())return FALSE;
                Trace(L"Host could not open the picker window -> native dialog");PostMessageW(h,WM_APP+4,2,0);return TRUE;
            }
            if(copy->dwData==Picker::Ready){
                DWORD id=r.number();r.number();
                if(!r.end()||id!=requestId||bound)return FALSE;
                wchar_t cls[64]={};GetClassNameW(sender,cls,64);if(wcscmp(cls,Picker::FooterClass))return FALSE;
                footer=sender;frame=GetAncestor(sender,GA_ROOT);bound=true;
                Trace(L"Picker window ready: frame %p",frame);return TRUE;
            }
            if(sender!=footer||!footer)return FALSE;
            switch(copy->dwData){
            case Picker::State:{
                DWORD index=r.number();auto f=r.string();auto list=r.strings();if(!r.end())return FALSE;
                if(index<1||index>std::max<size_t>(1,request.filters.size()))return FALSE;
                request.index=index;folder=f;paths=std::move(list);hasState=true;
                if(!stateQueued){stateQueued=true;PostMessageW(h,WM_APP+2,0,0);}
                if(testMode&&!testStarted)PostMessageW(h,WM_APP+5,0,0);
                return TRUE;}
            case Picker::Command:{
                Cmd c;c.kind=r.number();c.edited=r.number();c.index=r.number();c.typed=r.string();c.folder=r.string();c.paths=r.strings();c.drop=r.number();
                if(!r.end()||c.index<1||c.index>std::max<size_t>(1,request.filters.size()))return FALSE;
                commands.push_back(std::move(c));PostMessageW(h,WM_APP+1,0,0);return TRUE;}
            case Picker::ControlChange:{
                Change c;c.id=r.number();c.action=r.number();c.value=r.number();c.text=r.string();if(!r.end()||c.action>1)return FALSE;
                changes.push_back(std::move(c));PostMessageW(h,WM_APP+3,0,0);return TRUE;}
            case Picker::Cancel:closedByHost=true;PostMessageW(h,WM_APP+4,1,0);return TRUE;
            }
            return FALSE;}
        case WM_APP+1:{auto list=std::move(commands);commands.clear();for(auto& c:list){if(done)break;Accept(c);}return 0;}
        case WM_APP+2:ProcessState();return 0;
        case WM_APP+3:ProcessChanges();return 0;
        case WM_APP+4:Finish(static_cast<int>(w));return 0;
        case WM_APP+5:StartTest();return 0;
        case WM_APP+6:if(!done&&receiver&&!busy){Event e;e.kind=RefreshEvent;e.folder=folder;e.paths=paths;Notify(e);}return 0;
        case WM_TIMER:
            if(done)return 0;
            if(receiver&&!IsWindow(receiver)){Finish(2);return 0;}
            if(!bound&&GetTickCount64()-started>25000){Trace(L"No picker window -> native dialog");Finish(2);return 0;}
            if(bound&&!IsWindow(footer)){closedByHost=true;Finish(1);return 0;}
            return 0;
        case WM_CLOSE:Finish(result.status==0?0:1);return 0;
        case WM_DESTROY:
            KillTimer(h,1);
            if(bound&&!closedByHost&&IsWindow(footer)){Picker::Writer none;Picker::Post(footer,h,Picker::Close,none,1500);}
            if(!bound&&service){Picker::Writer w;w.number(requestId);Picker::Post(service,h,Picker::Abandon,w,1000);}
            Trace(result.status==0?L"RESULT selected":result.status==1?L"RESULT cancelled":L"RESULT native");
            PostQuitMessage(0);return 0;
        }
        return DefWindowProcW(h,m,w,l);
    }
    void Run(){
        started=GetTickCount64();
        if(Elevated()&&!allowElevated){Trace(L"Elevated process: picker disabled by settings");return;}
        service=FindService();
        if(!service){Trace(L"Explorer Picker is not active in Explorer -> native dialog");return;}
        WNDCLASSW wc={};wc.lpfnWndProc=Proc;wc.hInstance=g_module;wc.lpszClassName=Picker::ControllerClass;
        if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return;
        panel=CreateWindowExW(0,Picker::ControllerClass,L"",0,0,0,0,0,HWND_MESSAGE,nullptr,g_module,this);
        if(!panel)return;
        // Explorer runs with ordinary rights; let it answer an elevated caller.
        ChangeWindowMessageFilterEx(panel,WM_COPYDATA,MSGFLT_ALLOW,nullptr);
        requestId=(GetTickCount()^(GetCurrentThreadId()<<8)^static_cast<DWORD>(reinterpret_cast<UINT_PTR>(this)))|1;
        Picker::Writer w;w.number(requestId);Picker::Put(w,request);
        DWORD_PTR accepted=0;
        if(!Picker::Post(service,panel,Picker::Open,w,5000,&accepted)||!accepted){
            Trace(L"Explorer refused the request -> native dialog");done=true;DestroyWindow(panel);
        }else{
            result.status=1;Trace(L"Request %lu sent to Explorer (%ls)",requestId,request.folder.c_str());
            SetTimer(panel,1,250,nullptr);
        }
        MSG msg;while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}
        panel=nullptr;
    }
    static DWORD WINAPI Thread(LPVOID p){static_cast<Controller*>(p)->Run();return 0;}
};

struct EventReceiver {
    HWND window=nullptr;Target* target;const std::vector<DWORD>* indices;
    static LRESULT CALLBACK Proc(HWND h,UINT m,WPARAM w,LPARAM l){
        if(m==WM_NCCREATE){SetWindowLongPtrW(h,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams));return TRUE;}
        auto self=reinterpret_cast<EventReceiver*>(GetWindowLongPtrW(h,GWLP_USERDATA));
        if(m==EventMessage&&self&&l){
            auto e=reinterpret_cast<Event*>(l);
            if(!e->index||e->index>self->indices->size())return E_INVALIDARG;
            e->index=(*self->indices)[e->index-1];
            self->target->controller=reinterpret_cast<HWND>(w);self->target->frame=e->frame;
            HRESULT hr=self->target->Notify(*e);
            if(auto ui=self->target->UiState()){e->ui=*ui;e->hasUi=true;}
            return hr;
        }
        return DefWindowProcW(h,m,w,l);
    }
    EventReceiver(Target* t,const std::vector<DWORD>& mapping):target(t),indices(&mapping){
        if(!t)return;
        static const wchar_t name[]=L"ExplorerPickerEventsV6";
        WNDCLASSW wc={};wc.hInstance=g_module;wc.lpfnWndProc=Proc;wc.lpszClassName=name;
        if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return;
        window=CreateWindowExW(0,name,L"",0,0,0,0,0,HWND_MESSAGE,nullptr,g_module,this);
    }
    ~EventReceiver(){if(target)target->controller=nullptr;if(window)DestroyWindow(window);}
};
struct OwnerModal {
    HWND h;bool enabled;
    explicit OwnerModal(HWND owner):h(owner),enabled(owner&&IsWindowEnabled(owner)){if(enabled)EnableWindow(h,FALSE);}
    ~OwnerModal(){if(enabled&&IsWindow(h)){EnableWindow(h,TRUE);SetForegroundWindow(h);}}
};
thread_local bool inPicker=false;
struct PickerGuard {PickerGuard(){inPicker=true;}~PickerGuard(){inPicker=false;}};
std::atomic<int> activePickers=0;

// Returns 0 selected, 1 cancelled, 2 unavailable (caller shows the native dialog).
int Run(HWND owner,const Picker::Request& request,Result& result,Target* events,const std::vector<DWORD>& indices,UINT testMode){
    if(g_unloading)return 2;
    PinModule();
    EventReceiver receiver(events,indices);if(events&&!receiver.window)return 2;
    auto controller=new(std::nothrow)Controller;if(!controller)return 2;
    controller->request=request;controller->receiver=receiver.window;controller->testMode=testMode;
    controller->allowElevated=Wh_GetIntSetting(L"elevatedApps")!=0;
    HANDLE thread=CreateThread(nullptr,0,Controller::Thread,controller,0,nullptr);
    if(!thread){delete controller;return 2;}
    ++activePickers;
    {
        OwnerModal modal(owner);bool quit=false;int quitCode=0;
        for(;;){
            DWORD wait=MsgWaitForMultipleObjects(1,&thread,FALSE,INFINITE,QS_ALLINPUT);
            if(wait==WAIT_OBJECT_0||wait==WAIT_FAILED)break;
            MSG msg;while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){
                if(msg.message==WM_QUIT){quit=true;quitCode=static_cast<int>(msg.wParam);continue;}
                TranslateMessage(&msg);DispatchMessageW(&msg);
            }
        }
        if(quit)PostQuitMessage(quitCode);
    }
    --activePickers;
    WaitForSingleObject(thread,INFINITE);CloseHandle(thread);
    result=controller->result;delete controller;
    if(result.status)return result.status==1?1:2;
    if(result.index<1||result.index>std::max<size_t>(1,request.filters.size()))return 2;
    if(request.mode!=Picker::Multi&&result.paths.size()!=1)return 2;
    for(auto& path:result.paths){
        if(path.empty())return 2;
        DWORD attr=GetFileAttributesW(path.c_str());
        if(request.mode==Picker::Folder){if(attr==INVALID_FILE_ATTRIBUTES||!(attr&FILE_ATTRIBUTE_DIRECTORY))return 2;continue;}
        if(attr!=INVALID_FILE_ATTRIBUTES&&(attr&FILE_ATTRIBUTE_DIRECTORY))return 2;
        if(request.mode!=Picker::Save&&attr==INVALID_FILE_ATTRIBUTES)return 2;
        if(request.mode==Picker::Save){
            DWORD parent=GetFileAttributesW(Picker::Parent(path).c_str());
            if(parent==INVALID_FILE_ATTRIBUTES||!(parent&FILE_ATTRIBUTE_DIRECTORY))return 2;
        }
    }
    return 0;
}
}

// ================================================================ format policy
// Only reads an already initialized application's GStreamer registry; never
// loads plugins, starts GStreamer, or probes user files.
namespace PickerFormats {
inline std::wstring Trim(std::wstring s){
    auto first=s.find_first_not_of(L" \t");if(first==std::wstring::npos)return {};
    return s.substr(first,s.find_last_not_of(L" \t")-first+1);
}
inline bool Unrestricted(const std::wstring& pattern){
    if(Trim(pattern).empty())return true;
    size_t pos=0;
    while(pos<pattern.size()){
        size_t end=pattern.find(L';',pos);
        auto part=Trim(pattern.substr(pos,end==std::wstring::npos?end:end-pos));
        if(part==L"*"||part==L"*.*")return true;
        if(end==std::wstring::npos)break;pos=end+1;
    }
    return false;
}
inline bool ExtensionValid(const std::wstring& ext){
    if(ext.empty()||ext.size()>24)return false;
    for(wchar_t c:ext)if(!((c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z')||(c>=L'0'&&c<=L'9')))return false;
    return true;
}
inline std::wstring LowerExtension(std::wstring ext){
    while(!ext.empty()&&ext.front()==L'.')ext.erase(0,1);
    if(!ExtensionValid(ext))return {};
    CharLowerBuffW(ext.data(),static_cast<DWORD>(ext.size()));return ext;
}
inline std::wstring ExtensionFromName(const std::wstring& name){
    auto leaf=Picker::Leaf(name);auto dot=leaf.find_last_of(L'.');
    return dot==std::wstring::npos?L"":LowerExtension(leaf.substr(dot+1));
}
inline std::wstring Mask(const std::set<std::wstring>& extensions){
    std::wstring out;for(auto& ext:extensions){if(!out.empty())out+=L';';out+=L"*."+ext;}return out;
}
struct Plan {
    Picker::Request wire;
    // Wire entries may be pruned or synthesized, but the caller must receive
    // the original application's one-based index, not our display index.
    std::vector<DWORD> applicationIndices;
    const wchar_t* source=L"application";
};
inline bool UseApplicationFilters(const Picker::Request& original,Plan& plan){
    std::vector<Picker::Filter> restricted;std::vector<DWORD> indices;DWORD chosen=1;
    for(size_t i=0;i<original.filters.size();i++){
        const auto& f=original.filters[i];if(Unrestricted(f.pattern))continue;
        if(i+1==original.index)chosen=static_cast<DWORD>(restricted.size()+1);
        restricted.push_back(f);indices.push_back(static_cast<DWORD>(i+1));
    }
    if(restricted.empty())return false;
    plan.wire.filters=std::move(restricted);plan.applicationIndices=std::move(indices);
    plan.wire.index=chosen;plan.wire.flags|=Picker::StrictTypes;return true;
}
inline bool NtscExportFilter(const Picker::Request& original,Plan& plan){
    // ntsc-rs chooses its encoder in the main window, not in the file dialog.
    auto ext=ExtensionFromName(original.name);
    if(ext.empty())ext=LowerExtension(original.extension);
    const wchar_t* label=ext==L"mp4"?UiText(L"MPEG-4 Video",L"Видео MPEG-4"):ext==L"mkv"?UiText(L"Matroska Video",L"Видео Matroska"):ext==L"png"?UiText(L"PNG Image",L"Изображение PNG"):nullptr;
    if(!label)return false;
    plan.wire.filters={{label,L"*."+ext}};plan.wire.index=1;plan.wire.extension=ext;
    plan.wire.flags|=Picker::StrictTypes;
    plan.applicationIndices={original.index?original.index:1};plan.source=L"ntsc-rs output filename";
    return true;
}
// GLib's public GList layout; opaque GStreamer objects remain opaque.
struct GstList {void* data;GstList* next;GstList* prev;};
struct GstModule {
    HMODULE value=nullptr;
    ~GstModule(){if(value)FreeLibrary(value);}
    template<class T>T symbol(const char* name)const{return reinterpret_cast<T>(GetProcAddress(value,name));}
};
struct FeatureList {GstList* value;void (*freeList)(GstList*);~FeatureList(){if(value)freeList(value);}};
inline bool NtscInputFilters(Plan& plan){
    const DWORD applicationIndex=plan.wire.index?plan.wire.index:1;
    GstModule module;
    if(!GetModuleHandleExW(0,L"gstreamer-1.0-0.dll",&module.value)&&
       !GetModuleHandleExW(0,L"libgstreamer-1.0-0.dll",&module.value))return false;
    auto initialized=module.symbol<int(*)()>("gst_is_initialized");
    auto typeList=module.symbol<GstList*(*)()>("gst_type_find_factory_get_list");
    auto freeList=module.symbol<void(*)(GstList*)>("gst_plugin_feature_list_free");
    auto getExtensions=module.symbol<const char*const*(*)(void*)>("gst_type_find_factory_get_extensions");
    auto getCaps=module.symbol<void*(*)(void*)>("gst_type_find_factory_get_caps");
    auto capsSize=module.symbol<unsigned(*)(const void*)>("gst_caps_get_size");
    auto capsStructure=module.symbol<const void*(*)(const void*,unsigned)>("gst_caps_get_structure");
    auto structureName=module.symbol<const char*(*)(const void*)>("gst_structure_get_name");
    auto elements=module.symbol<GstList*(*)(unsigned long long,unsigned)>("gst_element_factory_list_get_elements");
    auto canSink=module.symbol<int(*)(void*,const void*)>("gst_element_factory_can_sink_any_caps");
    if(!initialized||!typeList||!freeList||!getExtensions||!getCaps||!capsSize||!capsStructure||
       !structureName||!elements||!canSink||!initialized())return false;
    // Public GstElementFactoryListType: DECODER (1<<0), DEMUXER (1<<5); rank MARGINAL (64).
    FeatureList types{typeList(),freeList},handlers{elements((1ull<<0)|(1ull<<5),64),freeList};
    if(!types.value||!handlers.value)return false;
    std::set<std::wstring> videos,images;
    const std::set<std::wstring> audioOnly={L"m4a",L"m4b",L"m4r",L"mka",L"weba",L"oga",L"spx",L"wma",L"3ga"};
    size_t visited=0;
    for(auto t=types.value;t&&visited++<4096;t=t->next){
        auto caps=getCaps(t->data);auto extensions=getExtensions(t->data);if(!caps||!extensions)continue;
        bool video=false,image=false;
        unsigned structures=std::min(capsSize(caps),256u);
        for(unsigned i=0;i<structures;i++){
            auto structure=capsStructure(caps,i);const char* mime=structure?structureName(structure):nullptr;if(!mime)continue;
            video|=!strncmp(mime,"video/",6)||!strcmp(mime,"application/ogg")||!strcmp(mime,"application/x-ogg")||
                !strcmp(mime,"application/vnd.rn-realmedia")||!strcmp(mime,"application/x-3gp");
            image|=!strncmp(mime,"image/",6);
        }
        if(!video&&!image)continue;
        bool supported=false;size_t examined=0;
        for(auto h=handlers.value;h&&examined++<4096;h=h->next)if(canSink(h->data,caps)){supported=true;break;}
        if(!supported)continue;
        for(size_t i=0;i<256&&extensions[i];i++){
            std::wstring ext;bool valid=true;
            for(size_t j=0;extensions[i][j];j++){
                unsigned char c=static_cast<unsigned char>(extensions[i][j]);
                if(j>=24||c>=128){valid=false;break;}ext.push_back(c);
            }
            ext=LowerExtension(ext);if(!valid||ext.empty()||audioOnly.count(ext))continue;
            if(video)videos.insert(ext);if(image)images.insert(ext);
        }
    }
    std::set<std::wstring> all=videos;all.insert(images.begin(),images.end());if(all.empty())return false;
    std::vector<Picker::Filter> filters={{UiText(L"Supported Videos and Images",L"Поддерживаемые видео и изображения"),Mask(all)}};
    if(!videos.empty()&&!images.empty()){filters.push_back({UiText(L"Video Files",L"Видеофайлы"),Mask(videos)});filters.push_back({UiText(L"Images",L"Изображения"),Mask(images)});}
    for(auto& ext:all){
        if(filters.size()>=Picker::MaxFilters)break;auto label=ext;CharUpperBuffW(label.data(),static_cast<DWORD>(label.size()));
        filters.push_back({label,L"*."+ext});
    }
    plan.wire.filters=std::move(filters);plan.wire.index=1;plan.wire.flags|=Picker::StrictTypes;
    plan.applicationIndices.assign(plan.wire.filters.size(),applicationIndex);plan.source=L"ntsc-rs loaded GStreamer registry";
    return true;
}
inline void Prepare(const Picker::Request& original,bool ntsc,Plan& plan){
    plan=Plan{};plan.wire=original;
    if(original.mode==Picker::Folder){plan.wire.filters.clear();plan.wire.index=1;plan.applicationIndices={1};plan.source=L"folder";return;}
    // ntsc-rs: the confirmed media policy (specific formats only, strict).
    // Everyone else keeps the application's own list, "All files" included,
    // and strict matching only when the application asked for it.
    if(ntsc){
        if(UseApplicationFilters(original,plan))return;
        if(original.mode==Picker::Save?NtscExportFilter(original,plan):NtscInputFilters(plan))return;
    }
    plan=Plan{};plan.wire=original;
    for(size_t i=0;i<std::max<size_t>(1,original.filters.size());i++)plan.applicationIndices.push_back(static_cast<DWORD>(i+1));
}
}

// ================================================================ bridge: shared
namespace Bridge {
bool demoProcess=false,ntscProcess=false,explorerProcess=false;
std::wstring lastOpenKey,lastSaveKey;

std::wstring Remembered(bool save){
    if(!Wh_GetIntSetting(L"rememberFolders")||demoProcess)return {};
    wchar_t value[32768]={};Wh_GetStringValue((save?lastSaveKey:lastOpenKey).c_str(),value,32768);
    DWORD attr=GetFileAttributesW(value);
    return attr!=INVALID_FILE_ATTRIBUTES&&(attr&FILE_ATTRIBUTE_DIRECTORY)?value:L"";
}
void Remember(bool save,const std::wstring& folder){
    if(!demoProcess&&!g_unloading&&!folder.empty())Wh_SetStringValue((save?lastSaveKey:lastOpenKey).c_str(),folder.c_str());
}
bool IsDirectory(const std::wstring& path){DWORD a=path.empty()?INVALID_FILE_ATTRIBUTES:GetFileAttributesW(path.c_str());return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY);}
std::wstring KnownFolder(REFKNOWNFOLDERID id){PWSTR p=nullptr;std::wstring out;if(SUCCEEDED(SHGetKnownFolderPath(id,0,nullptr,&p))){out=p;}CoTaskMemFree(p);return out;}

UINT TestMode(HWND owner){
    return demoProcess&&owner?static_cast<UINT>(reinterpret_cast<UINT_PTR>(GetPropW(owner,L"ExplorerPicker.BridgeSelfTest"))):0;
}
// Shortcut targets are returned unless the caller asked for links themselves.
void ResolveLinks(std::vector<std::wstring>& paths){
    HRESULT co=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
    struct Uninit{HRESULT hr;~Uninit(){if(SUCCEEDED(hr))CoUninitialize();}} uninit{co};
    for(auto& path:paths){
        if(_wcsicmp(PathFindExtensionW(path.c_str()),L".lnk"))continue;
        IShellLinkW* link=nullptr;IPersistFile* file=nullptr;
        if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&link)))){
            if(SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file)))&&SUCCEEDED(file->Load(path.c_str(),STGM_READ))){
                wchar_t target[32768]={};
                if(SUCCEEDED(link->GetPath(target,32768,nullptr,0))&&target[0]){
                    DWORD attr=GetFileAttributesW(target);
                    if(attr!=INVALID_FILE_ATTRIBUTES&&!(attr&FILE_ATTRIBUTE_DIRECTORY)){Trace(L"Shortcut resolved: %ls",target);path=target;}
                }
            }
            if(file)file->Release();link->Release();
        }
    }
}
// Applies the format policy, runs the picker and maps the type index back.
int RunPicker(HWND owner,const Picker::Request& request,Ctl::Result& result,Ctl::Target* events=nullptr){
    PickerFormats::Plan formats;PickerFormats::Prepare(request,ntscProcess,formats);
    Trace(L"%ls request, folder %ls, %zu filters (%ls)",request.mode==Picker::Save?L"Save":request.mode==Picker::Folder?L"Folder":
        request.mode==Picker::Multi?L"Multi-open":L"Open",formats.wire.folder.c_str(),formats.wire.filters.size(),formats.source);
    int status=Ctl::Run(owner,formats.wire,result,events,formats.applicationIndices,TestMode(owner));
    if(!status){
        if(!result.index||result.index>formats.applicationIndices.size())return 2;
        result.index=formats.applicationIndices[result.index-1];
    }
    return status;
}
}

// ================================================================ bridge: GetOpenFileName / GetSaveFileName
// Explorer-style OPENFILENAME customization, independent of application names.
// Keep the real native dialog alive: never fabricate a template or a hook HWND.
namespace PickerLegacy {
using Native=BOOL(WINAPI*)(LPOPENFILENAMEW);
constexpr DWORD ReadOnlyId=0x7FFF0001;
bool SizeValid(DWORD size){return size==sizeof(OPENFILENAMEW)||size==OPENFILENAME_SIZE_VERSION_400W;}
bool FlagsExValid(const OPENFILENAMEW* p){return p->lStructSize==OPENFILENAME_SIZE_VERSION_400W||!(p->FlagsEx&~OFN_EX_NOPLACESBAR);}
constexpr DWORD CommonFlags=OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_READONLY|
    OFN_NOCHANGEDIR|OFN_DONTADDTORECENT|OFN_ALLOWMULTISELECT|OFN_OVERWRITEPROMPT|OFN_NOREADONLYRETURN|OFN_NOTESTFILECREATE|
    OFN_ENABLESIZING|OFN_NONETWORKBUTTON|OFN_LONGNAMES|OFN_NODEREFERENCELINKS|OFN_SHAREAWARE|OFN_SHOWHELP|OFN_NOVALIDATE|
    OFN_FORCESHOWHIDDEN|OFN_EXTENSIONDIFFERENT|OFN_NOLONGNAMES;
inline bool Plain(const OPENFILENAMEW* p,bool save){
    return p&&SizeValid(p->lStructSize)&&p->lpstrFile&&p->nMaxFile>=2&&FlagsExValid(p)&&
        !(p->Flags&~CommonFlags)&&!p->lpstrCustomFilter&&
        !(p->Flags&(OFN_ENABLEHOOK|OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE))&&
        // Old-style multi-select returns space separated short names.
        (!(p->Flags&OFN_ALLOWMULTISELECT)||(!save&&(p->Flags&OFN_EXPLORER)));
}
inline bool Eligible(const OPENFILENAMEW* p,bool save){
    constexpr DWORD custom=OFN_ENABLEHOOK|OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE;
    return p&&SizeValid(p->lStructSize)&&p->lpstrFile&&p->nMaxFile>=2&&
        (p->Flags&OFN_EXPLORER)&&(p->Flags&OFN_HIDEREADONLY)&&(p->Flags&custom)&&
        !(p->Flags&~(CommonFlags|custom))&&FlagsExValid(p)&&!p->lpstrCustomFilter&&
        (!(p->Flags&OFN_ENABLEHOOK)||p->lpfnHook)&&
        (!(p->Flags&OFN_ENABLETEMPLATE)||p->lpTemplateName)&&
        (!(p->Flags&OFN_ENABLETEMPLATEHANDLE)||p->hInstance)&&
        !((p->Flags&OFN_ENABLETEMPLATE)&&(p->Flags&OFN_ENABLETEMPLATEHANDLE))&&
        !(save&&(p->Flags&OFN_ALLOWMULTISELECT));
}
inline bool ReadFilters(const wchar_t* p,std::vector<Picker::Filter>& out){
    out.clear();if(!p)return true;
    while(*p){
        if(out.size()>=Picker::MaxFilters)return false;
        std::wstring label=p;p+=label.size()+1;std::wstring pattern=p;p+=pattern.size()+1;
        if(pattern.empty())return false;out.push_back({label,pattern});
    }
    return true;
}
// Initial folder: lpstrInitialDir, then a path in lpstrFile, then the
// application's last folder, then the current directory, then Documents.
void InitialFolder(const OPENFILENAMEW* ofn,bool save,Picker::Request& q){
    if(ofn->lpstrInitialDir&&Bridge::IsDirectory(ofn->lpstrInitialDir))q.folder=ofn->lpstrInitialDir;
    auto slash=q.name.find_last_of(L"\\/");
    if(slash!=std::wstring::npos&&q.name.find(L'"')==std::wstring::npos){
        auto parent=Picker::Parent(q.name);
        if(Bridge::IsDirectory(parent)){if(q.folder.empty()||(parent.size()>2&&parent[1]==L':'))q.folder=parent;q.name=Picker::Leaf(q.name);}
    }
    if(q.folder.empty())q.folder=Bridge::Remembered(save);
    if(q.folder.empty()){wchar_t cwd[32768];DWORD n=GetCurrentDirectoryW(32768,cwd);if(n&&n<32768&&Bridge::IsDirectory(cwd))q.folder=cwd;}
    if(q.folder.empty())q.folder=Bridge::KnownFolder(FOLDERID_Documents);
}
inline std::wstring Text(HWND h){
    int count=GetWindowTextLengthW(h);if(count<0||count>32767)return {};
    std::wstring text(static_cast<size_t>(count)+1,0);text.resize(GetWindowTextW(h,text.data(),count+1));return text;
}
// IsWindowVisible cannot be used: the native root is intentionally hidden.
inline DWORD ControlState(HWND h,HWND root){
    DWORD state=3;
    for(HWND at=h;at&&at!=root;at=GetParent(at)){
        LONG_PTR style=GetWindowLongPtrW(at,GWL_STYLE);
        if(!(style&WS_VISIBLE))state&=~2u;if(style&WS_DISABLED)state&=~1u;
    }return state;
}
// Mirrors the application's template child dialog: checkboxes, radio groups,
// push buttons, edits, drop-down lists, texts, group boxes and separators.
// Every picker control remembers the native HWNDs it stands for, so changes
// reach the application as the same WM_COMMAND notifications.
struct Controls {
    struct Native {HWND hwnd;DWORD control;DWORD item;};
    HWND root=nullptr,custom=nullptr;
    std::vector<HWND> handles;std::vector<Native> natives;std::vector<Picker::Control> checks;
    std::vector<std::pair<RECT,DWORD>> groups;
    bool valid=true;
    static constexpr DWORD RadioBase=0x10000;
    Picker::Control* Find(DWORD id){for(auto& c:checks)if(c.id==id)return &c;return nullptr;}
    void Add(HWND h,Picker::Control c,DWORD item=Picker::None){
        if(checks.size()>=Picker::MaxControls){valid=false;return;}
        natives.push_back({h,c.id,item});checks.push_back(std::move(c));
    }
    static BOOL CALLBACK Visit(HWND h,LPARAM l){
        auto& self=*reinterpret_cast<Controls*>(l);
        if(GetParent(h)!=self.custom||!self.valid)return self.valid; // e.g. a combo box's own edit
        wchar_t cls[64]={};GetClassNameW(h,cls,64);
        int id=GetDlgCtrlID(h);LONG_PTR style=GetWindowLongPtrW(h,GWL_STYLE);
        DWORD state=ControlState(h,self.root);
        // stc32 is the documented placement placeholder, not an application label.
        if(!_wcsicmp(cls,L"STATIC")&&id==stc32)return TRUE;
        self.handles.push_back(h);
        if(id<=0||id>65535){
            // Unnumbered statics are decoration or labels; anything else cannot be addressed.
            if(_wcsicmp(cls,L"STATIC")){self.valid=false;return FALSE;}
            id=0xF000+static_cast<int>(self.handles.size());
        }
        for(auto& c:self.checks)if(c.id==static_cast<DWORD>(id)){self.valid=false;return FALSE;}
        Picker::Control c;c.id=static_cast<DWORD>(id);c.state=state;
        if(!_wcsicmp(cls,L"BUTTON")){
            DWORD type=static_cast<DWORD>(style&BS_TYPEMASK);
            if(type==BS_CHECKBOX||type==BS_AUTOCHECKBOX){
                LRESULT checked=SendMessageW(h,BM_GETCHECK,0,0);
                if(checked!=BST_CHECKED&&checked!=BST_UNCHECKED){self.valid=false;return FALSE;}
                c.kind=Picker::Check;c.value=checked==BST_CHECKED;c.label=Text(h);self.Add(h,c);
            }else if(type==BS_RADIOBUTTON||type==BS_AUTORADIOBUTTON){
                // A radio group starts at WS_GROUP or after any other control.
                Picker::Control* group=nullptr;
                if(!(style&WS_GROUP)&&!self.natives.empty()&&self.natives.back().item!=Picker::None)group=self.Find(self.natives.back().control);
                if(!group){
                    Picker::Control r;r.id=RadioBase|static_cast<DWORD>(id);r.kind=Picker::Radio;r.state=3;
                    if(self.Find(r.id)){self.valid=false;return FALSE;}
                    self.checks.push_back(r);group=&self.checks.back();
                }
                if(group->items.size()>=Picker::MaxItems){self.valid=false;return FALSE;}
                group->items.push_back({static_cast<DWORD>(id),state,Text(h)});
                if(SendMessageW(h,BM_GETCHECK,0,0)==BST_CHECKED)group->value=static_cast<DWORD>(id);
                self.natives.push_back({h,group->id,static_cast<DWORD>(id)});
            }else if(type==BS_PUSHBUTTON||type==BS_DEFPUSHBUTTON){c.kind=Picker::Push;c.label=Text(h);self.Add(h,c);}
            else if(type==BS_GROUPBOX){
                c.kind=Picker::Group;c.label=Text(h);RECT r;GetWindowRect(h,&r);self.groups.push_back({r,c.id});self.Add(h,c);
            }else{self.valid=false;return FALSE;}
        }else if(!_wcsicmp(cls,L"STATIC")){
            DWORD type=static_cast<DWORD>(style&SS_TYPEMASK);
            if(type==SS_ETCHEDHORZ){c.kind=Picker::Separator;self.Add(h,c);}
            else if(type<=SS_RIGHT||type==SS_LEFTNOWORDWRAP||type==SS_SIMPLE){
                c.kind=Picker::Text;c.label=Text(h);if(!c.label.empty())self.Add(h,c);
            } // icons, frames and bitmaps are decoration
        }else if(!_wcsicmp(cls,L"EDIT")){
            c.kind=Picker::Edit;c.text=Text(h);self.Add(h,c);
        }else if(!_wcsicmp(cls,L"COMBOBOX")){
            if((style&3)!=CBS_DROPDOWNLIST){c.kind=Picker::Edit;c.text=Text(h);self.Add(h,c);return TRUE;}
            c.kind=Picker::Combo;LRESULT count=SendMessageW(h,CB_GETCOUNT,0,0);
            if(count<0||count>static_cast<LRESULT>(Picker::MaxItems)){self.valid=false;return FALSE;}
            for(LRESULT i=0;i<count;i++){
                LRESULT n=SendMessageW(h,CB_GETLBTEXTLEN,i,0);std::wstring t(n>0&&n<4096?n+1:1,L'\0');
                if(n>0&&n<4096){SendMessageW(h,CB_GETLBTEXT,i,reinterpret_cast<LPARAM>(t.data()));t.resize(n);}else t.clear();
                c.items.push_back({static_cast<DWORD>(i),3,t});
            }
            LRESULT sel=SendMessageW(h,CB_GETCURSEL,0,0);c.value=sel>=0?static_cast<DWORD>(sel):Picker::None;self.Add(h,c);
        }else{self.valid=false;return FALSE;}
        return self.valid;
    }
    bool Read(HWND parent,HWND child){
        root=parent;custom=child;handles.clear();natives.clear();checks.clear();groups.clear();valid=IsWindow(parent)&&IsWindow(child);
        if(valid)EnumChildWindows(child,Visit,reinterpret_cast<LPARAM>(this));
        if(!valid)return false;
        // Controls drawn inside a group box belong to it.
        for(auto& n:natives){
            auto c=Find(n.control);if(!c||c->kind==Picker::Group||c->parent)continue;
            RECT r;GetWindowRect(n.hwnd,&r);
            for(auto& g:groups)if(r.left>=g.first.left&&r.right<=g.first.right&&r.top>=g.first.top&&r.bottom<=g.first.bottom){c->parent=g.second;break;}
        }
        return true;
    }
    bool SameIdentity(const Controls& other)const{return handles==other.handles;}
    HWND Handle(DWORD control,DWORD item=Picker::None)const{
        for(auto& n:natives)if(n.control==control&&(item==Picker::None||n.item==item))return n.hwnd;return nullptr;
    }
};
class Session final:public Ctl::Target {
    inline static thread_local Session* pending=nullptr;
    inline static constexpr wchar_t Property[]=L"ExplorerPicker.Legacy.Custom.V6";
    OPENFILENAMEW* ofn;bool save;
    LPOFNHOOKPROC applicationHook;DWORD originalFlags;
    HWND native=nullptr,child=nullptr;HHOOK activationHook=nullptr;
    Picker::Request request;Controls controls;std::wstring folder;
    std::vector<std::wstring> paths;
    bool suppress=true,started=false,active=false,fallback=false,confirming=false,approved=false,notifying=false,sendingNotice=false;
    DWORD resultError=0;
    DWORD finalIndex=1,finalFlags=0;WORD finalOffset=0,finalExtension=0;
    UINT startMessage=0;
    void Revert(){fallback=true;if(controller)PostMessageW(controller,WM_CLOSE,0,0);}
    static LRESULT CopyText(const std::wstring& text,WPARAM capacity,LPARAM buffer){
        if(buffer&&capacity){size_t n=(std::min)(static_cast<size_t>(capacity)-1,text.size());
            memcpy(reinterpret_cast<void*>(buffer),text.data(),n*sizeof(wchar_t));reinterpret_cast<wchar_t*>(buffer)[n]=0;}
        return static_cast<LRESULT>(text.size()+1);
    }
    std::wstring Names(bool full)const{
        if(paths.empty())return {};
        if(paths.size()==1)return full?paths[0]:Picker::Leaf(paths[0]);
        std::wstring value;for(auto& p:paths){if(!value.empty())value+=L" ";value+=L"\""+(full?p:Picker::Leaf(p))+L"\"";}return value;
    }
    bool Refresh(){
        Controls next;if(!next.Read(native,child)||!controls.SameIdentity(next)){Revert();return false;}
        controls=std::move(next);request.controls=controls.checks;request.okLabel=Text(GetDlgItem(native,IDOK));
        request.nameLabel=Text(GetDlgItem(native,stc3));return true;
    }
    void Notice(UINT code){
        OFNOTIFYW note={};note.hdr.hwndFrom=native;note.hdr.code=code;note.lpOFN=ofn;
        std::wstring file=Names(true);note.pszFile=file.data();
        sendingNotice=true;SendMessageW(child,WM_NOTIFY,0,reinterpret_cast<LPARAM>(&note));sendingNotice=false;
    }
    bool WriteResult(DWORD index){
        std::wstring value=paths[0];size_t offset=value.size()-Picker::Leaf(value).size();
        size_t dot=value.find_last_of(L'.');size_t extension=dot!=std::wstring::npos&&dot>=offset?dot+1:0;
        if(paths.size()>1){
            value=Picker::Parent(paths[0]);value.push_back(0);offset=value.size();extension=0;
            for(auto& path:paths){value+=Picker::Leaf(path);value.push_back(0);}
        }
        value.push_back(0);
        if(value.size()>ofn->nMaxFile||offset>65535||extension>65535){
            resultError=FNERR_BUFFERTOOSMALL;WORD required=static_cast<WORD>((std::min)(value.size(),size_t(65535)));
            memcpy(ofn->lpstrFile,&required,sizeof(required));return false;
        }
        memcpy(ofn->lpstrFile,value.data(),value.size()*sizeof(wchar_t));
        ofn->nFileOffset=static_cast<WORD>(offset);ofn->nFileExtension=static_cast<WORD>(extension);ofn->nFilterIndex=index;
        ofn->Flags&=~OFN_EXTENSIONDIFFERENT;
        if(paths.size()==1&&!request.extension.empty()&&
            _wcsicmp(extension?paths[0].c_str()+extension:L"",request.extension.c_str()))ofn->Flags|=OFN_EXTENSIONDIFFERENT;
        if(ofn->lpstrFileTitle&&ofn->nMaxFileTitle){auto leaf=Picker::Leaf(paths[0]);
            size_t n=(std::min)(leaf.size(),static_cast<size_t>(ofn->nMaxFileTitle-1));
            memcpy(ofn->lpstrFileTitle,leaf.data(),n*sizeof(wchar_t));ofn->lpstrFileTitle[n]=0;}
        return true;
    }
    bool Prepare(){
        size_t length=wcsnlen(ofn->lpstrFile,ofn->nMaxFile);if(length>=ofn->nMaxFile)return false;
        request.mode=save?Picker::Save:(ofn->Flags&OFN_ALLOWMULTISELECT)?Picker::Multi:Picker::OpenFile;
        request.flags=Picker::SameDirectory;request.name.assign(ofn->lpstrFile,length);
        if(ofn->lpstrTitle)request.title=ofn->lpstrTitle;
        if(ofn->lpstrDefExt)request.extension=ofn->lpstrDefExt;
        wchar_t current[32768]={};
        LRESULT n=SendMessageW(native,CDM_GETFOLDERPATH,32768,reinterpret_cast<LPARAM>(current));
        if(n>0&&n<=32768&&Bridge::IsDirectory(current))request.folder=current;
        else InitialFolder(ofn,save,request);
        folder=request.folder;
        if(!ReadFilters(ofn->lpstrFilter,request.filters))return false;
        request.index=ofn->nFilterIndex?ofn->nFilterIndex:1;
        if(request.index>(std::max)(size_t(1),request.filters.size()))return false;
        if(!controls.Read(native,child))return false;
        request.controls=controls.checks;request.okLabel=Text(GetDlgItem(native,IDOK));request.nameLabel=Text(GetDlgItem(native,stc3));return true;
    }
    void Reveal(){
        bool hadPicker=active;active=false;
        if(hadPicker){
            HWND types=GetDlgItem(native,cmb1);
            if(types&&SendMessageW(types,CB_GETCURSEL,0,0)!=static_cast<LRESULT>(request.index-1)){
                SendMessageW(types,CB_SETCURSEL,request.index-1,0);
                SendMessageW(native,WM_COMMAND,MAKEWPARAM(cmb1,CBN_SELCHANGE),reinterpret_cast<LPARAM>(types));
            }
            if(!paths.empty()){auto text=Names(true);SendMessageW(native,CDM_SETCONTROLTEXT,edt1,reinterpret_cast<LPARAM>(text.c_str()));}
        }
        suppress=false;Trace(L"Legacy custom: original dialog shown");
        ShowWindow(native,SW_SHOW);SetForegroundWindow(native);
    }
    void Start(){
        if(started)return;started=true;
        if(!Prepare()){Reveal();return;}
        active=true;Trace(L"Legacy custom: hook/template retained, %zu controls mirrored",request.controls.size());
        Ctl::Result result;int status=Bridge::RunPicker(ofn->hwndOwner,request,result,this);
        // EndDialog's modal loop unwinds only after this message returns.
        if(approved){active=false;return;}
        if(resultError){active=false;EndDialog(native,FALSE);return;}
        if(fallback||status!=1){Reveal();return;}
        active=false;
        SendMessageW(native,WM_COMMAND,IDCANCEL,0);
    }
    static LRESULT CALLBACK Activation(int code,WPARAM w,LPARAM l){
        auto self=pending;
        if(code==HCBT_ACTIVATE&&self&&self->suppress&&reinterpret_cast<HWND>(w)==self->native)return 1;
        return CallNextHookEx(nullptr,code,w,l);
    }
    static LRESULT CALLBACK Parent(HWND h,UINT m,WPARAM w,LPARAM l,UINT_PTR,DWORD_PTR data){
        auto self=reinterpret_cast<Session*>(data);
        if(m==self->startMessage){self->Start();return 0;}
        if(m==WM_WINDOWPOSCHANGING&&self->suppress){
            auto pos=reinterpret_cast<WINDOWPOS*>(l);pos->flags&=~SWP_SHOWWINDOW;pos->flags|=SWP_NOACTIVATE;
        }
        if(self->active){
            if(m==CDM_GETFOLDERPATH)return CopyText(self->folder,w,l);
            if(m==CDM_GETSPEC)return CopyText(self->Names(false),w,l);
            if(m==CDM_GETFILEPATH)return CopyText(self->Names(true),w,l);
            if(m==CDM_GETFOLDERIDLIST){
                auto pidl=ILCreateFromPathW(self->folder.c_str());if(!pidl)return -1;
                UINT bytes=ILGetSize(pidl);if(l&&w>=bytes)memcpy(reinterpret_cast<void*>(l),pidl,bytes);CoTaskMemFree(pidl);return bytes;
            }
            // Labels/default extension can be mirrored. Changes to editable
            // native controls require the original UI, not a partial imitation.
            if(!self->confirming&&m==CDM_SETCONTROLTEXT&&w!=IDOK&&w!=stc3)self->Revert();
            if(m==CDM_HIDECONTROL)self->Revert();
            if(m==CDM_SETDEFEXT&&l)self->request.extension=reinterpret_cast<LPCWSTR>(l);
            if(m==WM_COMMAND&&LOWORD(w)==IDCANCEL&&self->controller)PostMessageW(self->controller,WM_CLOSE,0,0);
        }
        if(m==WM_NCDESTROY){RemoveWindowSubclass(h,Parent,1);self->native=nullptr;if(self->controller)PostMessageW(self->controller,WM_CLOSE,0,0);}
        return DefSubclassProc(h,m,w,l);
    }
    static UINT_PTR CALLBACK Hook(HWND h,UINT m,WPARAM w,LPARAM l){
        auto self=reinterpret_cast<Session*>(GetPropW(h,Property));
        if(!self&&pending&&pending->child==h)self=pending;
        if(m==WM_INITDIALOG&&pending&&reinterpret_cast<OPENFILENAMEW*>(l)==pending->ofn){
            self=pending;self->child=h;self->native=GetParent(h);
            if(!SetPropW(h,Property,self)||!SetWindowSubclass(self->native,Parent,1,reinterpret_cast<DWORD_PTR>(self))){
                self->suppress=false;self->fallback=true;
            }
        }
        if(!self)return 0;
        if(self->active&&!self->sendingNotice&&m==WM_NOTIFY&&l){
            // Background initialization/navigation of the hidden native list must
            // not duplicate or replace events for the visible Explorer selection.
            UINT code=reinterpret_cast<OFNOTIFYW*>(l)->hdr.code;
            if(code==CDN_FOLDERCHANGE||code==CDN_SELCHANGE||code==CDN_TYPECHANGE)return 0;
        }
        UINT_PTR result=self->applicationHook?self->applicationHook(h,m,w,l):0;
        if(m==WM_NOTIFY&&l){auto note=reinterpret_cast<OFNOTIFYW*>(l);
            if(note->hdr.code==CDN_INITDONE&&!self->fallback&&!PostMessageW(self->native,self->startMessage,0,0)){
                self->fallback=true;self->suppress=false;
            }
            if(note->hdr.code==CDN_FILEOK&&self->confirming){
                self->approved=!(result&&GetWindowLongPtrW(h,DWLP_MSGRESULT));
                Trace(self->approved?L"Legacy custom: FileOk approved":L"Legacy custom: application veto retained");
            }
        }
        if(m==WM_NCDESTROY){RemovePropW(h,Property);self->child=nullptr;}
        return result;
    }
public:
    Session(OPENFILENAMEW* p,bool isSave):ofn(p),save(isSave),
        applicationHook((p->Flags&OFN_ENABLEHOOK)?p->lpfnHook:nullptr),originalFlags(p->Flags){}
    const Picker::Request* UiState()const override{return &request;}
    DWORD Error()const{return resultError;}
    HRESULT Notify(Ctl::Event& e)override{
        if(notifying||!active||fallback||!IsWindow(native)||!IsWindow(child))return E_UNEXPECTED;
        notifying=true;struct Reset{bool& b;~Reset(){b=false;}} reset{notifying};
        if(!Refresh())return E_FAIL;
        if(e.kind==Ctl::ControlEvent){
            auto c=controls.Find(e.control);if(!c)return E_INVALIDARG;
            if((c->state&3)!=3)return E_ACCESSDENIED;
            auto command=[&](HWND h,WORD code){SendMessageW(GetParent(h),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(h),code),reinterpret_cast<LPARAM>(h));};
            // The same native HWND/id and lCustData as a real click or edit.
            switch(c->kind){
            case Picker::Check:{
                if(e.action!=0||e.value>1)return E_INVALIDARG;if(e.value==c->value)return S_OK;
                HWND button=controls.Handle(c->id);
                // Manual checkboxes let the hook set their state; auto ones toggle first.
                if((GetWindowLongPtrW(button,GWL_STYLE)&BS_TYPEMASK)==BS_AUTOCHECKBOX)
                    SendMessageW(button,BM_SETCHECK,e.value?BST_CHECKED:BST_UNCHECKED,0);
                command(button,BN_CLICKED);break;}
            case Picker::Radio:{
                HWND chosen=controls.Handle(c->id,e.value);if(!chosen)return E_INVALIDARG;
                for(auto& item:c->items)if(HWND h=controls.Handle(c->id,item.id))
                    if((GetWindowLongPtrW(h,GWL_STYLE)&BS_TYPEMASK)==BS_AUTORADIOBUTTON)SendMessageW(h,BM_SETCHECK,h==chosen?BST_CHECKED:BST_UNCHECKED,0);
                command(chosen,BN_CLICKED);break;}
            case Picker::Push:if(e.action!=1)return E_INVALIDARG;command(controls.Handle(c->id),BN_CLICKED);break;
            case Picker::Edit:{HWND h=controls.Handle(c->id);SetWindowTextW(h,e.text.c_str());
                wchar_t cls[32]={};GetClassNameW(h,cls,32);command(h,_wcsicmp(cls,L"COMBOBOX")?EN_CHANGE:CBN_EDITCHANGE);break;}
            case Picker::Combo:{HWND h=controls.Handle(c->id);if(!c->FindItem(e.value))return E_INVALIDARG;
                SendMessageW(h,CB_SETCURSEL,e.value,0);command(h,CBN_SELCHANGE);break;}
            default:return E_INVALIDARG;
            }
            return Refresh()?S_OK:E_FAIL;
        }
        if(e.kind==Ctl::RefreshEvent)return S_OK;
        if(!e.index||e.index>(std::max)(size_t(1),request.filters.size()))return E_INVALIDARG;
        bool folderChanged=folder!=e.folder,selectionChanged=paths!=e.paths,typeChanged=request.index!=e.index;
        folder=e.folder;paths=e.paths;request.index=e.index;ofn->nFilterIndex=e.index;
        if(e.kind!=Ctl::FileOkEvent){
            // Selection notices describe file system items only.
            paths.erase(std::remove(paths.begin(),paths.end(),std::wstring()),paths.end());
        }
        if(folderChanged)Notice(CDN_FOLDERCHANGE);
        if(typeChanged)Notice(CDN_TYPECHANGE);
        if(selectionChanged)Notice(CDN_SELCHANGE);
        if(!Refresh()||fallback)return E_FAIL;
        if(e.kind!=Ctl::FileOkEvent)return S_OK;
        if(paths.empty()||(request.mode!=Picker::Multi&&paths.size()!=1))return E_INVALIDARG;
        for(auto& path:paths)if(path.empty()||path.find(L'"')!=std::wstring::npos||
            _wcsicmp(Picker::Parent(path).c_str(),Picker::Parent(paths[0]).c_str()))return E_INVALIDARG;
        if(ofn->Flags&OFN_NOREADONLYRETURN)for(auto& path:paths){DWORD attr=GetFileAttributesW(path.c_str());
            if(attr!=INVALID_FILE_ATTRIBUTES&&(attr&FILE_ATTRIBUTE_READONLY))return S_FALSE;}
        // Populate the documented result BEFORE CDN_FILEOK: application hooks
        // read it inside that callback and may veto without closing either UI.
        if(!WriteResult(e.index)){if(controller)PostMessageW(controller,WM_CLOSE,0,0);return E_ABORT;}
        approved=false;confirming=true;Notice(CDN_FILEOK);confirming=false;
        if(fallback){approved=false;return E_FAIL;}
        if(!approved){Refresh();return S_FALSE;}
        finalIndex=ofn->nFilterIndex;finalFlags=ofn->Flags;finalOffset=ofn->nFileOffset;finalExtension=ofn->nFileExtension;
        EndDialog(native,TRUE);return S_OK;
    }
    BOOL Run(Native original){
        startMessage=RegisterWindowMessageW(L"ExplorerPicker.Legacy.Start.V6");
        if(!startMessage)return original(ofn);
        Session* previous=pending;pending=this;
        activationHook=SetWindowsHookExW(WH_CBT,Activation,nullptr,GetCurrentThreadId());
        if(!activationHook){pending=previous;return original(ofn);}
        auto oldHook=ofn->lpfnHook;ofn->lpfnHook=Hook;ofn->Flags|=OFN_ENABLEHOOK;
        BOOL result=original(ofn);
        // The native wrapper writes its cached filter index on exit. Our visible
        // picker owns that selection; restore the values approved by the hook.
        if(result&&approved){ofn->nFilterIndex=finalIndex;ofn->Flags=finalFlags;ofn->nFileOffset=finalOffset;ofn->nFileExtension=finalExtension;}
        ofn->lpfnHook=oldHook;ofn->Flags=(ofn->Flags&~OFN_ENABLEHOOK)|(originalFlags&OFN_ENABLEHOOK);
        if(IsWindow(native))RemoveWindowSubclass(native,Parent,1);
        if(IsWindow(child))RemovePropW(child,Property);
        UnhookWindowsHookEx(activationHook);pending=previous;return result;
    }
};

// Plain dialogs: the whole contract is the structure itself.
struct PlainOutcome {int status=2;Ctl::Result result;bool readOnly=false;};
PlainOutcome RunPlain(const OPENFILENAMEW* ofn,bool save){
    PlainOutcome out;Picker::Request q;
    q.mode=save?Picker::Save:(ofn->Flags&OFN_ALLOWMULTISELECT)?Picker::Multi:Picker::OpenFile;q.flags=Picker::SameDirectory;
    size_t len=wcsnlen(ofn->lpstrFile,ofn->nMaxFile);if(len>=ofn->nMaxFile)return out;
    q.name.assign(ofn->lpstrFile,len);
    if(q.mode==Picker::Multi&&q.name.find(L'"')==std::wstring::npos&&len+1<ofn->nMaxFile&&ofn->lpstrFile[len+1])q.name.clear();
    if(ofn->lpstrDefExt)q.extension=ofn->lpstrDefExt;if(ofn->lpstrTitle)q.title=ofn->lpstrTitle;
    if(!ReadFilters(ofn->lpstrFilter,q.filters))return out;
    q.index=ofn->nFilterIndex?ofn->nFilterIndex:1;if(q.index>std::max<size_t>(1,q.filters.size()))q.index=1;
    InitialFolder(ofn,save,q);
    if(!save&&!(ofn->Flags&OFN_HIDEREADONLY)){
        Picker::Control c;c.id=ReadOnlyId;c.kind=Picker::Check;c.label=UiText(L"Open as read-only",L"Только для чтения");c.value=(ofn->Flags&OFN_READONLY)?1:0;
        q.controls.push_back(c);
    }
    // The read-only box is ours; mirror its final state without an application callback.
    struct ReadOnlyBox final:Ctl::Target {
        Picker::Request ui;bool checked=false;
        HRESULT Notify(Ctl::Event& e)override{if(e.kind==Ctl::ControlEvent&&e.control==ReadOnlyId){checked=e.value==1;if(!ui.controls.empty())ui.controls[0].value=e.value;}return S_OK;}
        const Picker::Request* UiState()const override{return &ui;}
    } box;
    box.ui=q;box.checked=(ofn->Flags&OFN_READONLY)!=0;
    out.status=Bridge::RunPicker(ofn->hwndOwner,q,out.result,q.controls.empty()?nullptr:&box);
    out.readOnly=box.checked;
    if(out.status==0&&!(ofn->Flags&OFN_NODEREFERENCELINKS)&&!save)Bridge::ResolveLinks(out.result.paths);
    if(out.status==0){
        Bridge::Remember(save,Picker::Parent(out.result.paths[0]));
        if(!(ofn->Flags&OFN_NOCHANGEDIR))SetCurrentDirectoryW(Picker::Parent(out.result.paths[0]).c_str());
    }
    return out;
}
// Result string in the documented layout: full path, or directory followed by names.
std::wstring ResultString(const std::vector<std::wstring>& paths,size_t& offset,size_t& extension){
    std::wstring output=paths[0];offset=output.size()-Picker::Leaf(output).size();
    size_t dot=output.find_last_of(L'.');extension=dot!=std::wstring::npos&&dot>=offset?dot+1:0;
    if(paths.size()>1){
        output=Picker::Parent(paths[0]);output.push_back(0);offset=output.size();extension=0;
        for(auto& p:paths){output+=Picker::Leaf(p);output.push_back(0);}
    }
    output.push_back(0);return output;
}
}

namespace Bridge {
using OpenFnW=BOOL(WINAPI*)(LPOPENFILENAMEW);
using OpenFnA=BOOL(WINAPI*)(LPOPENFILENAMEA);
using ErrorFn=DWORD(WINAPI*)();
OpenFnW originalOpenW=nullptr,originalSaveW=nullptr;
OpenFnA originalOpenA=nullptr,originalSaveA=nullptr;
ErrorFn originalError=nullptr;
thread_local bool overrideError=false;
thread_local DWORD pickerError=0;
DWORD WINAPI ErrorHook(){return overrideError?pickerError:originalError();}

BOOL Wide(LPOPENFILENAMEW ofn,bool save){
    auto original=save?originalSaveW:originalOpenW;
    if(g_unloading||Ctl::inPicker){overrideError=false;return original(ofn);}
    if(PickerLegacy::Eligible(ofn,save)){
        Ctl::PickerGuard guard;overrideError=false;
        PickerLegacy::Session session(ofn,save);
        BOOL result=session.Run(original);
        if(session.Error()){overrideError=true;pickerError=session.Error();}
        return result;
    }
    if(!PickerLegacy::Plain(ofn,save)){
        if(ofn)Trace(L"GetOpen/SaveFileNameW with unsupported flags 0x%08lX -> native",ofn->Flags);
        overrideError=false;return original(ofn);
    }
    PickerLegacy::PlainOutcome out;{Ctl::PickerGuard guard;out=PickerLegacy::RunPlain(ofn,save);}
    if(out.status==2){Trace(L"Picker unavailable -> native dialog");overrideError=false;return original(ofn);}
    overrideError=true;pickerError=0;if(out.status)return FALSE;
    size_t offset=0,extension=0;auto output=PickerLegacy::ResultString(out.result.paths,offset,extension);
    if(output.size()>ofn->nMaxFile){
        pickerError=FNERR_BUFFERTOOSMALL;WORD needed=static_cast<WORD>(std::min<size_t>(output.size(),65535));
        memcpy(ofn->lpstrFile,&needed,sizeof(needed));return FALSE;
    }
    memcpy(ofn->lpstrFile,output.data(),output.size()*sizeof(wchar_t));
    ofn->nFileOffset=static_cast<WORD>(offset);ofn->nFileExtension=static_cast<WORD>(extension);ofn->nFilterIndex=out.result.index;
    if(!save&&!(ofn->Flags&OFN_HIDEREADONLY)){if(out.readOnly)ofn->Flags|=OFN_READONLY;else ofn->Flags&=~OFN_READONLY;}
    ofn->Flags&=~OFN_EXTENSIONDIFFERENT;
    if(ofn->lpstrDefExt&&out.result.paths.size()==1&&_wcsicmp(extension?output.c_str()+extension:L"",ofn->lpstrDefExt))ofn->Flags|=OFN_EXTENSIONDIFFERENT;
    if(ofn->lpstrFileTitle&&ofn->nMaxFileTitle){auto leaf=Picker::Leaf(out.result.paths[0]);
        size_t count=std::min<size_t>(leaf.size(),ofn->nMaxFileTitle-1);memcpy(ofn->lpstrFileTitle,leaf.data(),count*2);ofn->lpstrFileTitle[count]=0;}
    return TRUE;
}
BOOL WINAPI OpenHookW(LPOPENFILENAMEW ofn){return Wide(ofn,false);}
BOOL WINAPI SaveHookW(LPOPENFILENAMEW ofn){return Wide(ofn,true);}

// ANSI callers: convert the plain structure, run the same picker, convert back
// in the caller's code page. Anything else keeps the ANSI native dialog.
std::wstring Widen(const char* s,int bytes=-1){
    if(!s)return {};int n=MultiByteToWideChar(CP_ACP,0,s,bytes,nullptr,0);if(n<=0)return {};
    std::wstring out(n,L'\0');MultiByteToWideChar(CP_ACP,0,s,bytes,out.data(),n);
    if(bytes==-1&&!out.empty())out.pop_back();return out;
}
std::string Narrow(const std::wstring& s){
    int n=WideCharToMultiByte(CP_ACP,0,s.data(),static_cast<int>(s.size()),nullptr,0,nullptr,nullptr);
    std::string out(n>0?n:0,'\0');if(n>0)WideCharToMultiByte(CP_ACP,0,s.data(),static_cast<int>(s.size()),out.data(),n,nullptr,nullptr);return out;
}
bool Representable(const std::wstring& s){
    BOOL lost=FALSE;WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,s.data(),static_cast<int>(s.size()),nullptr,0,nullptr,&lost);return !lost;
}
BOOL Ansi(LPOPENFILENAMEA a,bool save){
    auto original=save?originalSaveA:originalOpenA;
    if(g_unloading||Ctl::inPicker||!a||!Wh_GetIntSetting(L"ansiApps")||
       (a->lStructSize!=sizeof(OPENFILENAMEA)&&a->lStructSize!=OPENFILENAME_SIZE_VERSION_400A)||!a->lpstrFile||a->nMaxFile<2||
       (a->lStructSize==sizeof(OPENFILENAMEA)&&(a->FlagsEx&~OFN_EX_NOPLACESBAR))||(a->Flags&~PickerLegacy::CommonFlags)||a->lpstrCustomFilter||
       ((a->Flags&OFN_ALLOWMULTISELECT)&&(save||!(a->Flags&OFN_EXPLORER)))){
        overrideError=false;return original(a);
    }
    size_t len=strnlen(a->lpstrFile,a->nMaxFile);if(len>=a->nMaxFile){overrideError=false;return original(a);}
    std::wstring filter;
    if(a->lpstrFilter){const char* p=a->lpstrFilter;while(*p){size_t n=strlen(p);filter+=Widen(p);filter.push_back(0);p+=n+1;
        if(!*p){overrideError=false;return original(a);}n=strlen(p);filter+=Widen(p);filter.push_back(0);p+=n+1;}filter.push_back(0);}
    std::wstring file=Widen(a->lpstrFile);file.resize(std::max<size_t>(file.size()+1,32768),0);
    std::wstring title=Widen(a->lpstrTitle),dir=Widen(a->lpstrInitialDir),ext=Widen(a->lpstrDefExt);
    OPENFILENAMEW w={};w.lStructSize=sizeof(w);w.hwndOwner=a->hwndOwner;w.hInstance=a->hInstance;
    w.lpstrFilter=a->lpstrFilter?filter.c_str():nullptr;w.nFilterIndex=a->nFilterIndex;
    w.lpstrFile=file.data();w.nMaxFile=static_cast<DWORD>(file.size());
    w.lpstrInitialDir=a->lpstrInitialDir?dir.c_str():nullptr;w.lpstrTitle=a->lpstrTitle?title.c_str():nullptr;
    w.Flags=a->Flags;w.lpstrDefExt=a->lpstrDefExt?ext.c_str():nullptr;
    PickerLegacy::PlainOutcome out;{Ctl::PickerGuard guard;out=PickerLegacy::RunPlain(&w,save);}
    if(out.status==2){overrideError=false;return original(a);}
    overrideError=true;pickerError=0;if(out.status)return FALSE;
    for(auto& p:out.result.paths)if(!Representable(p)){
        // The ANSI caller cannot open a name outside its code page.
        Trace(L"ANSI caller: path not representable -> native dialog");overrideError=false;return original(a);
    }
    std::string output;size_t offset=0,extension=0;
    if(out.result.paths.size()==1){
        output=Narrow(out.result.paths[0]);
        std::string leaf=Narrow(Picker::Leaf(out.result.paths[0]));offset=output.size()-leaf.size();
        size_t dot=output.find_last_of('.');extension=dot!=std::string::npos&&dot>=offset?dot+1:0;output.push_back(0);
    }else{
        output=Narrow(Picker::Parent(out.result.paths[0]));output.push_back(0);offset=output.size();
        for(auto& p:out.result.paths){output+=Narrow(Picker::Leaf(p));output.push_back(0);}
    }
    output.push_back(0);
    if(output.size()>a->nMaxFile){
        pickerError=FNERR_BUFFERTOOSMALL;WORD needed=static_cast<WORD>(std::min<size_t>(output.size(),65535));
        memcpy(a->lpstrFile,&needed,sizeof(needed));return FALSE;
    }
    memcpy(a->lpstrFile,output.data(),output.size());
    a->nFileOffset=static_cast<WORD>(offset);a->nFileExtension=static_cast<WORD>(extension);a->nFilterIndex=out.result.index;
    if(!save&&!(a->Flags&OFN_HIDEREADONLY)){if(out.readOnly)a->Flags|=OFN_READONLY;else a->Flags&=~OFN_READONLY;}
    if(a->lpstrFileTitle&&a->nMaxFileTitle){auto leaf=Narrow(Picker::Leaf(out.result.paths[0]));
        size_t count=std::min<size_t>(leaf.size(),a->nMaxFileTitle-1);memcpy(a->lpstrFileTitle,leaf.data(),count);a->lpstrFileTitle[count]=0;}
    return TRUE;
}
BOOL WINAPI OpenHookA(LPOPENFILENAMEA ofn){return Ansi(ofn,false);}
BOOL WINAPI SaveHookA(LPOPENFILENAMEA ofn){return Ansi(ofn,true);}
}

// ================================================================ bridge: IFileOpenDialog / IFileSaveDialog
namespace Bridge {
// Supply metadata for a not-yet-existing save target without creating a file.
class FileBindData final:public IFileSystemBindData {
    LONG refs=1;WIN32_FIND_DATAW data={};
public:
    explicit FileBindData(const std::wstring& path){data.dwFileAttributes=FILE_ATTRIBUTE_NORMAL;auto leaf=Picker::Leaf(path);wcsncpy(data.cFileName,leaf.c_str(),MAX_PATH-1);}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** out)override{
        if(!out)return E_POINTER;*out=nullptr;if(iid!=IID_IUnknown&&iid!=IID_IFileSystemBindData)return E_NOINTERFACE;
        *out=static_cast<IFileSystemBindData*>(this);AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef()override{return InterlockedIncrement(&refs);}
    ULONG STDMETHODCALLTYPE Release()override{LONG n=InterlockedDecrement(&refs);if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE SetFindData(const WIN32_FIND_DATAW* p)override{if(!p)return E_POINTER;data=*p;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetFindData(WIN32_FIND_DATAW* p)override{if(!p)return E_POINTER;*p=data;return S_OK;}
};
HRESULT MakeItem(const std::wstring& path,bool save,IShellItem** out){
    *out=nullptr;if(path.empty())return E_INVALIDARG;
    if(!save||GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES)return SHCreateItemFromParsingName(path.c_str(),nullptr,IID_PPV_ARGS(out));
    IBindCtx* context=nullptr;HRESULT hr=CreateBindCtx(0,&context);if(FAILED(hr))return hr;
    auto data=new(std::nothrow)FileBindData(path);if(!data){context->Release();return E_OUTOFMEMORY;}
    hr=context->RegisterObjectParam(const_cast<wchar_t*>(L"File System Bind Data"),data);data->Release();
    if(SUCCEEDED(hr))hr=SHCreateItemFromParsingName(path.c_str(),context,IID_PPV_ARGS(out));
    context->Release();return hr;
}
std::wstring ItemPath(IShellItem* item){
    PWSTR path=nullptr;std::wstring out;if(item&&SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH,&path))&&path){out=path;CoTaskMemFree(path);}return out;
}
bool Blank(const wchar_t* text){if(!text)return true;for(;*text;++text)if(!iswspace(*text))return false;return true;}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
template<bool Save>class FileDialog final:public std::conditional_t<Save,IFileSaveDialog,IFileOpenDialog>,public IFileDialog2,
    public IFileDialogCustomize,public IOleWindow,public Ctl::Target {
    using Interface=std::conditional_t<Save,IFileSaveDialog,IFileOpenDialog>;
    LONG refs=1;Interface* inner;std::vector<IShellItem*> results;
    IFileDialogCustomize* customization=nullptr;IFileDialog2* dialog2=nullptr;
    bool custom=false,advanced=false,showing=false;
    Picker::Request request;DWORD group=0;
    std::wstring explicitFolder,defaultFolder;IPropertyStore* properties=nullptr;
    struct Sink {DWORD cookie;IFileDialogEvents* events;};
    std::vector<Sink> sinks;
    std::wstring liveFolder;std::vector<std::wstring> livePaths;
    bool initialized=false,approved=false,closed=false,notifying=false,resolveLinks=false;
    HRESULT closeReason=HRESULT_FROM_WIN32(ERROR_CANCELLED);

    void Unsupported(const wchar_t* what){if(!advanced)Trace(L"COM: %ls -> native dialog",what);advanced=true;}
    void Changed(){if(showing&&controller)PostMessageW(controller,WM_APP+6,0,0);}
    Picker::Control* Find(DWORD id){return request.Find(id);}
    IFileDialog* Self(){return static_cast<Interface*>(this);}
    ~FileDialog(){
        for(auto& s:sinks){inner->Unadvise(s.cookie);s.events->Release();}
        ClearResult();if(customization)customization->Release();if(dialog2)dialog2->Release();if(properties)properties->Release();inner->Release();
    }
    void ClearResult(){for(auto p:results)p->Release();results.clear();}
    HRESULT SetResults(const std::vector<std::wstring>& paths){
        std::vector<IShellItem*> next;
        for(auto& path:paths){if(path.empty())continue;IShellItem* item=nullptr;HRESULT hr=MakeItem(path,Save,&item);
            if(FAILED(hr)){for(auto p:next)p->Release();return hr;}next.push_back(item);}
        ClearResult();results=std::move(next);return S_OK;
    }
    HRESULT ItemResult(IShellItem** out){
        if(!out)return E_POINTER;*out=nullptr;if(results.size()!=1)return E_UNEXPECTED;
        results[0]->AddRef();*out=results[0];return S_OK;
    }
    HRESULT AddControl(DWORD id,DWORD kind,const wchar_t* label,DWORD value=Picker::None,const wchar_t* text=nullptr){
        if(Find(id))return S_OK;
        if(request.controls.size()>=Picker::MaxControls){Unsupported(L"more than 64 custom controls");return S_OK;}
        Picker::Control c;c.id=id;c.kind=kind;c.label=label?label:L"";c.text=text?text:L"";c.value=value;
        if(kind!=Picker::Group&&kind!=Picker::OpenDrop)c.parent=group;
        request.controls.push_back(std::move(c));Changed();return S_OK;
    }
    struct Snapshot {std::vector<IFileDialogEvents*> list;~Snapshot(){for(auto p:list)p->Release();}};
    template<class F>void EachControlSink(Snapshot& s,F f){
        for(auto sink:s.list){IFileDialogControlEvents* events=nullptr;
            if(SUCCEEDED(sink->QueryInterface(IID_PPV_ARGS(&events)))&&events){f(events);events->Release();}}
    }
public:
    explicit FileDialog(Interface* native):inner(native){
        inner->QueryInterface(IID_PPV_ARGS(&customization));inner->QueryInterface(IID_PPV_ARGS(&dialog2));
    }
    const Picker::Request* UiState()const override{return &request;}
    HRESULT Notify(Ctl::Event& e)override{
        if(closed)return closeReason==S_OK?E_ABORT:closeReason;
        if(notifying)return E_UNEXPECTED;
        notifying=true;struct Reset {bool& flag;~Reset(){flag=false;}} reset{notifying};
        if(e.kind==Ctl::RefreshEvent)return S_OK;
        // Hold a snapshot: callbacks may call Unadvise, including on themselves.
        Snapshot snapshot;for(auto& s:sinks){s.events->AddRef();snapshot.list.push_back(s.events);}
        custom=true;
        if(e.kind==Ctl::ControlEvent){
            auto c=Find(e.control);if(!c)return E_INVALIDARG;DWORD id=c->id;
            if(e.action==1){
                if(c->kind==Picker::Push)EachControlSink(snapshot,[&](IFileDialogControlEvents* x){x->OnButtonClicked(this,id);});
                else if(c->kind==Picker::Menu||c->kind==Picker::OpenDrop)EachControlSink(snapshot,[&](IFileDialogControlEvents* x){x->OnControlActivating(this,id);});
                return S_OK;
            }
            if(!(c->state&1))return E_ACCESSDENIED;
            switch(c->kind){
            case Picker::Check:{BOOL checked=e.value==1;c->value=checked;if(customization)customization->SetCheckButtonState(id,checked);
                EachControlSink(snapshot,[&](IFileDialogControlEvents* x){x->OnCheckButtonToggled(this,id,checked);});break;}
            case Picker::Combo:case Picker::Radio:case Picker::Menu:{
                if(!c->FindItem(e.value))return E_INVALIDARG;DWORD item=e.value;
                if(c->kind!=Picker::Menu){c->value=item;if(customization)customization->SetSelectedControlItem(id,item);}
                EachControlSink(snapshot,[&](IFileDialogControlEvents* x){x->OnItemSelected(this,id,item);});break;}
            case Picker::Edit:c->text=e.text;if(customization)customization->SetEditBoxText(id,c->text.c_str());break;
            default:return E_INVALIDARG;
            }
            return S_OK;
        }
        HRESULT hr=S_OK;
        // Folder events describe file system folders; virtual locations
        // (This PC, Network) are browsed without an application veto.
        bool folderChanged=!e.folder.empty()&&(!initialized||!Picker::SamePath(liveFolder,e.folder));
        if(folderChanged){
            IShellItem* folder=nullptr;
            if(SUCCEEDED(MakeItem(e.folder,false,&folder))){
                for(auto sink:snapshot.list){
                    hr=sink->OnFolderChanging(Self(),folder);
                    // Windows explicitly accepts E_NOTIMPL for an unused event.
                    if(hr==E_NOTIMPL)hr=S_OK;
                    if(hr!=S_OK){Trace(L"COM OnFolderChanging veto 0x%08lX",static_cast<unsigned long>(hr));break;}
                }
                folder->Release();if(hr!=S_OK)return hr;
            }
            liveFolder=e.folder;
        }
        std::vector<std::wstring> paths=e.paths;
        if(e.kind==Ctl::FileOkEvent&&resolveLinks)ResolveLinks(paths);
        bool typeChanged=!initialized||request.index!=e.index;
        bool selectionChanged=!initialized||livePaths!=paths;
        if(FAILED(SetResults(paths))){if(e.kind==Ctl::FileOkEvent)return E_INVALIDARG;ClearResult();}
        livePaths=paths;request.index=e.index;initialized=true;
        if(folderChanged)for(auto sink:snapshot.list)sink->OnFolderChange(Self());
        if(typeChanged)for(auto sink:snapshot.list)sink->OnTypeChange(Self());
        if(selectionChanged)for(auto sink:snapshot.list)sink->OnSelectionChange(Self());
        if(closed)return E_ABORT;
        if(e.kind==Ctl::FileOkEvent){
            approved=false;
            if(e.dropItem!=Picker::None){
                for(auto& c:request.controls)if(c.kind==Picker::OpenDrop&&c.FindItem(e.dropItem)){
                    c.value=e.dropItem;DWORD id=c.id,item=e.dropItem;
                    EachControlSink(snapshot,[&](IFileDialogControlEvents* x){x->OnItemSelected(this,id,item);});
                }
            }
            for(auto sink:snapshot.list){
                hr=sink->OnFileOk(Self());
                if(hr==E_NOTIMPL)hr=S_OK;
                if(hr!=S_OK){Trace(L"COM OnFileOk veto 0x%08lX: picker stays open",static_cast<unsigned long>(hr));break;}
            }
            if(hr!=S_OK)return hr;
            if(closed)return E_ABORT;
            approved=true;
        }
        return S_OK;
    }
    // ---- IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** out)override{
        if(!out)return E_POINTER;*out=nullptr;
        if(iid==IID_IUnknown||iid==IID_IModalWindow||iid==IID_IFileDialog||iid==(Save?IID_IFileSaveDialog:IID_IFileOpenDialog))
            *out=static_cast<Interface*>(this);
        else if(iid==IID_IFileDialog2&&dialog2)*out=static_cast<IFileDialog2*>(this);
        else if(iid==IID_IFileDialogCustomize&&customization)*out=static_cast<IFileDialogCustomize*>(this);
        else if(iid==IID_IOleWindow)*out=static_cast<IOleWindow*>(this);
        if(*out){AddRef();return S_OK;}
        // Preserve application-specific COM extensions. Such callers use the
        // native dialog, not a partial emulation that drops their controls.
        HRESULT hr=inner->QueryInterface(iid,out);
        if(SUCCEEDED(hr)){wchar_t id[64]={};StringFromGUID2(iid,id,64);Unsupported((std::wstring(L"extension interface ")+id).c_str());}
        return hr;
    }
    ULONG STDMETHODCALLTYPE AddRef()override{return InterlockedIncrement(&refs);}
    ULONG STDMETHODCALLTYPE Release()override{LONG n=InterlockedDecrement(&refs);if(!n)delete this;return n;}
    // ---- IOleWindow: while our picker is up, the dialog window is Explorer's.
    HRESULT STDMETHODCALLTYPE GetWindow(HWND* window)override{
        if(!window)return E_POINTER;
        if(showing&&custom){*window=frame?frame:nullptr;return frame?S_OK:E_FAIL;}
        IOleWindow* native=nullptr;HRESULT hr=inner->QueryInterface(IID_PPV_ARGS(&native));
        if(FAILED(hr))return hr;hr=native->GetWindow(window);native->Release();return hr;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL)override{return E_NOTIMPL;}
    // ---- IModalWindow
    HRESULT STDMETHODCALLTYPE Show(HWND owner)override{
        if(g_unloading||showing)return inner->Show(owner);
        ClearResult();custom=false;
        FILEOPENDIALOGOPTIONS options=0;HRESULT hr=inner->GetOptions(&options);if(FAILED(hr))return hr;
        constexpr FILEOPENDIALOGOPTIONS unsupported=FOS_CREATEPROMPT;
        if(advanced||(options&unsupported)||(Save&&(options&FOS_ALLOWMULTISELECT))||
           ((options&FOS_PICKFOLDERS)&&(options&FOS_ALLOWMULTISELECT))||Ctl::inPicker){
            Trace(L"COM native dialog: advanced=%d options=0x%08lX sinks=%zu",advanced,static_cast<unsigned long>(options),sinks.size());
            return inner->Show(owner);
        }
        request.mode=Save?Picker::Save:(options&FOS_PICKFOLDERS)?Picker::Folder:(options&FOS_ALLOWMULTISELECT)?Picker::Multi:Picker::OpenFile;
        request.flags=(options&FOS_STRICTFILETYPES)?Picker::StrictTypes:0;
        resolveLinks=!Save&&!(options&FOS_NODEREFERENCELINKS)&&!(options&FOS_PICKFOLDERS);
        const bool rememberSave=Save;
        request.folder=explicitFolder;
        if(request.folder.empty())request.folder=Remembered(rememberSave);
        if(request.folder.empty()){IShellItem* current=nullptr;if(SUCCEEDED(inner->GetFolder(&current))&&current){request.folder=ItemPath(current);current->Release();}}
        if(request.folder.empty()||!IsDirectory(request.folder))request.folder=defaultFolder;
        if(request.folder.empty()||!IsDirectory(request.folder))request.folder=KnownFolder(FOLDERID_Desktop);
        if(request.index<1||request.index>std::max<size_t>(1,request.filters.size()))request.index=1;
        initialized=false;approved=false;closed=false;liveFolder.clear();livePaths.clear();frame=nullptr;
        Ctl::Result reply;int status;
        {Ctl::PickerGuard guard;showing=true;custom=true;status=RunPicker(owner,request,reply,this);showing=false;}
        ClearResult();
        if(closed){Trace(L"COM closed by the application");return FAILED(closeReason)?closeReason:HRESULT_FROM_WIN32(ERROR_CANCELLED);}
        if(status==1)return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        if(status!=0||!approved){custom=false;Trace(L"COM picker unavailable -> native dialog");return inner->Show(owner);}
        if(resolveLinks)ResolveLinks(reply.paths);
        request.index=reply.index;
        hr=SetResults(reply.paths);if(FAILED(hr))return hr;
        Remember(rememberSave,request.mode==Picker::Folder?reply.paths[0]:Picker::Parent(reply.paths[0]));
        Trace(L"COM results ready");return S_OK;
    }
    // ---- IFileDialog
    HRESULT STDMETHODCALLTYPE SetFileTypes(UINT n,const COMDLG_FILTERSPEC* specs)override{
        HRESULT hr=inner->SetFileTypes(n,specs);if(SUCCEEDED(hr)){
            request.filters.clear();if(n>Picker::MaxFilters){Unsupported(L"more than 128 file types");return hr;}
            for(UINT i=0;i<n;i++)request.filters.push_back({specs[i].pszName?specs[i].pszName:L"",specs[i].pszSpec?specs[i].pszSpec:L""});
        }return hr;
    }
    HRESULT STDMETHODCALLTYPE SetFileTypeIndex(UINT n)override{HRESULT hr=inner->SetFileTypeIndex(n);if(SUCCEEDED(hr))request.index=n?n:1;return hr;}
    HRESULT STDMETHODCALLTYPE GetFileTypeIndex(UINT* n)override{if(!custom)return inner->GetFileTypeIndex(n);if(!n)return E_POINTER;*n=request.index;return S_OK;}
    HRESULT STDMETHODCALLTYPE Advise(IFileDialogEvents* events,DWORD* cookie)override{
        if(!events||!cookie)return E_POINTER;
        HRESULT hr=inner->Advise(events,cookie);if(SUCCEEDED(hr)){events->AddRef();sinks.push_back({*cookie,events});}return hr;
    }
    HRESULT STDMETHODCALLTYPE Unadvise(DWORD cookie)override{
        HRESULT hr=inner->Unadvise(cookie);if(SUCCEEDED(hr)){
            for(auto i=sinks.begin();i!=sinks.end();++i)if(i->cookie==cookie){auto events=i->events;sinks.erase(i);events->Release();break;}
        }return hr;
    }
    HRESULT STDMETHODCALLTYPE SetOptions(FILEOPENDIALOGOPTIONS options)override{return inner->SetOptions(options);}
    HRESULT STDMETHODCALLTYPE GetOptions(FILEOPENDIALOGOPTIONS* options)override{return inner->GetOptions(options);}
    HRESULT STDMETHODCALLTYPE SetDefaultFolder(IShellItem* folder)override{HRESULT hr=inner->SetDefaultFolder(folder);if(SUCCEEDED(hr))defaultFolder=ItemPath(folder);return hr;}
    HRESULT STDMETHODCALLTYPE SetFolder(IShellItem* folder)override{
        HRESULT hr=inner->SetFolder(folder);
        if(SUCCEEDED(hr)){explicitFolder=ItemPath(folder);
            // Called from a callback: move the visible picker there.
            if(showing&&controller&&!explicitFolder.empty()){Trace(L"COM SetFolder during Show: %ls",explicitFolder.c_str());}
        }return hr;
    }
    HRESULT STDMETHODCALLTYPE GetFolder(IShellItem** folder)override{
        if(!folder)return E_POINTER;if(custom&&!liveFolder.empty())return MakeItem(liveFolder,false,folder);return inner->GetFolder(folder);
    }
    HRESULT STDMETHODCALLTYPE GetCurrentSelection(IShellItem** item)override{return custom?ItemResult(item):inner->GetCurrentSelection(item);}
    HRESULT STDMETHODCALLTYPE SetFileName(LPCWSTR name)override{HRESULT hr=inner->SetFileName(name);if(SUCCEEDED(hr))request.name=name?name:L"";return hr;}
    HRESULT STDMETHODCALLTYPE GetFileName(LPWSTR* name)override{
        if(!custom)return inner->GetFileName(name);if(!name)return E_POINTER;*name=nullptr;
        if(results.size()==1)return results[0]->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,name);
        return SHStrDupW(request.name.c_str(),name);
    }
    HRESULT STDMETHODCALLTYPE SetTitle(LPCWSTR title)override{HRESULT hr=inner->SetTitle(title);if(SUCCEEDED(hr))request.title=title?title:L"";return hr;}
    HRESULT STDMETHODCALLTYPE SetOkButtonLabel(LPCWSTR text)override{HRESULT hr=inner->SetOkButtonLabel(text);if(SUCCEEDED(hr)){request.okLabel=text?text:L"";Changed();}return hr;}
    HRESULT STDMETHODCALLTYPE SetFileNameLabel(LPCWSTR text)override{HRESULT hr=inner->SetFileNameLabel(text);if(SUCCEEDED(hr)){request.nameLabel=text?text:L"";Changed();}return hr;}
    HRESULT STDMETHODCALLTYPE GetResult(IShellItem** item)override{return custom?ItemResult(item):inner->GetResult(item);}
    HRESULT STDMETHODCALLTYPE AddPlace(IShellItem* item,FDAP align)override{return inner->AddPlace(item,align);}
    HRESULT STDMETHODCALLTYPE SetDefaultExtension(LPCWSTR ext)override{HRESULT hr=inner->SetDefaultExtension(ext);if(SUCCEEDED(hr))request.extension=ext?ext:L"";return hr;}
    HRESULT STDMETHODCALLTYPE Close(HRESULT reason)override{
        if(showing&&controller){closed=true;closeReason=reason;PostMessageW(controller,WM_CLOSE,0,0);return S_OK;}return inner->Close(reason);
    }
    HRESULT STDMETHODCALLTYPE SetClientGuid(REFGUID guid)override{return inner->SetClientGuid(guid);}
    HRESULT STDMETHODCALLTYPE ClearClientData()override{return inner->ClearClientData();}
    HRESULT STDMETHODCALLTYPE SetFilter(IShellItemFilter* filter)override{Unsupported(L"SetFilter");return inner->SetFilter(filter);}
    // ---- IFileDialog2
    HRESULT STDMETHODCALLTYPE SetCancelButtonLabel(LPCWSTR label)override{
        HRESULT hr=dialog2?dialog2->SetCancelButtonLabel(label):E_NOINTERFACE;if(SUCCEEDED(hr)){request.cancelLabel=label?label:L"";Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE SetNavigationRoot(IShellItem* item)override{Unsupported(L"SetNavigationRoot");return dialog2?dialog2->SetNavigationRoot(item):E_NOINTERFACE;}
    // ---- IFileOpenDialog
    HRESULT STDMETHODCALLTYPE GetResults(IShellItemArray** items){
        if constexpr(Save){if(items)*items=nullptr;return E_NOINTERFACE;}
        else {
            if(!custom)return inner->GetResults(items);if(!items)return E_POINTER;*items=nullptr;if(results.empty())return E_UNEXPECTED;
            std::vector<PIDLIST_ABSOLUTE> ids;HRESULT hr=S_OK;
            for(auto item:results){PIDLIST_ABSOLUTE id=nullptr;hr=SHGetIDListFromObject(item,&id);if(FAILED(hr))break;ids.push_back(id);}
            if(SUCCEEDED(hr))hr=SHCreateShellItemArrayFromIDLists(static_cast<UINT>(ids.size()),const_cast<PCIDLIST_ABSOLUTE*>(ids.data()),items);
            for(auto id:ids)CoTaskMemFree(id);return hr;
        }
    }
    HRESULT STDMETHODCALLTYPE GetSelectedItems(IShellItemArray** items){
        if constexpr(Save){if(items)*items=nullptr;return E_NOINTERFACE;}
        else return custom?GetResults(items):inner->GetSelectedItems(items);
    }
    // ---- IFileSaveDialog
    HRESULT STDMETHODCALLTYPE SetSaveAsItem(IShellItem* item){
        if constexpr(Save){
            HRESULT hr=inner->SetSaveAsItem(item);if(FAILED(hr))return hr;
            auto path=ItemPath(item);
            if(path.empty())Unsupported(L"SetSaveAsItem outside the file system");
            else{explicitFolder=Picker::Parent(path);request.name=Picker::Leaf(path);}
            return hr;
        }else return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetProperties(IPropertyStore* p){
        if constexpr(Save){HRESULT hr=inner->SetProperties(p);if(SUCCEEDED(hr)&&p){if(properties)properties->Release();properties=p;p->AddRef();}return hr;}else return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetCollectedProperties(IPropertyDescriptionList* p,BOOL append){if constexpr(Save)return inner->SetCollectedProperties(p,append);else return E_NOINTERFACE;}
    HRESULT STDMETHODCALLTYPE GetProperties(IPropertyStore** p){
        if constexpr(Save){
            if(!custom)return inner->GetProperties(p);if(!p)return E_POINTER;*p=nullptr;
            // Explorer shows no property editor; hand back what the caller supplied.
            if(!properties)return E_UNEXPECTED;properties->AddRef();*p=properties;return S_OK;
        }else return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE ApplyProperties(IShellItem* item,IPropertyStore* p,HWND h,IFileOperationProgressSink* sink){
        if constexpr(Save)return inner->ApplyProperties(item,p,h,sink);else return E_NOINTERFACE;
    }
    // ---- IFileDialogCustomize (mirrored; the native object is kept in step for a fallback)
#define PICKER_NATIVE(call) if(!customization)return E_NOINTERFACE;HRESULT hr=customization->call;if(FAILED(hr))return hr
    HRESULT STDMETHODCALLTYPE EnableOpenDropDown(DWORD id)override{PICKER_NATIVE(EnableOpenDropDown(id));return AddControl(id,Picker::OpenDrop,L"");}
    HRESULT STDMETHODCALLTYPE AddMenu(DWORD id,LPCWSTR text)override{PICKER_NATIVE(AddMenu(id,text));return AddControl(id,Picker::Menu,text);}
    HRESULT STDMETHODCALLTYPE AddPushButton(DWORD id,LPCWSTR text)override{PICKER_NATIVE(AddPushButton(id,text));return AddControl(id,Picker::Push,text);}
    HRESULT STDMETHODCALLTYPE AddComboBox(DWORD id)override{PICKER_NATIVE(AddComboBox(id));return AddControl(id,Picker::Combo,L"");}
    HRESULT STDMETHODCALLTYPE AddRadioButtonList(DWORD id)override{PICKER_NATIVE(AddRadioButtonList(id));return AddControl(id,Picker::Radio,L"");}
    HRESULT STDMETHODCALLTYPE AddCheckButton(DWORD id,LPCWSTR label,BOOL checked)override{PICKER_NATIVE(AddCheckButton(id,label,checked));return AddControl(id,Picker::Check,label,checked?1:0);}
    HRESULT STDMETHODCALLTYPE AddEditBox(DWORD id,LPCWSTR text)override{PICKER_NATIVE(AddEditBox(id,text));return AddControl(id,Picker::Edit,L"",Picker::None,text);}
    HRESULT STDMETHODCALLTYPE AddSeparator(DWORD id)override{PICKER_NATIVE(AddSeparator(id));return AddControl(id,Picker::Separator,L"");}
    HRESULT STDMETHODCALLTYPE AddText(DWORD id,LPCWSTR text)override{
        PICKER_NATIVE(AddText(id,text));
        // An empty text only reserves room for a child window the application
        // creates later inside the native dialog (WDL/REAPER). That needs the
        // real dialog window.
        if(Blank(text))Unsupported(L"empty text placeholder for an application panel");
        return AddControl(id,Picker::Text,text);
    }
    HRESULT STDMETHODCALLTYPE SetControlLabel(DWORD id,LPCWSTR label)override{
        PICKER_NATIVE(SetControlLabel(id,label));if(auto c=Find(id)){c->label=label?label:L"";Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE GetControlState(DWORD id,CDCONTROLSTATEF* state)override{
        if(!state)return E_POINTER;if(custom){if(auto c=Find(id)){*state=static_cast<CDCONTROLSTATEF>(c->state);return S_OK;}}
        return customization?customization->GetControlState(id,state):E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetControlState(DWORD id,CDCONTROLSTATEF state)override{
        PICKER_NATIVE(SetControlState(id,state));if(auto c=Find(id)){c->state=static_cast<DWORD>(state)&3u;Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE GetEditBoxText(DWORD id,WCHAR** text)override{
        if(!text)return E_POINTER;if(custom){if(auto c=Find(id);c&&c->kind==Picker::Edit)return SHStrDupW(c->text.c_str(),text);}
        return customization?customization->GetEditBoxText(id,text):E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetEditBoxText(DWORD id,LPCWSTR text)override{
        PICKER_NATIVE(SetEditBoxText(id,text));if(auto c=Find(id)){c->text=text?text:L"";Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE GetCheckButtonState(DWORD id,BOOL* checked)override{
        if(!checked)return E_POINTER;if(custom){if(auto c=Find(id);c&&c->kind==Picker::Check){*checked=c->value==1;return S_OK;}}
        return customization?customization->GetCheckButtonState(id,checked):E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetCheckButtonState(DWORD id,BOOL checked)override{
        PICKER_NATIVE(SetCheckButtonState(id,checked));if(auto c=Find(id)){c->value=checked?1:0;Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE AddControlItem(DWORD id,DWORD item,LPCWSTR label)override{
        PICKER_NATIVE(AddControlItem(id,item,label));
        auto c=Find(id);if(!c){Unsupported(L"item for an unknown control");return hr;}
        if(c->items.size()>=Picker::MaxItems){Unsupported(L"more than 256 items");return hr;}
        if(!c->FindItem(item)){c->items.push_back({item,3,label?label:L""});Changed();}
        return hr;
    }
    HRESULT STDMETHODCALLTYPE RemoveControlItem(DWORD id,DWORD item)override{
        PICKER_NATIVE(RemoveControlItem(id,item));
        if(auto c=Find(id)){c->items.erase(std::remove_if(c->items.begin(),c->items.end(),[&](const Picker::Item& i){return i.id==item;}),c->items.end());
            if(c->value==item&&c->kind!=Picker::Check)c->value=Picker::None;Changed();}
        return hr;
    }
    HRESULT STDMETHODCALLTYPE RemoveAllControlItems(DWORD id)override{
        PICKER_NATIVE(RemoveAllControlItems(id));if(auto c=Find(id)){c->items.clear();if(c->kind!=Picker::Check)c->value=Picker::None;Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE GetControlItemState(DWORD id,DWORD item,CDCONTROLSTATEF* state)override{
        if(!state)return E_POINTER;
        if(custom){if(auto c=Find(id))if(auto i=c->FindItem(item)){*state=static_cast<CDCONTROLSTATEF>(i->state);return S_OK;}}
        return customization?customization->GetControlItemState(id,item,state):E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetControlItemState(DWORD id,DWORD item,CDCONTROLSTATEF state)override{
        PICKER_NATIVE(SetControlItemState(id,item,state));
        if(auto c=Find(id))if(auto i=c->FindItem(item)){i->state=static_cast<DWORD>(state)&3u;Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE GetSelectedControlItem(DWORD id,DWORD* item)override{
        if(!item)return E_POINTER;
        if(custom){if(auto c=Find(id);c&&c->kind!=Picker::Check){if(c->value==Picker::None)return E_FAIL;*item=c->value;return S_OK;}}
        return customization?customization->GetSelectedControlItem(id,item):E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE SetSelectedControlItem(DWORD id,DWORD item)override{
        PICKER_NATIVE(SetSelectedControlItem(id,item));if(auto c=Find(id)){c->value=item;Changed();}return hr;
    }
    HRESULT STDMETHODCALLTYPE StartVisualGroup(DWORD id,LPCWSTR label)override{
        PICKER_NATIVE(StartVisualGroup(id,label));HRESULT added=AddControl(id,Picker::Group,label);group=id;return added;
    }
    HRESULT STDMETHODCALLTYPE EndVisualGroup()override{PICKER_NATIVE(EndVisualGroup());group=0;return hr;}
    HRESULT STDMETHODCALLTYPE MakeProminent(DWORD id)override{PICKER_NATIVE(MakeProminent(id));if(auto c=Find(id))c->prominent=1;return hr;}
    HRESULT STDMETHODCALLTYPE SetControlItemText(DWORD id,DWORD item,LPCWSTR text)override{
        PICKER_NATIVE(SetControlItemText(id,item,text));if(auto c=Find(id))if(auto i=c->FindItem(item)){i->label=text?text:L"";Changed();}return hr;
    }
#undef PICKER_NATIVE
};
#pragma clang diagnostic pop

using CreateFn=HRESULT(WINAPI*)(REFCLSID,LPUNKNOWN,DWORD,REFIID,LPVOID*);
CreateFn originalCreate=nullptr;
template<bool Save>HRESULT CreatePickerDialog(DWORD context,REFIID iid,void** out){
    using Interface=std::conditional_t<Save,IFileSaveDialog,IFileOpenDialog>;
    Interface* native=nullptr;HRESULT hr=originalCreate(Save?CLSID_FileSaveDialog:CLSID_FileOpenDialog,nullptr,context,
        Save?IID_IFileSaveDialog:IID_IFileOpenDialog,reinterpret_cast<void**>(&native));
    if(FAILED(hr))return hr;
    PinModule();
    auto proxy=new(std::nothrow)FileDialog<Save>(native);
    if(!proxy){native->Release();return E_OUTOFMEMORY;}hr=proxy->QueryInterface(iid,out);proxy->Release();return hr;
}
HRESULT WINAPI CreateHook(REFCLSID clsid,LPUNKNOWN outer,DWORD context,REFIID iid,LPVOID* out){
    if(g_unloading||outer||!out)return originalCreate(clsid,outer,context,iid,out);
    bool save=IsEqualCLSID(clsid,CLSID_FileSaveDialog),open=IsEqualCLSID(clsid,CLSID_FileOpenDialog);
    if(!save&&!open)return originalCreate(clsid,outer,context,iid,out);
    bool supportedIid=iid==(save?IID_IFileSaveDialog:IID_IFileOpenDialog)||iid==IID_IFileDialog||iid==IID_IModalWindow||
        iid==IID_IUnknown||iid==IID_IFileDialog2||iid==IID_IFileDialogCustomize;
    if(!supportedIid)return originalCreate(clsid,outer,context,iid,out);
    *out=nullptr;return save?CreatePickerDialog<true>(context,iid,out):CreatePickerDialog<false>(context,iid,out);
}
}

// ================================================================ initialization
namespace {
bool g_host=false;

struct TokenHandle {HANDLE h=nullptr;~TokenHandle(){if(h)CloseHandle(h);}};
// Services, system accounts, critical processes and AppContainers have no
// interactive file dialogs worth replacing; fail closed when unsure.
bool InteractiveProcess(){
    DWORD session=0;if(!ProcessIdToSessionId(GetCurrentProcessId(),&session)||!session)return false;
    using CriticalFn=BOOL(WINAPI*)(HANDLE,PBOOL);
    auto isCritical=reinterpret_cast<CriticalFn>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"IsProcessCritical"));
    BOOL critical=FALSE;if(!isCritical||!isCritical(GetCurrentProcess(),&critical)||critical)return false;
    TokenHandle token;if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token.h))return false;
    DWORD bytes=0,appContainer=0;
    if(!GetTokenInformation(token.h,TokenIsAppContainer,&appContainer,sizeof(appContainer),&bytes)||appContainer)return false;
    if(GetTokenInformation(token.h,TokenUser,nullptr,0,&bytes)||GetLastError()!=ERROR_INSUFFICIENT_BUFFER||bytes<sizeof(TOKEN_USER)||bytes>65536)return false;
    std::vector<BYTE> storage(bytes);
    if(!GetTokenInformation(token.h,TokenUser,storage.data(),bytes,&bytes))return false;
    auto user=reinterpret_cast<TOKEN_USER*>(storage.data());
    return !IsWellKnownSid(user->User.Sid,WinLocalSystemSid)&&
        !IsWellKnownSid(user->User.Sid,WinLocalServiceSid)&&!IsWellKnownSid(user->User.Sid,WinNetworkServiceSid);
}
void ReadSettings(){
    auto dir=Setting(L"logDirectory");
    while(!dir.empty()&&(dir.back()==L'\\'||dir.back()==L'/'||dir.back()==L' '))dir.pop_back();
    g_logDirectory=Bridge::IsDirectory(dir)?dir:L"";
}
template<class T>bool Hook(HMODULE module,const char* name,T hook,T* original){
    void* target=module?reinterpret_cast<void*>(GetProcAddress(module,name)):nullptr;
    return target&&Wh_SetFunctionHook(target,reinterpret_cast<void*>(hook),reinterpret_cast<void**>(original));
}
bool InstallBridge(const std::wstring& name){
    std::wstring lower=name;CharLowerBuffW(lower.data(),static_cast<DWORD>(lower.size()));
    Bridge::demoProcess=lower==L"explorer-picker-demo.exe"||lower==L"ep-demo.exe";
    Bridge::ntscProcess=lower==L"ntsc-rs-standalone.exe";
    // ntsc-rs keeps the keys it used before the mods were merged.
    Bridge::lastOpenKey=Bridge::ntscProcess?L"lastMediaFolder":lower+L".lastOpenFolder";
    Bridge::lastSaveKey=Bridge::ntscProcess?L"lastSaveFolder":lower+L".lastSaveFolder";
    HMODULE comdlg=LoadLibraryExW(L"comdlg32.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!comdlg)return false;
    bool ok=Hook(comdlg,"GetOpenFileNameW",&Bridge::OpenHookW,&Bridge::originalOpenW)&&
        Hook(comdlg,"GetSaveFileNameW",&Bridge::SaveHookW,&Bridge::originalSaveW)&&
        Hook(comdlg,"CommDlgExtendedError",&Bridge::ErrorHook,&Bridge::originalError);
    // ANSI entry points are optional: a failure leaves them native.
    Hook(comdlg,"GetOpenFileNameA",&Bridge::OpenHookA,&Bridge::originalOpenA);
    Hook(comdlg,"GetSaveFileNameA",&Bridge::SaveHookA,&Bridge::originalSaveA);
    ok=ok&&Wh_SetFunctionHook(reinterpret_cast<void*>(CoCreateInstance),reinterpret_cast<void*>(Bridge::CreateHook),reinterpret_cast<void**>(&Bridge::originalCreate));
    return ok;
}
}

BOOL Wh_ModInit(){
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&Wh_ModInit),&g_module))return FALSE;
    wchar_t exe[32768]={};DWORD length=GetModuleFileNameW(nullptr,exe,32768);
    if(!length||length>=32768)return FALSE;
    const wchar_t* name=wcsrchr(exe,L'\\');name=name?name+1:exe;
    if(!_wcsicmp(name,L"windhawk.exe")||!InteractiveProcess())return FALSE;
    // Chromium/Electron child processes (renderers, GPU, sandboxed utilities)
    // never show file dialogs; the browser process does, or Chrome's own
    // "Windows utilities" process. Stay out of the sandboxes.
    {const wchar_t* command=GetCommandLineW();
        if(command&&wcsstr(command,L" --type=")&&!wcsstr(command,L"--utility-sub-type=chrome.mojom.UtilWin"))return FALSE;}
    ReadSettings();
    bool explorer=!_wcsicmp(name,L"explorer.exe");
    if(explorer){
        g_host=Host::Init();
        if(!g_host)Wh_Log(L"Explorer host could not start");
        Bridge::explorerProcess=true;
        if(!Wh_GetIntSetting(L"explorerDialogs"))return g_host;
    }
    if(!InstallBridge(name))return g_host;
    Wh_Log(L"Explorer Picker 1.0 %ls %ls",explorer?L"host+bridge":L"bridge",sizeof(void*)==8?L"x64":L"x86");
    return TRUE;
}
void Wh_ModBeforeUninit(){
    g_unloading=true;
    if(g_host)Host::BeforeUninit();
}
void Wh_ModUninit(){
    if(g_host)Host::Uninit();
}
void Wh_ModSettingsChanged(){ReadSettings();}
