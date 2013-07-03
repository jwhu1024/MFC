#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <winsock.h>
#include "Chat1.h"

//Dialog procedures
BOOL CALLBACK DlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam);
BOOL CALLBACK ConnectDlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam);
BOOL CALLBACK ListenDlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam);
    
int TryConnect(long hostname, int PortNo);
int ListenOnPort(int PortNo);

char Title[] = "ChatMate";

HINSTANCE hInst = NULL;
HWND hwnd, hStatus;

SOCKET s;
SOCKADDR_IN from;
int fromlen = sizeof(from);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	  hInst = hInstance;

	  return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN),
   						NULL, DlgProc);

}

void GetTextandAddLine(char Line[], HWND hParent, int IDC)
{
    HWND hEdit = GetDlgItem(hParent, IDC);
	int nTxtLen = GetWindowTextLength(hEdit); // Get length of existing text
	SendMessage(hEdit, EM_SETSEL, nTxtLen, nTxtLen);	// move caret to end
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)Line);	    // append text
	SendMessage(hEdit, EM_SCROLLCARET, 0, 0);		// scroll to caret
} //End function   

BOOL CALLBACK DlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam)
{
switch(Message)
    {
    
    case WM_INITDIALOG:
    {
        //Our dialog box is being created
        hwnd = hdwnd;
        
        hStatus = GetDlgItem(hdwnd, ID_STATUS_MAIN);
    }
    return TRUE;
    
    //Winsock related message...
    case 1045:
        switch (lParam)
        {
                case FD_CONNECT: //Connected OK
                    MessageBeep(MB_OK);
                    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Connection Established."); 
                break;
                
                case FD_CLOSE: //Lost connection
                    MessageBeep(MB_ICONERROR);
                    
                    //Clean up
                    if (s) closesocket(s);
                    WSACleanup();
                    
                    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Connection to Remote Host Lost.");                
                break;
                
                case FD_READ: //Incoming data to receive
                    char buffer[80];
                    memset(buffer, 0, sizeof(buffer)); //Clear the buffer
                    
                    recv (s, buffer, sizeof(buffer)-1, 0); //Get the text
                    
                    GetTextandAddLine(buffer, hwnd, ID_EDIT_DATA); //Display it
                break;
                
                case FD_ACCEPT: //Connection request
                {
                    MessageBeep(MB_OK);
                    
                    SOCKET TempSock = accept(s, (struct sockaddr*)&from, &fromlen);
                    s = TempSock; //Switch our old socket to the new one
                    
                    char szAcceptAddr[100];
                    wsprintf(szAcceptAddr, "Connection from [%s] accepted.",
                      inet_ntoa(from.sin_addr)); 
                      
                    SendMessage(hStatus, SB_SETTEXT, 0, 
                      (LPARAM)szAcceptAddr);
                }
                break;
        }
    break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
                case ID_BTN_CONNECT:
                  return DialogBox(hInst, MAKEINTRESOURCE(DLG_CONNECT),
                   						NULL, ConnectDlgProc);
                break;

                case ID_BTN_LISTEN:
                  return DialogBox(hInst, MAKEINTRESOURCE(DLG_LISTEN),
                   						NULL, ListenDlgProc);                
                break;
                
                case ID_BTN_CLEAR: //Clear edit and disconnect
                {
                    if (s) //If there's a connection
                    {
                        int a = MessageBox(hdwnd, "Are you sure you want to end the current connection?",
                                        "End Connection", MB_ICONQUESTION | MB_YESNO);
                        if (a == IDYES)
                        {
                            SendDlgItemMessage(hdwnd, ID_EDIT_DATA, WM_SETTEXT, 0, (LPARAM)"");
                            closesocket(s); //Shut down socket
                            WSACleanup(); //Clean up Winsock
                        }
                    }
                }
                break;
                
                case ID_BTN_SEND: //Send data
                {
                    int len = GetWindowTextLength(GetDlgItem(hdwnd, ID_EDIT_SEND));
			        
          	        if (len) //If there's text in the reply box...
			        {
			            if (s)
			            {
			                char* Data;
			                Data = (char*)GlobalAlloc(GPTR, len + 1);
			                
			                GetDlgItemText(hdwnd, ID_EDIT_SEND, Data, len + 1);

			                //Add a carriage return and line feed before sending data
			                char szTemp[len+100];
			                
			                wsprintf(szTemp, "%s\r\n", Data);
			                
			                send(s, szTemp, len+strlen(szTemp)+1, 0); //Send the string
			                
			                SetDlgItemText(hdwnd, ID_EDIT_SEND, ""); //Reset the textbox
			                
			                wsprintf(szTemp, "[You said:] %s\r\n", Data);
			                GetTextandAddLine(szTemp, hdwnd, ID_EDIT_DATA); //Add this line to our textbox
			                
			                GlobalFree((HANDLE)Data); //Free the memory: Important!!
			            }
			            else
			            {
			            //We aren't connected!!
			            MessageBox(hwnd, "No established connection detected.",
			              Title, MB_ICONERROR | MB_OK);
			            }
			        }
                }
                break;
                
                case IDCANCEL:
                    //Clean up
                    if (s) closesocket(s);
                    WSACleanup();
                    
                    EndDialog(hdwnd, IDOK);
                break;
        } //End switch
        default:
            return FALSE;
    break;
    } //End Message switch
    return TRUE;
}

