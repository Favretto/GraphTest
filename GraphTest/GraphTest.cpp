//
// LARA's BIRTHDAY
// 
// 
//
// ------------------------
// GraphTest - Versione 1.0
// Alessandro Favretto
// 31/10/2025
// ------------------------
//

#define _USE_MATH_DEFINES

#include <windows.h>
#include <shellapi.h>   // Aggiunto: necessario per la tray icon (Shell_NotifyIcon)
#include "resource.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

#define WM_TRAYICON (WM_USER + 1)   // Aggiunto: messaggio personalizzato per la tray icon
NOTIFYICONDATA nid = { 0 };         // Aggiunto: struttura per l’icona di notifica
#define TRAY_ICON_ID 1              // Per icona sulla TNR

#define IDM_FILE_EXIT  101
#define IDM_WINVER 102
#define IDM_HELP_ABOUT 103
#define IDM_SYSINFO 107
#define IDM_ALWAYS_ON_TOP 109
#define IDM_TEMPDIR 110
#define IDM_USRDIR 111
#define IDM_WINDIR 112

#define IDM_OPEN_GUI 200

// 1.Timer 
#define IDT_CLOCK_TIMER 5001

WCHAR szTempPath[MAX_PATH];

HWND hwnd = NULL;

void SetAlwaysOnTop(HWND hwnd, BOOL enable);

BOOL isTopMost = FALSE;
BOOL isAppRunningAsAdmin = FALSE;

float fAngle = 0.0f; // RANDOM ANGLE 0<THETA<=PI/2 ...
float fDeltaX = static_cast<float>(M_SQRT2 / 2.0);
float fDeltaY = fDeltaX;
float fX = 320.0f;
float fY = 240.0f;
float fCircleRay = 25.0;
BOOL bSignX = TRUE; // Inc
BOOL bSignY = TRUE; // Inc
BOOL bRayInc = TRUE;

