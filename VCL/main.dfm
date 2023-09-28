object fmMain: TfmMain
  Left = 192
  Top = 117
  BorderStyle = bsSingle
  Caption = 'RFCOMM Client Connection Test Application'
  ClientHeight = 430
  ClientWidth = 681
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object btDiscover: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Discover'
    TabOrder = 0
    OnClick = btDiscoverClick
  end
  object lbLog: TListBox
    Left = 8
    Top = 40
    Width = 665
    Height = 377
    ItemHeight = 13
    TabOrder = 1
  end
  object cbUnpair: TCheckBox
    Left = 96
    Top = 16
    Width = 153
    Height = 17
    Caption = 'Unpair before connect'
    TabOrder = 2
  end
  object btClear: TButton
    Left = 592
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 3
    OnClick = btClearClick
  end
  object wclBluetoothManager: TwclBluetoothManager
    AfterOpen = wclBluetoothManagerAfterOpen
    OnAuthenticationCompleted = wclBluetoothManagerAuthenticationCompleted
    OnClosed = wclBluetoothManagerClosed
    OnConfirm = wclBluetoothManagerConfirm
    OnDeviceFound = wclBluetoothManagerDeviceFound
    OnDiscoveringCompleted = wclBluetoothManagerDiscoveringCompleted
    OnDiscoveringStarted = wclBluetoothManagerDiscoveringStarted
    OnNumericComparison = wclBluetoothManagerNumericComparison
    OnPasskeyNotification = wclBluetoothManagerPasskeyNotification
    OnPasskeyRequest = wclBluetoothManagerPasskeyRequest
    OnPinRequest = wclBluetoothManagerPinRequest
    Left = 248
    Top = 168
  end
end
