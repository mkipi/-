// ServicesApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ServicesApp.h"
#include "CommCtrl.h"
#include "winsvc.h"
#include <string>
#include <vector>
#include <sstream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                               
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVICESAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVICESAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVICESAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVICESAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
HWND hWnd;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED |  WS_CAPTION |  WS_SYSMENU  | WS_MINIMIZEBOX,
      100, 100, 833, 800, nullptr, nullptr, hInstance, nullptr);
   
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
LPWSTR StringConvert(std::wstring text) {
    WCHAR* buffer = new WCHAR[text.size() + 1];
    wcscpy_s(buffer, text.size() + 1, text.c_str());
    buffer[text.size()] = '\0'; 
    return buffer;
}

HWND hData;
void CreateColums() {
    std::wstring text1 = L"Имя";
    LVCOLUMN LVCOLUMN1; 
    LVCOLUMN1.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT; 
    LVCOLUMN1.fmt = LVCFMT_LEFT;
    LVCOLUMN1.cx = 500; 
    LVCOLUMN1.pszText = StringConvert(text1);
    ListView_InsertColumn(hData, 0, &LVCOLUMN1);

    std::wstring text2 = L"Состояние";
    LVCOLUMN LVCOLUMN2;
    LVCOLUMN2.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    LVCOLUMN2.fmt = LVCFMT_LEFT;
    LVCOLUMN2.cx = 300;
    LVCOLUMN2.pszText = StringConvert(text2);
    ListView_InsertColumn(hData, 1, &LVCOLUMN2);
}

DWORD size;
ENUM_SERVICE_STATUSW* arr;
SC_HANDLE ServicesDB;
void LoadData();
DWORD WINAPI GetServices(LPVOID param) {
    DWORD error;
    ServicesDB = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    error = GetLastError();
    if (error != 0)
    {
        MessageBox(hWnd, StringConvert(std::to_wstring(error)), L"OpenSCManagerA", NULL);
    }
    DWORD needed, count, resume = 0;
    // что бы узнать сколько нужно места
    EnumServicesStatusW(
        ServicesDB,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        NULL,
        0,
        &needed,
        &count,
        &resume);
    arr = new ENUM_SERVICE_STATUS[needed];

    // записать массив
    EnumServicesStatusW(
        ServicesDB,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        arr,
        needed,
        &needed,
        &count,
        &resume);
    size = count;
    LoadData();
    return 0;
}

void LoadData() {
    for (int i = 0; i < size; i++)
    {
        LV_ITEM LVInsert;
        LVInsert.mask = LVIF_TEXT | LVIF_IMAGE;
        LVInsert.iItem = i;
        LVInsert.iSubItem = 0;
        LVInsert.pszText = arr[i].lpDisplayName;
        ListView_InsertItem(hData, &LVInsert);

        LVITEM LVItemColum;
        LVItemColum.mask = LVIF_TEXT;
        LVItemColum.iItem = i;
        std::wstring text;
        switch (arr[i].ServiceStatus.dwCurrentState)
        {
        case SERVICE_RUNNING: {
            text = L"Выполняется";
            break;
        }
        case SERVICE_STOPPED: {
            text = L"";
            break;
        }
        case SERVICE_PAUSED: {
            text = L"Остановлена";
            break;
        }
        case SERVICE_START_PENDING: {
            text = L"Запускается";
            break;
        }
        default:
            text = L"Неизвестное состояние";
        }
        LVItemColum.pszText = StringConvert(text);
        LVItemColum.iSubItem = 1;
        ListView_SetItem(hData, &LVItemColum);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        RECT rt;
        GetClientRect(hWnd, &rt);
        hData = CreateWindowW(WC_LISTVIEWW, L"ServicesList", WS_CHILD | WS_VISIBLE | WS_BORDER |
            LVS_REPORT | LVS_SINGLESEL | WS_HSCROLL | WS_HSCROLL,
            0, 0, rt.right, rt.bottom, hWnd, (HMENU)0, hInst, nullptr);
        CreateColums();
        CreateThread(NULL, 0, GetServices, NULL, 0, 0);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        
        switch (wmId)
        {
        
        case 3: // запустить
        {
            int index = ListView_GetNextItem(hData, -1, LVNI_SELECTED);
            if (index == -1)
            {
                break;
            }
         
            std::wstring command = L"sc start ";
            command += arr[index].lpServiceName;
            _wsystem(command.c_str());
            CreateThread(NULL, 0, GetServices, NULL, 0, 0);
            break;
        }
        case 4: // остановить
        {
            int index = ListView_GetNextItem(hData, -1, LVNI_SELECTED);
            if (index == -1)
            {
                break;
            }
            std::wstring command = L"sc stop ";
            command += arr[index].lpServiceName;
            _wsystem(command.c_str());
            CreateThread(NULL, 0, GetServices, NULL, 0, 0);
            break;
        }
        case 10:
        {
            CreateThread(NULL, 0, GetServices, NULL, 0, 0);
            break;
        }
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_NOTIFY: {
        NMHDR* notifyMess = (NMHDR*)lParam; // реинтерпрет каст структуры из лпарама
        switch (notifyMess->code)
        {
        case NM_RCLICK: {
            // клик правой
                POINT cursor;
                GetCursorPos(&cursor); // взять позицию курсора
                // создать контектсное меню
                TrackPopupMenu((HMENU)GetSubMenu(LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1)), 0), TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
            }
            break;
        }
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
