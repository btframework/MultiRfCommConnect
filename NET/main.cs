using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;

using wclCommon;
using wclBluetooth;

namespace RfCommClientTest
{
    public partial class fmMain : Form
    {
        private wclBluetoothManager FManager;
        private List<wclRfCommClient> FClients;
        private Int32 FCurDevice;

        private wclBluetoothRadio GetRadio()
        {
            wclBluetoothRadio Radio;
            Int32 Res = FManager.GetClassicRadio(out Radio);
            if (Res == wclErrors.WCL_E_SUCCESS)
                return Radio;

            lbLog.Items.Add("Unable to get Classic Radio: 0x" + Res.ToString("X8"));
            return null;
        }

        private void DisconnectClients()
        {
            while (FClients.Count > 0)
            {
                if (FClients[0].State != wclCommunication.wclClientState.csDisconnected)
                    FClients[0].Disconnect();
                else
                    FClients.RemoveAt(0);
            }
            FClients.Clear();
        }

        private void ConnectToNextDevice()
        {
            FCurDevice++;
            lbLog.Items.Add("Processing next client: " + FCurDevice.ToString());
            if (FCurDevice < FClients.Count)
                ConnectToCurrentDevice();
            else
                lbLog.Items.Add("All clients processed");
        }

        private void ConnectToCurrentDevice()
        {
            lbLog.Items.Add("Try to connect to [" + FClients[FCurDevice].Address.ToString("X12") + "]");
            wclBluetoothRadio Radio = GetRadio();
            if (Radio != null)
            {
                Int32 Res;
                if (cbUnpair.Checked)
                {
                    lbLog.Items.Add("Try to unpair");
                    Res = Radio.RemoteUnpair(FClients[FCurDevice].Address);
                    lbLog.Items.Add("Unpair result: 0x" + Res.ToString("X8"));
                }

                Res = FClients[FCurDevice].Connect(Radio);
                if (Res != wclErrors.WCL_E_SUCCESS)
                {
                    lbLog.Items.Add("Connect failed: 0x" + Res.ToString("X8"));
                    ConnectToNextDevice();
                }
            }
        }

        public fmMain()
        {
            InitializeComponent();
        }

        private void Form1_Load(Object sender, EventArgs e)
        {
            FManager = new wclBluetoothManager();
            FManager.AfterOpen += ManagerAfterOpen;
            FManager.OnClosed += ManagerClosed;

            FManager.OnAuthenticationCompleted += ManagerAuthenticationCompleted;
            FManager.OnConfirm += ManagerConfirm;
            FManager.OnNumericComparison += ManagerNumericComparison;
            FManager.OnPasskeyNotification += ManagerPasskeyNotification;
            FManager.OnPasskeyRequest += ManagerPasskeyRequest;
            FManager.OnPinRequest += ManagerPinRequest;

            FManager.OnDiscoveringStarted += ManagerDiscoveringStarted;
            FManager.OnDeviceFound += ManagerDeviceFound;
            FManager.OnDiscoveringCompleted += ManagerDiscoveringCompleted;

            FClients = new List<wclRfCommClient>();

            Int32 Res = FManager.Open();
            if (Res != wclErrors.WCL_E_SUCCESS)
                lbLog.Items.Add("Bluetooth Manager open failed: 0x" + Res.ToString("X8"));
        }

        private void ManagerDiscoveringCompleted(Object Sender, wclBluetoothRadio Radio, Int32 Error)
        {
            lbLog.Items.Add("Discovering completed with result: 0x" + Error.ToString("X8"));
            lbLog.Items.Add("Found " + FClients.Count.ToString() + " devices");
            if (FClients.Count > 0)
            {
                FCurDevice = 0;
                ConnectToCurrentDevice();
            }
        }

        private void ManagerDeviceFound(Object Sender, wclBluetoothRadio Radio, Int64 Address)
        {
            lbLog.Items.Add("Device found [" + Address.ToString("X12") + "]");
            
            wclRfCommClient Client = new wclRfCommClient();
            Client.Address = Address;
            Client.Service = wclUUIDs.SerialPortServiceClass_UUID;

            Client.OnConnect += ClientConnect;
            Client.OnDisconnect += ClientDisconnect;
            Client.OnData += ClientData;

            FClients.Add(Client);
        }

