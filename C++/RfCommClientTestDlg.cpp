// RfCommClientTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RfCommClientTest.h"
#include "RfCommClientTestDlg.h"

#include "Bluetooth\wclUUIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRfCommClientTestDlg dialog


CString IntToHex(const int i)
{
	CString s;
	s.Format(_T("%.8X"), i);
	return s;
}

CString IntToStr(const int i)
{
	CString s;
	s.Format(_T("%d"), i);
	return s;
}

CString IntToHex(const __int64 i)
{
	CString s;
	s.Format(_T("%.4X%.8X"), static_cast<INT32>((i >> 32) & 0x00000FFFF),
		static_cast<INT32>(i) & 0xFFFFFFFF);
	return s;
}



CRfCommClientTestDlg::CRfCommClientTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRfCommClientTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRfCommClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_UNPAIR, cbUnpair);
	DDX_Control(pDX, IDC_LIST_LOG, lbLog);
}

BEGIN_MESSAGE_MAP(CRfCommClientTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_DISCOVER, &CRfCommClientTestDlg::OnBnClickedButtonDiscover)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CRfCommClientTestDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CRfCommClientTestDlg message handlers

BOOL CRfCommClientTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	FManager = new CwclBluetoothManager();
	__hook(&CwclBluetoothManager::AfterOpen, FManager, &CRfCommClientTestDlg::ManagerAfterOpen);
	__hook(&CwclBluetoothManager::OnClosed, FManager, &CRfCommClientTestDlg::ManagerClosed);

	__hook(&CwclBluetoothManager::OnAuthenticationCompleted, FManager, &CRfCommClientTestDlg::ManagerAuthenticationCompleted);
	__hook(&CwclBluetoothManager::OnConfirm, FManager, &CRfCommClientTestDlg::ManagerConfirm);
	__hook(&CwclBluetoothManager::OnNumericComparison, FManager, &CRfCommClientTestDlg::ManagerNumericComparison);
	__hook(&CwclBluetoothManager::OnPasskeyNotification, FManager, &CRfCommClientTestDlg::ManagerPasskeyNotification);
	__hook(&CwclBluetoothManager::OnPasskeyRequest, FManager, &CRfCommClientTestDlg::ManagerPasskeyRequest);
	__hook(&CwclBluetoothManager::OnPinRequest, FManager, &CRfCommClientTestDlg::ManagerPinRequest);
	
	__hook(&CwclBluetoothManager::OnDiscoveringStarted, FManager, &CRfCommClientTestDlg::ManagerDiscoveringStarted);
	__hook(&CwclBluetoothManager::OnDeviceFound, FManager, &CRfCommClientTestDlg::ManagerDeviceFound);
	__hook(&CwclBluetoothManager::OnDiscoveringCompleted, FManager, &CRfCommClientTestDlg::ManagerDiscoveringCompleted);
	
	FClients = new CLIENTS_LIST();
	
	int Res = FManager->Open();
	if (Res != WCL_E_SUCCESS)
		lbLog.AddString(_T("Bluetooth Manager open failed: 0x") + IntToHex(Res));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRfCommClientTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRfCommClientTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CwclBluetoothRadio* CRfCommClientTestDlg::GetRadio()
{
	CwclBluetoothRadio* Radio;
	int Res = FManager->GetClassicRadio(Radio);
	if (Res == WCL_E_SUCCESS)
		return Radio;
	
	lbLog.AddString(_T("Unable to get Classic Radio: 0x") + IntToHex(Res));
	return NULL;
}

void CRfCommClientTestDlg::DisconnectClients()
{
	while (FClients->size() > 0)
	{
		CwclRfCommClient* Client = (*FClients->begin());
		if (Client->State != csDisconnected)
			Client->Disconnect();
		else
			FClients->erase(FClients->begin());
		__unhook(Client);
		delete Client;
	}
	FClients->clear();
}

void CRfCommClientTestDlg::ConnectToNextDevice()
{
	FCurDevice++;
	FCurDeviceNum++;
	lbLog.AddString(_T("Processing next client: ") + IntToStr(FCurDeviceNum));
	if (FCurDevice != FClients->end())
		ConnectToCurrentDevice();
	else
		lbLog.AddString(_T("All clients processed"));
}

void CRfCommClientTestDlg::ConnectToCurrentDevice()
{
	lbLog.AddString(_T("Try to connect to [") + IntToHex((*FCurDevice)->Address) + _T("]"));
	CwclBluetoothRadio* Radio = GetRadio();
	if (Radio != NULL)
	{
		int Res;
		if (cbUnpair.GetCheck())
		{
			lbLog.AddString(_T("Try to unpair"));
			Res = Radio->RemoteUnpair((*FCurDevice)->Address);
			lbLog.AddString(_T("Unpair result: 0x") + IntToHex(Res));
		}
		
		Res = (*FCurDevice)->Connect(Radio);
		if (Res != WCL_E_SUCCESS)
		{
			lbLog.AddString(_T("Connect failed: 0x") + IntToHex(Res));
			ConnectToNextDevice();
		}
	}
}

void CRfCommClientTestDlg::ManagerDiscoveringCompleted(void* Sender,
	CwclBluetoothRadio* const Radio, const int Error)
{
	lbLog.AddString(_T("Discovering completed with result: 0x") + IntToHex(Error));
	lbLog.AddString(_T("Found ") + IntToStr((int)FClients->size()) + _T(" devices"));
	if (FClients->size() > 0)
	{
		FCurDevice = FClients->begin();
		FCurDeviceNum = 0;
		ConnectToCurrentDevice();
	}
}

