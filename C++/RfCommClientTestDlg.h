// RfCommClientTestDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <list>
#include "Bluetooth\wclBluetooth.h"

using namespace wclCommon;
using namespace wclCommunication;
using namespace wclBluetooth;


// CRfCommClientTestDlg dialog
class CRfCommClientTestDlg : public CDialog
{
// Construction
public:
	CRfCommClientTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RFCOMMCLIENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CButton cbUnpair;
	CListBox lbLog;

	typedef std::list<CwclRfCommClient*> CLIENTS_LIST;

	CwclBluetoothManager*	FManager;
	CLIENTS_LIST*			FClients;
	CLIENTS_LIST::iterator	FCurDevice;
	int						FCurDeviceNum;

	CwclBluetoothRadio* GetRadio();
	void DisconnectClients();
	void ConnectToNextDevice();
	void ConnectToCurrentDevice();

	void ManagerDiscoveringCompleted(void* Sender, CwclBluetoothRadio* const Radio,
		const int Error);
	void ManagerDeviceFound(void* Sender, CwclBluetoothRadio* const Radio,
		__int64 Address);
	void ManagerDiscoveringStarted(void* Sender, CwclBluetoothRadio* const Radio);
	void ManagerClosed(void* Sender);
	void ManagerAfterOpen(void* Sender);
	void ManagerPinRequest(void* Sender, CwclBluetoothRadio* const Radio,
		const __int64 Address, tstring& Pin);
	void ManagerPasskeyRequest(void* Sender, CwclBluetoothRadio* const Radio,
		const __int64 Address, unsigned long& Passkey);
	void ManagerPasskeyNotification(void* Sender, CwclBluetoothRadio* const Radio,
		const __int64 Address, const unsigned long Passkey);
	void ManagerNumericComparison(void* Sender, CwclBluetoothRadio* const Radio,
		const __int64 Address, unsigned long Number, bool& Boolean);
	void ManagerConfirm(void* Sender, CwclBluetoothRadio* const Radio,
		const __int64 Address, bool& Confirm);
	void ManagerAuthenticationCompleted(void* Sender, CwclBluetoothRadio* const Radio,
		const __int64 Address, const int Error);

	void ClientData(void* Sender, const void* const Data, const unsigned long Size);
	void ClientDisconnect(void* Sender, const int Reason);
	void ClientConnect(void* Sender, const int Error);
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonDiscover();
	afx_msg void OnBnClickedButtonClear();
};