BOOL CALLBACK ConnectDlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam)
{
switch(Message)
    {
    case WM_INITDIALOG:
    {
        //Our dialog box is being created
    }
    return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
                case ID_BTN_GO:
                {
                    int len = GetWindowTextLength(GetDlgItem(hdwnd, ID_EDIT_HOST));
                    int lenport = GetWindowTextLength(GetDlgItem(hdwnd, ID_EDIT_PORT));
                    
                    if (!lenport) return 0; //Was the port specified?
                    
                    int portno = GetDlgItemInt(hdwnd, ID_EDIT_PORT, 0, 0);
                    
                    if (len)
                    {
                            char* Data;
                            Data = (char*)GlobalAlloc(GPTR, len + 1); //Allocate memory
                    
                            GetDlgItemText(hdwnd, ID_EDIT_HOST, Data, len + 1); //Get text into buffer
                    
                            if (!gethostbyname(Data))
                            {
                            //Couldn't get hostname; assume it's an IP Address
                                long hostname = inet_addr(Data);
                                if(!TryConnect(hostname, portno))
                                {
                                    MessageBox(hdwnd, "Could not connect to remote host.", Title, MB_ICONERROR | MB_OK);
                                    if (s) closesocket(s); //Shut down socket
                                }
                            }
                            
                            GlobalFree((HANDLE)Data); //Free memory
                            
                            EndDialog(hdwnd, IDOK);
                    }
                }
                break;

                case IDCANCEL:
                    EndDialog(hdwnd, IDOK);
                break;
        } //End switch
        default:
            return FALSE;
    break;
    } //End Message switch
    return TRUE;
}

BOOL CALLBACK ListenDlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam)
{
switch(Message)
    {
    case WM_INITDIALOG:
    {
        //Our dialog box is being created
    }
    return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
                case ID_BTN_GO:
                {
                    int lenport = GetWindowTextLength(GetDlgItem(hdwnd, ID_EDIT_PORT));
                    if (!lenport) return 0; //Was the port specified?
                    
                    int portno = GetDlgItemInt(hdwnd, ID_EDIT_PORT, 0, 0);
                    
                    if (!ListenOnPort(portno)) 
                    {
                        if (s) closesocket(s);
                        MessageBox(hdwnd, "Error listening on specified port.", Title, MB_ICONERROR | MB_OK);
                    }                                                            
                    EndDialog(hdwnd, IDOK);
                }
                break;

                case IDCANCEL:
                    EndDialog(hdwnd, IDOK);
                break;
        } //End switch
        default:
            return FALSE;
    break;
    } //End Message switch
    return TRUE;
}

int TryConnect(long hostname, int PortNo)
{
    WSADATA w; //Winsock startup info
    SOCKADDR_IN target; //Information about host
    
    int error = WSAStartup (0x0202, &w);   // Fill in WSA info
     
    if (error)
    { // there was an error
      return 0;
    }
    if (w.wVersion != 0x0202)
    { // wrong WinSock version!
      WSACleanup (); // unload ws2_32.dll
      return 0;
    }
    
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create socket
    if (s == INVALID_SOCKET)
    {
        return 0;
    }

    target.sin_family = AF_INET;           // address family Internet
    target.sin_port = htons (PortNo);        // set server’s port number
    target.sin_addr.s_addr = hostname;  // set server’s IP
     
    //Try connecting...
    if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR) //Try binding
    { // error
          return 0;
    }
    WSAAsyncSelect (s, hwnd, 1045, FD_READ | FD_CONNECT | FD_CLOSE); 
        //Switch to Non-Blocking mode
        
    SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"Connected to Remote Host.");
    
    return 1; //OK
}

int ListenOnPort(int PortNo)
{
    WSADATA w;
    
    int error = WSAStartup (0x0202, &w);   // Fill in WSA info
     
    if (error)
    { // there was an error
        SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"Could not initialize Winsock.");
      return 0;
    }
    if (w.wVersion != 0x0202)
    { // wrong WinSock version!
      WSACleanup (); // unload ws2_32.dll
      SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"Wrong Winsock version.");
      return 0;
    }
    
    SOCKADDR_IN addr; // the address structure for a TCP socket
    SOCKET client; //The connected socket handle
    
    addr.sin_family = AF_INET;      // Address family Internet
    addr.sin_port = htons (PortNo);   // Assign port to this socket
    addr.sin_addr.s_addr = htonl (INADDR_ANY);   // No destination
    
    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create socket
    
    if (s == INVALID_SOCKET)
    {
        SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"Could not create socket.");
        return 0;
    }
    
    if (bind(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) //Try binding
    { // error
        SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"Could not bind to IP.");
        return 0;
    }
    
    listen(s, 10); //Start listening
    WSAAsyncSelect (s, hwnd, 1045, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT); //Switch to Non-Blocking mode
    
    char szTemp[100];
    wsprintf(szTemp, "Listening on port %d...", PortNo);
    
    SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)szTemp);  
    return 1;
}


