
#include <Windows.h>
#define ID_BTNMSG 10
#include "Resource.h"
#include <strsafe.h>
#include <process.h>
#include <winsock.h>
#define BUFFER_LEN 1024

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ChatDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void recvMsgThread(void*);
BOOL connectServer(SOCKET * p);
HWND hEditBox;
HWND hEditSendBox;
HANDLE hEvent;//自动重置事件，用于发送消息的线程
SOCKET sock;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    MSG          msg;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_CHAT), NULL, ChatDlgProc);
    return 0;
}
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC         hdc;
//    PAINTSTRUCT ps;
//    RECT        rect;
//    static HWND hwndScroll,hrect,hwndButton;
//    HINSTANCE      hInstance,ownInstance;
//    int            cxClient, cyClient;
//    static int      cyChar,scrvalue;
//
//    switch (message)
//    {
//    case WM_CREATE:
//        hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
//        ownInstance = ((LPCREATESTRUCT)lParam)->hInstance;
//        cyChar = HIWORD(GetDialogBaseUnits());
//        hrect = CreateWindow(TEXT("static"), NULL,
//            WS_CHILD | WS_VISIBLE | SS_WHITERECT,
//            0,0,0,0,hwnd,(HMENU)9,hInstance,NULL
//            );
//        hwndScroll = CreateWindow(TEXT("scrollbar"), NULL,
//            WS_CHILD | WS_VISIBLE |
//            WS_TABSTOP | SBS_VERT,
//            0, 0, 0, 0,
//            hwnd, (HMENU)2, hInstance, NULL);
//        hwndButton = CreateWindow(TEXT("button"), NULL,
//            WS_CHILD | WS_VISIBLE |
//            WS_TABSTOP,
//            0, 0, 0, 0,
//            hwnd, (HMENU)ID_BTNMSG, hInstance, NULL);
//        SetScrollRange(hwndScroll, SB_CTL, 0, 255, FALSE);
//        return 0;
//    case WM_SIZE:
//        cxClient = LOWORD(lParam);
//        cyClient = HIWORD(lParam);
//        MoveWindow(hrect, 0, 0, cxClient/2, cyClient, TRUE);
//        MoveWindow(hwndScroll, 0, 0, cxClient / 7, cyClient - 4 * cyChar, TRUE);
//        MoveWindow(hwndButton, cxClient / 7, 0, cxClient / 7, 4 * cyChar, TRUE);
//        SetWindowText(hwndButton, TEXT("显示消息窗口"));
//        SetFocus(hwnd);
//        return 0;
//    case WM_VSCROLL:
//        switch (LOWORD(wParam))
//        {
//        case SB_PAGEDOWN:
//            scrvalue += 1;
//            break;
//        case SB_PAGEUP:
//            scrvalue -= 1;
//            break;
//        case SB_THUMBTRACK:
//            scrvalue = HIWORD(wParam);
//        default:
//            break;
//        }
//        SetScrollPos(hwndScroll, SB_CTL, scrvalue, TRUE);
//        return 0;
//    case WM_COMMAND:
//        switch (LOWORD(wParam))
//        {
//        case ID_BTNMSG:
//            
//            break;
//        default:
//            break;
//        }
//        return 0;
//    case WM_PAINT:
//        hdc = BeginPaint(hwnd, &ps);
//
//        GetClientRect(hwnd, &rect);
//
//        //DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect,
//                  //DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;
//        //TextOut(hdc, 100, 0, TEXT("hello windows 10!"), strlen("hello windows 10!"));
//        EndPaint(hwnd, &ps);
//        return 0;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}

INT_PTR CALLBACK ChatDlgProc(HWND hDlg, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    TCHAR msgbuffer[BUFFER_LEN];
    TCHAR readyBuffer[BUFFER_LEN] = { L"我:" };
    char sockBuffer[BUFFER_LEN];
    switch (message)
    {
    case WM_INITDIALOG:
        hEditBox = GetDlgItem(hDlg, IDC_MSG);
        hEditSendBox = GetDlgItem(hDlg, IDC_SENDEDIT);
        //hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);//自动重置事件
        connectServer(&sock);
        _beginthread(recvMsgThread, BUFFER_LEN*2, &sock);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        case IDC_BTNSEND:
            memset(msgbuffer, 0, sizeof(msgbuffer));
            memset(sockBuffer, 0, sizeof(sockBuffer));
            GetDlgItemText(hDlg, IDC_SENDEDIT, msgbuffer, 1024);
            WideCharToMultiByte(CP_UTF8, 0, msgbuffer, -1, sockBuffer, BUFFER_LEN, NULL, NULL);
            lstrcat(readyBuffer,msgbuffer);
            lstrcat(readyBuffer, L"\n");
            SendMessage(hEditBox, EM_SETSEL, -2, -1);
            SendMessage(hEditBox, EM_REPLACESEL, true, (LPARAM)readyBuffer);
            SetWindowText(hEditSendBox, L"");
            send(sock, sockBuffer, BUFFER_LEN, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void recvMsgThread(void*arg) {
    //SOCKET sock = (SOCKET)((SOCKET*)arg);
    char msgBuffer[BUFFER_LEN];
    TCHAR readyBuffer[BUFFER_LEN];
    while (1) {
        memset(msgBuffer, 0, sizeof(msgBuffer));
        memset(readyBuffer, 0, BUFFER_LEN);
        recv(sock, msgBuffer, BUFFER_LEN,0);
        
        int len = MultiByteToWideChar(CP_UTF8, 0, msgBuffer, -1, readyBuffer, 0);
        MultiByteToWideChar(CP_UTF8, 0, msgBuffer, -1, readyBuffer, len);
        lstrcat(readyBuffer, L"\n");
        SendMessage(hEditBox, EM_SETSEL, -2, -1);
        SendMessage(hEditBox, EM_REPLACESEL, true, (LPARAM)readyBuffer);
        
    }
}

BOOL connectServer(SOCKET * arg) {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 0), &data)) {
        MessageBox(NULL, TEXT("socket startup error"), TEXT("error"), MB_OK);
        return false;
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8888);
    sa.sin_addr.S_un.S_addr = inet_addr("124.223.60.250");
    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        MessageBox(NULL, TEXT("connect error"), TEXT("error"), MB_OK);
        return FALSE;
    }
    *arg = sock;
}

//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC         hdc;
//    PAINTSTRUCT ps;
//    RECT        rect;
//    static HWND hwndScroll, hrect, hwndButton;
//    HINSTANCE      hInstance, ownInstance;
//    int            cxClient, cyClient;
//    static int      cyChar, scrvalue;
//
//    switch (message)
//    {
//    case WM_CREATE:
//        DialogBox(NULL, MAKEINTRESOURCE(IDD_CHAT), NULL, ChatDlgProc);
//        return 0;
//    case WM_SIZE:
//        
//        return 0;
//    case WM_COMMAND:
//        return 0;
//    case WM_PAINT:
//        hdc = BeginPaint(hwnd, &ps);
//
//        GetClientRect(hwnd, &rect);
//        EndPaint(hwnd, &ps);
//        return 0;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}