HANDLE hMutex = NULL;
HBITMAP hBitmap = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE retSE = NULL;

    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        int width = rcClient.right - rcClient.left;
        int height = rcClient.bottom - rcClient.top;

        // --- Memory DC per double buffering ---
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, width, height);
        HGDIOBJ oldBmpMem = SelectObject(hdcMem, hbmMem);

        // Sfondo nero
        HBRUSH hBr = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdcMem, &rcClient, hBr);
        DeleteObject(hBr);

        // Disegna bitmap se presente
        if (hBitmap) {
            HDC hdcBmp = CreateCompatibleDC(hdcMem);
            HGDIOBJ oldBmp = SelectObject(hdcBmp, hBitmap);
            BitBlt(hdcMem, 0, 0, width, height, hdcBmp, 0, 0, SRCCOPY);
            SelectObject(hdcBmp, oldBmp);
            DeleteDC(hdcBmp);
        }

        // Disegni sopra: ellisse con pennello e penna
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 128));
        HBRUSH hBrush = CreateHatchBrush(HS_CROSS, RGB(210, 240, 255));
        HGDIOBJ oldPen = SelectObject(hdcMem, hPen);
        HGDIOBJ oldBrush = SelectObject(hdcMem, hBrush);

        Ellipse(hdcMem, fX - fCircleRay, fY - fCircleRay, fX + fCircleRay, fY + fCircleRay);

        SelectObject(hdcMem, oldPen);
        SelectObject(hdcMem, oldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);

        // Copia tutto sul client
        BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

        // Pulizia
        SelectObject(hdcMem, oldBmpMem);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_TRAYICON:
        // Aggiunto: gestisce i click sull’icona della tray
        if (lParam == WM_LBUTTONDBLCLK)
        {
            Shell_NotifyIcon(NIM_DELETE, &nid); // rimuove l’icona, spostato in WM_DESTROY ...
            ShowWindow(hwnd, SW_SHOW);          // ripristina la finestra
            SetForegroundWindow(hwnd);
        }
        else if (lParam == WM_RBUTTONUP) // AGGIUNTO: click destro
        {
            HMENU hTrayMenu = CreatePopupMenu();
            AppendMenu(hTrayMenu, MF_STRING, IDM_OPEN_GUI, L"▶  Apri GUI");
            AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);

            HMENU hHelpMenu = CreatePopupMenu();
            AppendMenu(hHelpMenu, MF_STRING, IDM_WINVER, L"Versione OS");
            AppendMenu(hHelpMenu, MF_STRING, IDM_SYSINFO, L"Informazioni di Sistema");
            AppendMenu(hHelpMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, L"Circa");
            AppendMenu(hTrayMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"?");
            AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hTrayMenu, MF_STRING, IDM_FILE_EXIT, L"Esci da GraphTest");
            
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            PostMessage(hwnd, WM_NULL, 0, 0);

            DestroyMenu(hHelpMenu);
            DestroyMenu(hTrayMenu);
        }
        break;

    case WM_SYSCOMMAND:
        // Aggiunto: intercetta la minimizzazione per nascondere la finestra e mettere l’icona nella tray
        if ((wParam & 0xFFF0) == SC_MINIMIZE)
        {
            ShowWindow(hwnd, SW_HIDE); // nasconde la finestra
            Shell_NotifyIcon(NIM_ADD, &nid); // aggiunge l’icona di notifica
            return 0;
        }

        // Dopo aver gestito la parte che ti interessa, devi lasciare che Windows completi l’elaborazione predefinita richiamando DefWindowProc
        return DefWindowProc(hwnd, msg, wParam, lParam);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_FILE_EXIT:
            // PostQuitMessage(0);
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case IDM_TEMPDIR:
            GetEnvironmentVariableW(L"TEMP", szTempPath, MAX_PATH);

            retSE = ShellExecute(hwnd, L"open", szTempPath, NULL, NULL, SW_SHOWNORMAL);

            if ((INT_PTR)retSE <= 32) {
                MessageBox(hwnd, L"Comando non trovato!", L"%TEMP% not found ...", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_USRDIR:
            GetEnvironmentVariableW(L"USERPROFILE", szTempPath, MAX_PATH);

            retSE = ShellExecute(hwnd, L"open", szTempPath, NULL, NULL, SW_SHOWNORMAL);

            if ((INT_PTR)retSE <= 32) {
                MessageBox(hwnd, L"Comando non trovato!", L"%TEMP% not found ...", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_WINDIR:
            GetEnvironmentVariableW(L"WINDIR", szTempPath, MAX_PATH);

            retSE = ShellExecute(hwnd, L"open", szTempPath, NULL, NULL, SW_SHOWNORMAL);

            if ((INT_PTR)retSE <= 32) {
                MessageBox(hwnd, L"Comando non trovato!", L"%TEMP% not found ...", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_SYSINFO:
            retSE = ShellExecute(hwnd, L"open", L"msinfo32.exe", NULL, NULL, SW_SHOWNORMAL);
            if ((INT_PTR)retSE <= 32) {
                MessageBox(hwnd, L"Comando non trovato!", L"msinfo32.exe not found ...", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_WINVER:
            retSE = ShellExecute(hwnd, L"open", L"winver.exe", NULL, NULL, SW_SHOWNORMAL);
            if ((INT_PTR)retSE <= 32) {
                MessageBox(hwnd, L"Comando non trovato!", L"winver.exe not found ...", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_ALWAYS_ON_TOP:

            SetAlwaysOnTop(hwnd, !isTopMost);
            isTopMost = !isTopMost;

            MENUITEMINFO mii;
            ZeroMemory(&mii, sizeof(MENUITEMINFO)); // azzera la struttura
            mii.cbSize = sizeof(MENUITEMINFO);
            mii.fMask = MIIM_STATE;

            GetMenuItemInfo(GetMenu(hwnd), IDM_ALWAYS_ON_TOP, FALSE, &mii);

            if (mii.fState & MFS_CHECKED)
                CheckMenuItem(GetMenu(hwnd), IDM_ALWAYS_ON_TOP, MF_BYCOMMAND | MF_UNCHECKED);
            else
                CheckMenuItem(GetMenu(hwnd), IDM_ALWAYS_ON_TOP, MF_BYCOMMAND | MF_CHECKED);

            break;
        case IDM_HELP_ABOUT:
            MessageBox(hwnd, L"GraphTest v1.0 (x64) per Windows 10 e Windows 11\n\nRealizzato da Alessandro Favretto.\n\n\nApplicazione standalone realizzata in VC++ e WinAPI.",
                L"Informazioni su GraphTest\n",
                MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_OPEN_GUI:
            ShowWindow(hwnd, SW_SHOW);          // ripristina la finestra
            SetForegroundWindow(hwnd);
            break;
        }

        break;
    case WM_TIMER:
        if (wParam == IDT_CLOCK_TIMER) {
            // Intervallo in radianti (escludendo estremi)
            const float minDeg = 3.0f;
            const float maxDeg = 87.0f;
            const float minRad = minDeg * static_cast<float>(M_PI) / 180.0f;
            const float maxRad = maxDeg * static_cast<float>(M_PI) / 180.0f;

            int clientWidth = 0;
            int clientHeight = 0;

            if (fCircleRay < 15)
                bRayInc = TRUE;
            else if (fCircleRay > 125)
                bRayInc = FALSE;

            if (bRayInc)
                fCircleRay += 0.5f;
            else
                fCircleRay -= 0.5f;
                
            // Ottieni l'area client
            RECT rcClient;
            if (GetClientRect(hwnd, &rcClient))
            {
                clientWidth = rcClient.right - rcClient.left;
                clientHeight = rcClient.bottom - rcClient.top;
            }

            if (fY >= clientHeight-fCircleRay) {
                // Numero casuale in (0,1), ma evitando 0 e 1
                float rnd = (static_cast<float>(std::rand()) + 1.0f) / (RAND_MAX + 2.0f);

                // Scala sull’intervallo (min, max)
                float val = minRad + rnd * (maxRad - minRad);
                fDeltaX = std::cos(val);
                fDeltaY = std::sin(val);

                bSignY = FALSE;
            }
             
            if (fY <= fCircleRay) {
                // Numero casuale in (0,1), ma evitando 0 e 1
                float rnd = (static_cast<float>(std::rand()) + 1.0f) / (RAND_MAX + 2.0f);

                // Scala sull’intervallo (min, max)
                float val = minRad + rnd * (maxRad - minRad);
                fDeltaX = std::cos(val);
                fDeltaY = std::sin(val);

                bSignY = TRUE;
            }

            if (fX >= clientWidth-fCircleRay) {
                // Numero casuale in (0,1), ma evitando 0 e 1
                float rnd = (static_cast<float>(std::rand()) + 1.0f) / (RAND_MAX + 2.0f);

                // Scala sull’intervallo (min, max)
                float val = minRad + rnd * (maxRad - minRad);
                fDeltaX = std::cos(val);
                fDeltaY = std::sin(val);

                bSignX = FALSE;
            }

            if (fX <= fCircleRay) {
                // Numero casuale in (0,1), ma evitando 0 e 1
                float rnd = (static_cast<float>(std::rand()) + 1.0f) / (RAND_MAX + 2.0f);

                // Scala sull’intervallo (min, max)
                float val = minRad + rnd * (maxRad - minRad);
                fDeltaX = std::cos(val);
                fDeltaY = std::sin(val);

                bSignX = TRUE;
            }

            if(bSignX)
                fX += 3*fDeltaX;
            else
                fX -= 3*fDeltaX;

            if (bSignY)
                fY += 3*fDeltaY;
            else
                fY -= 3*fDeltaY;

            /*
            RECT rcInvalidate;
            rcInvalidate.left = LONG(fX - fCircleRay - 4);
            rcInvalidate.top = LONG(fY - fCircleRay - 4);
            rcInvalidate.right = LONG(fX + fCircleRay + 4);
            rcInvalidate.bottom = LONG(fY + fCircleRay + 4);

            InvalidateRect(hwnd, &rcInvalidate, FALSE);
            */

            InvalidateRect(hwnd, NULL, FALSE);  // forza il ridisegno senza cancellare la finesdtra non crea flickering
            //InvalidateRect(hwnd, NULL, TRUE); // forza il ridisegno cancellando tutta la finesdtra però crea flickering
            //Mai usare SendMessage(hWnd, WM_PAINT, (WPARAM)hdc, 0)
        }
        break;
    case WM_CLOSE:
    {
        int result = MessageBox(hwnd, L"Vuoi davvero uscire dall'applicazione?", L"Conferma uscita",
            MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);

        if (result == IDYES) {
            DestroyWindow(hwnd);
        }
        // se NO, ignora la chiusura
        return 0;
    }
    case WM_DESTROY:{
        NOTIFYICONDATA nid = { 0 };
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd = hwnd;
        nid.uID = TRAY_ICON_ID;
        Shell_NotifyIcon(NIM_DELETE, &nid); // Aggiunto: rimuove l’icona di notifica se presente
        
        // 3.Timer ogni
        KillTimer(hwnd, IDT_CLOCK_TIMER);

        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void SetAlwaysOnTop(HWND hwnd, BOOL enable)
{
    SetWindowPos(
        hwnd,
        enable ? HWND_TOPMOST : HWND_NOTOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE
    );
}

bool IsRunningAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;

    // Crea un SID per il gruppo Administrators
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin == TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1.Impedisce l'esecuzione multipla ...    
    hMutex = CreateMutexW(NULL, TRUE, L"GraphTest_Mutex_SingleInstance");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(NULL, L"Processo già in esecuzione.", L"Istanze multiple non permesse ...", MB_OK | MB_ICONEXCLAMATION);
        if(hMutex!=NULL) CloseHandle(hMutex);
        return 0;
    }

    // CARICO LA BITMAP DI SFONDO ...
    hBitmap = (HBITMAP) LoadImage(
        NULL,                // NULL = file esterno
        L"Background.bmp",     // percorso del BMP
        IMAGE_BITMAP,
        0, 0,                // usa dimensione originale
        LR_LOADFROMFILE
    );

    if (!hBitmap) {
        MessageBox(hwnd, L"Impossibile caricare il BMP!", L"Errore", MB_OK | MB_ICONERROR);
        return 0;
    }

    // GENERO UN SEED ...
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW); // obbligatorio per WNDCLASSEX
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GraphTest";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);

    // --- Aggiunta icona ---
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));    // icona grande (taskbar, Alt+Tab)
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));  // icona piccola (tray, titlebar)

    RegisterClassExW(&wc);

    // CENTRATURA DELLA FINESTRA SULLO SCHERMO ...
    // 
    // Dimensioni dello schermo
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calcolo posizione centrata
    int x = (screenWidth - 640) / 2;
    int y = (screenHeight - 480) / 2;

    isAppRunningAsAdmin = IsRunningAsAdmin();
    if(isAppRunningAsAdmin)
        hwnd = CreateWindowEx(
            0,
            L"GraphTest",
            L"GraphTest v.1.0 (x64) - {Administrator}",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            x, y,
            640, 480,
            NULL, NULL, hInstance, NULL
        );
    else
        hwnd = CreateWindowEx(
            0,
            L"GraphTest",
            L"GraphTest v.1.0 (x64)",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            x, y,
            640, 480,
            NULL, NULL, hInstance, NULL
        );

    // Aggiunto: inizializzazione struttura tray icon
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ICON_ID; // 1; Icona definita sopra
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    //nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // icona

    lstrcpy(nid.szTip, L"GraphTest - Doppio-click per ripristinare");

    // END

    // BEGIN: GESTIONE MENU A TENDINA ...
    // 
    HMENU hMenuBar = CreateMenu();      // menu principale (barra)

    HMENU hFileMenu = CreateMenu();     // menu File
    HMENU hWindowMenu = CreateMenu();   // menu Finestra
    HMENU hHelpMenu = CreateMenu();     // menu Help
    HMENU hGoTo = CreateMenu();         // Navigazione Cartelle Notevoli
    

    // Navigazione Cartelle Notevoli: Aggiunge voci al sotto-sottomenu ...
    AppendMenu(hGoTo, MF_STRING, IDM_TEMPDIR, L"Cartella TEMP");
    AppendMenu(hGoTo, MF_STRING, IDM_USRDIR, L"Cartella USER");
    AppendMenu(hGoTo, MF_STRING, IDM_WINDIR, L"Cartella OS");
    
    // Aggiunge i sottomenu al menu File
    AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hGoTo, L"Vai a ...");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"Esci");
    
    AppendMenu(hWindowMenu, MF_STRING, IDM_ALWAYS_ON_TOP, L"Sempre in primo piano");

    AppendMenu(hHelpMenu, MF_STRING, IDM_WINVER, L"Versione OS");
    AppendMenu(hHelpMenu, MF_STRING, IDM_SYSINFO, L"Informazioni di Sistema");
    AppendMenu(hHelpMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, L"Circa");
 
    // Aggiunge i sottomenu alla barra principale ...
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hFileMenu, L"&File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hWindowMenu, L"Fi&nestra");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR) hHelpMenu, L"&?");

    // Imposta la barra menu sulla finestra ...
    SetMenu(hwnd, hMenuBar);

    // END

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 2.Timer
    SetTimer(hwnd, IDT_CLOCK_TIMER, 40, NULL);

    // Finestra NON sulla TNR ...
    // PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    // Shell_NotifyIcon(NIM_ADD, &nid); // aggiunge l'icona nella tray

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //2.Impedisce l'esecuzione multipla ...
    if (hMutex != NULL) CloseHandle(hMutex);

    return (int)msg.wParam;
}