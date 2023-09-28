unit main;

interface

uses
  Forms, wclBluetooth, StdCtrls, Controls, Classes;

type
  TfmMain = class(TForm)
    btDiscover: TButton;
    lbLog: TListBox;
    cbUnpair: TCheckBox;
    btClear: TButton;
    wclBluetoothManager: TwclBluetoothManager;
    procedure FormCreate(Sender: TObject);
    procedure wclBluetoothManagerDiscoveringCompleted(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Error: Integer);
    procedure wclBluetoothManagerDeviceFound(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64);
    procedure wclBluetoothManagerDiscoveringStarted(Sender: TObject;
      const Radio: TwclBluetoothRadio);
    procedure wclBluetoothManagerClosed(Sender: TObject);
    procedure wclBluetoothManagerAfterOpen(Sender: TObject);
    procedure wclBluetoothManagerPinRequest(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64;
      out Pin: String);
    procedure wclBluetoothManagerPasskeyRequest(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64;
      out Passkey: Cardinal);
    procedure wclBluetoothManagerPasskeyNotification(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64;
      const Passkey: Cardinal);
    procedure wclBluetoothManagerNumericComparison(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64;
      const Number: Cardinal; out Confirm: Boolean);
    procedure wclBluetoothManagerConfirm(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64;
      out Confirm: Boolean);
    procedure wclBluetoothManagerAuthenticationCompleted(Sender: TObject;
      const Radio: TwclBluetoothRadio; const Address: Int64;
      const Error: Integer);
    procedure FormDestroy(Sender: TObject);
    procedure btDiscoverClick(Sender: TObject);
    procedure btClearClick(Sender: TObject);

  private
    FClients: TList;
    FCurDevice: Integer;

    function GetRadio: TwclBluetoothRadio;
    procedure DisconnectClients;
    procedure ConnectToNextDevice;
    procedure ConnectToCurrentDevice;

    procedure ClientData(Sender: TObject; const Data: Pointer;
      const Size: Cardinal);
    procedure ClientDisconnect(Sender: TObject; const Reason: Integer);
    procedure ClientConnect(Sender: TObject; const Error: Integer);
  end;

var
  fmMain: TfmMain;

implementation

uses
  wclErrors, SysUtils, wclConnections, wclUUIDs, Windows;

{$R *.dfm}

{ TfmMain }

procedure TfmMain.ConnectToCurrentDevice;
var
  Radio: TwclBluetoothRadio;
  Res: Integer;
begin
  lbLog.Items.Add('Try to connect to [' +
    IntToHex(TwclRfCommClient(FClients[FCurDevice]).Address, 12) + ']');
  Radio := GetRadio;
  if Radio <> nil then begin
    if cbUnpair.Checked then begin
      lbLog.Items.Add('Try to unpair');
      Res := Radio.RemoteUnpair(TwclRfCommClient(FClients[FCurDevice]).Address);
      lbLog.Items.Add('Unpair result: 0x' + IntToHex(Res, 8));
    end;

    Res := TwclRfCommClient(FClients[FCurDevice]).Connect(Radio);
    if Res <> WCL_E_SUCCESS then begin
      lbLog.Items.Add('Connect failed: 0x' + IntToHex(Res, 8));
      ConnectToNextDevice;
    end;
  end;
end;

procedure TfmMain.ConnectToNextDevice;
begin
  Inc(FCurDevice);
  lbLog.Items.Add('Processing next client: ' + IntToStr(FCurDevice));
  if FCurDevice < FClients.Count then
    ConnectToCurrentDevice
  else
    lbLog.Items.Add('All clients processed');
end;

procedure TfmMain.DisconnectClients;
var
  Client: TwclRfCommClient;
begin
  while FClients.Count > 0 do begin
    Client := TwclRfCommClient(FClients[0]);
    if Client.State <> csDisconnected then
      Client.Disconnect
    else
      FClients.Delete(0);
    Client.Free;
  end;
  FClients.Clear;
end;

function TfmMain.GetRadio: TwclBluetoothRadio;
var
  Res: Integer;
begin
  Res := wclBluetoothManager.GetClassicRadio(Result);
  if Res <> WCL_E_SUCCESS then
    lbLog.Items.Add('Unable to get Classic Radio: 0x' + IntToHex(Res, 8));
end;

procedure TfmMain.FormCreate(Sender: TObject);
var
  Res: Integer;
begin
  FClients := TList.Create;

  Res := wclBluetoothManager.Open;
  if Res <> WCL_E_SUCCESS then
    lbLog.Items.Add('Bluetooth Manager open failed: 0x' + IntToHex(Res, 8));
end;

procedure TfmMain.wclBluetoothManagerDiscoveringCompleted(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Error: Integer);
begin
  lbLog.Items.Add('Discovering completed with result: 0x' + IntToHex(Error, 8));
  lbLog.Items.Add('Found ' + IntToStr(FClients.Count) + ' devices');
  if FClients.Count > 0 then begin
    FCurDevice := 0;
    ConnectToCurrentDevice;
  end;