void CRfCommClientTestDlg::ManagerDeviceFound(void* Sender, CwclBluetoothRadio* const Radio,
	__int64 Address)
{
	lbLog.AddString(_T("Device found [") + IntToHex(Address) + _T("]"));
	
	CwclRfCommClient* Client = new CwclRfCommClient();
	Client->Address = Address;
	Client->Service = SerialPortServiceClass_UUID;
	
	__hook(&CwclRfCommClient::OnConnect, Client, &CRfCommClientTestDlg::ClientConnect);
	__hook(&CwclRfCommClient::OnDisconnect, Client, &CRfCommClientTestDlg::ClientDisconnect);
	__hook(&CwclRfCommClient::OnData, Client, &CRfCommClientTestDlg::ClientData);
	
	FClients->push_back(Client);
}

void CRfCommClientTestDlg::ClientData(void* Sender, const void* const Data,
	const unsigned long Size)
{
	if (Size > 0)
	{
		CStringA Str((PCHAR)Data, (int)Size);
		CString s(Str);
		lbLog.AddString(_T("Received: ") + s);
	}
}

void CRfCommClientTestDlg::ClientDisconnect(void* Sender, const int Reason)
{
	CString Address = IntToHex(((CwclRfCommClient*)Sender)->Address);
	lbLog.AddString(_T("Client [") + Address + _T("] disconnected with reason: 0x") + IntToHex(Reason));
	FClients->remove((CwclRfCommClient*)Sender);
}

void CRfCommClientTestDlg::ClientConnect(void* Sender, const int Error)
{
	CString Address = IntToHex(((CwclRfCommClient*)Sender)->Address);
	if (Error != WCL_E_SUCCESS)
		lbLog.AddString(_T("Client [") + Address + _T("] connect failed: 0x") + IntToHex(Error));
	else
		lbLog.AddString(_T("Client [") + Address + _T("] connected"));
	ConnectToNextDevice();
}

void CRfCommClientTestDlg::ManagerDiscoveringStarted(void* Sender, CwclBluetoothRadio* const Radio)
{
	lbLog.AddString(_T("Discovering started"));
	DisconnectClients();
}

void CRfCommClientTestDlg::ManagerClosed(void* Sender)
{
	lbLog.AddString(_T("Bluetooth Manager closed"));
}

void CRfCommClientTestDlg::ManagerAfterOpen(void* Sender)
{
	lbLog.AddString(_T("Bluetooth Manager opened"));
}

void CRfCommClientTestDlg::ManagerPinRequest(void* Sender, CwclBluetoothRadio* const Radio,
	const __int64 Address, tstring& Pin)
{
	lbLog.AddString(_T("PIN pairing with device [") + IntToHex(Address) + _T("]"));
	Pin = _T("0000");
}

void CRfCommClientTestDlg::ManagerPasskeyRequest(void* Sender, CwclBluetoothRadio* const Radio,
	const __int64 Address, unsigned long& Passkey)
{
	lbLog.AddString(_T("Passkey request pairing with device [") + IntToHex(Address) +
		_T("]"));
	Passkey = 1234;
}

void CRfCommClientTestDlg::ManagerPasskeyNotification(void* Sender, CwclBluetoothRadio* const Radio,
	const __int64 Address, const unsigned long Passkey)
{
	lbLog.AddString(_T("Passkey notification pairing with device [") + IntToHex(Address) +
		_T("]: ") + IntToStr(Passkey));
}

void CRfCommClientTestDlg::ManagerNumericComparison(void* Sender, CwclBluetoothRadio* const Radio,
	const __int64 Address, unsigned long Number, bool& Confirm)
{
	lbLog.AddString(_T("Numeric comparison with device [") + IntToHex(Address) + _T("]: ") +
		IntToStr(Number));
	Confirm = true;
}

void CRfCommClientTestDlg::ManagerConfirm(void* Sender, CwclBluetoothRadio* const Radio,
	const __int64 Address, bool& Confirm)
{
	lbLog.AddString(_T("Just Works pairing with device [") + IntToHex(Address) + _T("]"));
	Confirm = true;
}

void CRfCommClientTestDlg::ManagerAuthenticationCompleted(void* Sender,
	CwclBluetoothRadio* const Radio, const __int64 Address, const int Error)
{
	lbLog.AddString(_T("Authentication with device [") + IntToHex(Address) +
		_T("] completed with result: 0x") + IntToHex(Error));
}
void CRfCommClientTestDlg::OnDestroy()
{
	CDialog::OnDestroy();

	DisconnectClients();
	FManager->Close();

	delete FManager;
	delete FClients;
}

void CRfCommClientTestDlg::OnBnClickedButtonDiscover()
{
	CwclBluetoothRadio* Radio = GetRadio();
	if (Radio != NULL)
	{
		int Res = Radio->Discover(10, dkClassic);
		if (Res != WCL_E_SUCCESS)
			lbLog.AddString(_T("Start discovering failed: 0x") + IntToHex(Res));
	}
}

void CRfCommClientTestDlg::OnBnClickedButtonClear()
{
	lbLog.ResetContent();
}