        private void ClientData(Object Sender, Byte[] Data)
        {
            if (Data != null && Data.Length > 0)
            {
                String Str = Encoding.ASCII.GetString(Data);
                lbLog.Items.Add("Received: " + Str);
            }
        }

        private void ClientDisconnect(Object Sender, Int32 Reason)
        {
            String Address = ((wclRfCommClient)Sender).Address.ToString("X12");
            lbLog.Items.Add("Client [" + Address + "] disconnected with reason: 0x" + Reason.ToString("X8"));
            FClients.Remove(((wclRfCommClient)Sender));
        }

        private void ClientConnect(Object Sender, Int32 Error)
        {
            String Address = ((wclRfCommClient)Sender).Address.ToString("X12");
            if (Error != wclErrors.WCL_E_SUCCESS)
                lbLog.Items.Add("Client [" + Address + "] connect failed: 0x" + Error.ToString("X8"));
            else
                lbLog.Items.Add("Client [" + Address + "] connected");
            ConnectToNextDevice();
        }

        private void ManagerDiscoveringStarted(Object Sender, wclBluetoothRadio Radio)
        {
            lbLog.Items.Add("Discovering started");
            DisconnectClients();
        }

        private void ManagerClosed(Object sender, EventArgs e)
        {
            lbLog.Items.Add("Bluetooth Manager closed");
        }

        private void ManagerAfterOpen(Object sender, EventArgs e)
        {
            lbLog.Items.Add("Bluetooth Manager opened");
        }

        private void ManagerPinRequest(Object Sender, wclBluetoothRadio Radio, Int64 Address,
            out String Pin)
        {
            lbLog.Items.Add("PIN pairing with device [" + Address.ToString("X12") + "]");
            Pin = "0000";
        }

        private void ManagerPasskeyRequest(Object Sender, wclBluetoothRadio Radio, Int64 Address,
            out UInt32 Passkey)
        {
            lbLog.Items.Add("Passkey request pairing with device [" + Address.ToString("X12") +
                "]");
            Passkey = 1234;
        }

        private void ManagerPasskeyNotification(Object Sender, wclBluetoothRadio Radio,
            Int64 Address, UInt32 Passkey)
        {
            lbLog.Items.Add("Passkey notification pairing with device [" + Address.ToString("X12") +
                "]: " + Passkey.ToString());
        }

        private void ManagerNumericComparison(Object Sender, wclBluetoothRadio Radio, Int64 Address,
            UInt32 Number, out Boolean Confirm)
        {
            lbLog.Items.Add("Numeric comparison with device [" + Address.ToString("X12") + "]: " +
                Number.ToString());
            Confirm = true;
        }

        private void ManagerConfirm(Object Sender, wclBluetoothRadio Radio, Int64 Address,
            out Boolean Confirm)
        {
            lbLog.Items.Add("Just Works pairing with device [" + Address.ToString("X12") + "]");
            Confirm = true;
        }

        private void ManagerAuthenticationCompleted(Object Sender, wclBluetoothRadio Radio,
            Int64 Address, Int32 Error)
        {
            lbLog.Items.Add("Authentication with device [" + Address.ToString("X12") +
                "] completed with result: 0x" + Error.ToString("X8"));
        }

        private void Form1_FormClosed(Object sender, FormClosedEventArgs e)
        {
            DisconnectClients();
            FManager.Close();
        }

        private void btDiscover_Click(object sender, EventArgs e)
        {
            wclBluetoothRadio Radio = GetRadio();
            if (Radio != null)
            {
                Int32 Res = Radio.Discover(10, wclBluetoothDiscoverKind.dkClassic);
                if (Res != wclErrors.WCL_E_SUCCESS)
                    lbLog.Items.Add("Start discovering failed: 0x" + Res.ToString("X8"));
            }
        }

        private void btClear_Click(object sender, EventArgs e)
        {
            lbLog.Items.Clear();
        }
    }
}