end;

procedure TfmMain.wclBluetoothManagerDeviceFound(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Address: Int64);
var
  Client: TwclRfCommClient;
begin
  lbLog.Items.Add('Device found [' + IntToHex(Address, 12) + ']');

  Client := TwclRfCommClient.Create(nil);
  Client.Address := Address;
  Client.Service := SerialPortServiceClass_UUID;

  Client.OnConnect := ClientConnect;
  Client.OnDisconnect := ClientDisconnect;
  Client.OnData := ClientData;

  FClients.Add(Client);
end;

procedure TfmMain.ClientData(Sender: TObject; const Data: Pointer;
  const Size: Cardinal);
var
  Str: AnsiString;
begin
  if (Data <> nil) and (Size > 0) then begin
    SetLength(Str, Size);
    CopyMemory(Pointer(Str), Data, Size);
    lbLog.Items.Add('Received: ' + Str);
  end;
end;

procedure TfmMain.ClientDisconnect(Sender: TObject; const Reason: Integer);
var
  Address: string;
begin
  Address := IntToHex(TwclRfCommClient(Sender).Address, 12);
  lbLog.Items.Add('Client [' + Address + '] disconnected with reason: 0x' +
    IntToHex(Reason, 8));
  FClients.Remove(TwclRfCommClient(Sender));
end;

procedure TfmMain.ClientConnect(Sender: TObject; const Error: Integer);
var
  Address: string;
begin
  Address := IntToHex(TwclRfCommClient(Sender).Address, 12);
  if Error <> WCL_E_SUCCESS then begin
    lbLog.Items.Add('Client [' + Address + '] connect failed: 0x' +
      IntToHex(Error, 8));
  end else
    lbLog.Items.Add('Client [' + Address + '] connected');
  ConnectToNextDevice;
end;

procedure TfmMain.wclBluetoothManagerDiscoveringStarted(Sender: TObject;
  const Radio: TwclBluetoothRadio);
begin
  lbLog.Items.Add('Discovering started');
  DisconnectClients;
end;

procedure TfmMain.wclBluetoothManagerClosed(Sender: TObject);
begin
  lbLog.Items.Add('Bluetooth Manager closed');
end;

procedure TfmMain.wclBluetoothManagerAfterOpen(Sender: TObject);
begin
  lbLog.Items.Add('Bluetooth Manager opened');
end;

procedure TfmMain.wclBluetoothManagerPinRequest(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Address: Int64; out Pin: String);
begin
  lbLog.Items.Add('PIN pairing with device [' + IntToHex(Address, 12) + ']');
  Pin := '0000';
end;

procedure TfmMain.wclBluetoothManagerPasskeyRequest(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Address: Int64;
  out Passkey: Cardinal);
begin
  lbLog.Items.Add('Passkey request pairing with device [' +
    IntToHex(Address, 12) + ']');
  Passkey := 1234;
end;

procedure TfmMain.wclBluetoothManagerPasskeyNotification(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Address: Int64;
  const Passkey: Cardinal);
begin
  lbLog.Items.Add('Passkey notification pairing with device [' +
    IntToHex(Address, 12) + ']: ' + IntToStr(Passkey));
end;

procedure TfmMain.wclBluetoothManagerNumericComparison(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Address: Int64;
  const Number: Cardinal; out Confirm: Boolean);
begin
  lbLog.Items.Add('Numeric comparison with device [' + IntToHex(Address, 12) +
    ']: ' + IntToStr(Number));
  Confirm := True;
end;

procedure TfmMain.wclBluetoothManagerConfirm(Sender: TObject;
  const Radio: TwclBluetoothRadio; const Address: Int64;
  out Confirm: Boolean);
begin
  lbLog.Items.Add('Just Works pairing with device [' + IntToHex(Address, 12) +
    ']');
  Confirm := True;
end;

procedure TfmMain.wclBluetoothManagerAuthenticationCompleted(
  Sender: TObject; const Radio: TwclBluetoothRadio; const Address: Int64;
  const Error: Integer);
begin
  lbLog.Items.Add('Authentication with device [' + IntToHex(Address, 12) +
    '] completed with result: 0x' + IntToHex(Error, 8));
end;

procedure TfmMain.FormDestroy(Sender: TObject);
begin
  DisconnectClients;
  wclBluetoothManager.Close;
  FClients.Free;
end;

procedure TfmMain.btDiscoverClick(Sender: TObject);
var
  Radio: TwclBluetoothRadio;
  Res: Integer;
begin
  Radio := GetRadio;
  if Radio <> nil then begin
    Res := Radio.Discover(10, dkClassic);
    if Res <> WCL_E_SUCCESS then
      lbLog.Items.Add('Start discovering failed: 0x' + IntToHex(Res, 8));
  end;
end;

procedure TfmMain.btClearClick(Sender: TObject);
begin
  lbLog.Items.Clear;
end;

end.
