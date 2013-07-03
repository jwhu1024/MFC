; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CChildView
LastTemplate=CRichEditCtrl
NewFileInclude1=#include "StdAfx.h"
NewFileInclude2=#include "SerialTestMFC.h"
LastPage=0

ClassCount=10
Class1=CTestApp
Class2=CChildView
Class3=CMainFrame
Class4=CChildFrame
Class5=CAboutDlg
Class6=CPageComEvents
Class7=CPageComSettings
Class8=CSheetSettings
Class9=CComSelectDlg
Class10=CRichCommEdit

ResourceCount=12
Resource1=IDR_SERIALTYPE
Resource2=IDD_COM_SETTINGS
Resource3=IDD_COM_EVENTS
Resource4=IDD_COM_SETTINGS (English (U.S.))
Resource5=IDD_COM_EVENTS (English (U.S.))
Resource6=IDD_ABOUTBOX
Resource7=IDR_MAINFRAME
Resource8=IDR_SERIALTYPE (English (U.S.))
Resource9=IDD_ABOUTBOX (English (U.S.))
Resource10=IDD_COM_SELECT
Resource11=IDR_MAINFRAME (English (U.S.))
Resource12=IDD_COM_SELECT (English (U.S.))

[CLS:CTestApp]
Type=0
HeaderFile=SerialTestMFC.h
ImplementationFile=SerialTestMFC.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CTestApp

[CLS:CChildView]
Type=0
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=W
BaseClass=CWnd
VirtualFilter=WC
LastObject=CChildView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame

[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
BaseClass=CMDIChildWnd
VirtualFilter=mfWC
LastObject=ID_COM_OPEN

[CLS:CAboutDlg]
Type=0
HeaderFile=AboutDlg.h
ImplementationFile=AboutDlg.cpp
Filter=D
LastObject=CAboutDlg
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDI_ABOUT_ICON,static,1342177283
Control2=IDC_ABOUT_TITLE,static,1342308480
Control3=IDC_ABOUT_COPYRIGHT,static,1342308480
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,button,1342177287

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_COM_OPEN
Command2=ID_APP_EXIT
Command3=ID_APP_ABOUT
CommandCount=3

[MNU:IDR_SERIALTYPE]
Type=1
Class=CSerialTestMFCView
Command1=ID_COM_OPEN
Command2=ID_COM_CLOSE
Command3=ID_COM_SETTINGS
Command4=ID_APP_EXIT
Command5=ID_WINDOW_CASCADE
Command6=ID_WINDOW_TILE_HORZ
Command7=ID_WINDOW_ARRANGE
Command8=ID_APP_ABOUT
Command9=ID_COMMANDS_SENDFILE
Command10=ID_COMMANDS_SENDBREAK
CommandCount=10

[DLG:IDD_COM_SETTINGS]
Type=1
Class=CPageComSettings
ControlCount=29
Control1=IDC_BAUDRATE,button,1342308359
Control2=IDC_BAUDRATE_1200,button,1342373897
Control3=IDC_BAUDRATE_2400,button,1342177289
Control4=IDC_BAUDRATE_9600,button,1342177289
Control5=IDC_BAUDRATE_14400,button,1342177289
Control6=IDC_BAUDRATE_19200,button,1342177289
Control7=IDC_BAUDRATE_38400,button,1342177289
Control8=IDC_BAUDRATE_56000,button,1342177289
Control9=IDC_BAUDRATE_57600,button,1342177289
Control10=IDC_BAUDRATE_115200,button,1342177289
Control11=IDC_DATA,button,1342308359
Control12=IDC_DATA_5,button,1342373897
Control13=IDC_DATA_6,button,1342177289
Control14=IDC_DATA_7,button,1342177289
Control15=IDC_DATA_8,button,1342177289
Control16=IDC_PARITY,button,1342308359
Control17=IDC_PARITY_NONE,button,1342373897
Control18=IDC_PARITY_ODD,button,1342177289
Control19=IDC_PARITY_EVEN,button,1342177289
Control20=IDC_PARITY_MARK,button,1342177289
Control21=IDC_PARITY_SPACE,button,1342177289
Control22=IDC_STOP,button,1342308359
Control23=IDC_STOP_1,button,1342373897
Control24=IDC_STOP_15,button,1342177289
Control25=IDC_STOP_2,button,1342177289
Control26=IDC_HANDSHAKING,button,1342308359
Control27=IDC_HANDSHAKING_OFF,button,1342373897
Control28=IDC_HANDSHAKING_SOFTWARE,button,1342177289
Control29=IDC_HANDSHAKING_HARDWARE,button,1342177289

[DLG:IDD_COM_EVENTS]
Type=1
Class=CPageComEvents
ControlCount=13
Control1=IDC_EVENTS,button,1342308359
Control2=IDC_EVENT_BREAK,button,1342242819
Control3=IDC_EVENT_CTS,button,1342242819
Control4=IDC_EVENT_DSR,button,1342242819
Control5=IDC_EVENT_ERROR,button,1342242819
Control6=IDC_EVENT_RING,button,1342242819
Control7=IDC_EVENT_RLSD,button,1342242819
Control8=IDC_EVENT_RECV,button,1342242819
Control9=IDC_EVENT_RCV_EV,button,1342242819
Control10=IDC_EVENT_SEND,button,1342242819
Control11=IDC_EVENT_CHAR,button,1342308359
Control12=IDC_EVENT_CHAR_TEXT,static,1342177280
Control13=IDC_EVENT_CHAR_VALUE,edit,1350639744

[CLS:CPageComEvents]
Type=0
HeaderFile=PageComEvents.h
ImplementationFile=PageComEvents.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CPageComEvents
VirtualFilter=idWC

[CLS:CPageComSettings]
Type=0
HeaderFile=PageComSettings.h
ImplementationFile=PageComSettings.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CPageComSettings
VirtualFilter=idWC

[CLS:CSheetSettings]
Type=0
HeaderFile=SheetSettings.h
ImplementationFile=SheetSettings.cpp
BaseClass=CPropertySheet
Filter=W
VirtualFilter=hWC

[DLG:IDD_COM_SELECT]
Type=1
Class=CComSelectDlg
ControlCount=6
Control1=IDC_STATIC,button,1342177287
Control2=IDC_COM1,button,1342373897
Control3=IDC_COM2,button,1342177289
Control4=IDC_COM3,button,1342177289
Control5=IDC_COM4,button,1342177289
Control6=IDOK,button,1342373889

[CLS:CComSelectDlg]
Type=0
HeaderFile=ComSelectDlg.h
ImplementationFile=ComSelectDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CComSelectDlg

[CLS:CRichCommEdit]
Type=0
HeaderFile=RichCommEdit.h
ImplementationFile=RichCommEdit.cpp
BaseClass=CRichEditCtrl
Filter=W
LastObject=CRichCommEdit
VirtualFilter=WC

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDI_ABOUT_ICON,static,1342177283
Control2=IDC_ABOUT_TITLE,static,1342308480
Control3=IDC_ABOUT_COPYRIGHT,static,1342308480
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,button,1342177287

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_COM_OPEN
Command2=ID_APP_EXIT
Command3=ID_APP_ABOUT
CommandCount=3

[MNU:IDR_SERIALTYPE (English (U.S.))]
Type=1
Class=CSerialTestMFCView
Command1=ID_COM_OPEN
Command2=ID_COM_CLOSE
Command3=ID_COM_SETTINGS
Command4=ID_APP_EXIT
Command5=ID_WINDOW_CASCADE
Command6=ID_WINDOW_TILE_HORZ
Command7=ID_WINDOW_ARRANGE
Command8=ID_APP_ABOUT
Command9=ID_COMMANDS_SENDFILE
Command10=ID_COMMANDS_SENDBREAK
CommandCount=10

[DLG:IDD_COM_SETTINGS (English (U.S.))]
Type=1
Class=?
ControlCount=29
Control1=IDC_BAUDRATE,button,1342308359
Control2=IDC_BAUDRATE_1200,button,1342373897
Control3=IDC_BAUDRATE_2400,button,1342177289
Control4=IDC_BAUDRATE_9600,button,1342177289
Control5=IDC_BAUDRATE_14400,button,1342177289
Control6=IDC_BAUDRATE_19200,button,1342177289
Control7=IDC_BAUDRATE_38400,button,1342177289
Control8=IDC_BAUDRATE_56000,button,1342177289
Control9=IDC_BAUDRATE_57600,button,1342177289
Control10=IDC_BAUDRATE_115200,button,1342177289
Control11=IDC_DATA,button,1342308359
Control12=IDC_DATA_5,button,1342373897
Control13=IDC_DATA_6,button,1342177289
Control14=IDC_DATA_7,button,1342177289
Control15=IDC_DATA_8,button,1342177289
Control16=IDC_PARITY,button,1342308359
Control17=IDC_PARITY_NONE,button,1342373897
Control18=IDC_PARITY_ODD,button,1342177289
Control19=IDC_PARITY_EVEN,button,1342177289
Control20=IDC_PARITY_MARK,button,1342177289
Control21=IDC_PARITY_SPACE,button,1342177289
Control22=IDC_STOP,button,1342308359
Control23=IDC_STOP_1,button,1342373897
Control24=IDC_STOP_15,button,1342177289
Control25=IDC_STOP_2,button,1342177289
Control26=IDC_HANDSHAKING,button,1342308359
Control27=IDC_HANDSHAKING_OFF,button,1342373897
Control28=IDC_HANDSHAKING_SOFTWARE,button,1342177289
Control29=IDC_HANDSHAKING_HARDWARE,button,1342177289

[DLG:IDD_COM_EVENTS (English (U.S.))]
Type=1
Class=?
ControlCount=13
Control1=IDC_EVENTS,button,1342308359
Control2=IDC_EVENT_BREAK,button,1342242819
Control3=IDC_EVENT_CTS,button,1342242819
Control4=IDC_EVENT_DSR,button,1342242819
Control5=IDC_EVENT_ERROR,button,1342242819
Control6=IDC_EVENT_RING,button,1342242819
Control7=IDC_EVENT_RLSD,button,1342242819
Control8=IDC_EVENT_RECV,button,1342242819
Control9=IDC_EVENT_RCV_EV,button,1342242819
Control10=IDC_EVENT_SEND,button,1342242819
Control11=IDC_EVENT_CHAR,button,1342308359
Control12=IDC_EVENT_CHAR_TEXT,static,1342177280
Control13=IDC_EVENT_CHAR_VALUE,edit,1350639744

[DLG:IDD_COM_SELECT (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDC_STATIC,button,1342177287
Control2=IDC_COM1,button,1342373897
Control3=IDC_COM2,button,1342177289
Control4=IDC_COM3,button,1342177289
Control5=IDC_COM4,button,1342177289
Control6=IDOK,button,1342373889

