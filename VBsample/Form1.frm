VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows �̊���l
   Begin VB.CommandButton Command1 
      Caption         =   "Command1"
      Height          =   615
      Left            =   1680
      TabIndex        =   0
      Top             =   1800
      Width           =   1815
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'RigidChips�Ƀf�[�^�t�@�C�����J�����b�Z�[�W�𑗂�(VB6�T���v��)

'WinAPI�o�^
Private Declare Function RegisterWindowMessageA Lib "user32" (ByVal msgid As String) As Long
Private Declare Function PostMessageA Lib "user32" (ByVal hwd As Long, ByVal msg As Long, ByVal wpara As Long, ByVal lpara As Long) As Long
Const HWND_BROADCAST = -1 '0xFFFF

'���b�Z�[�W��ʒ萔
Const UMSG_RCLOAD_START = 0   '�����񑗐M�J�n
Const UMSG_RCLOAD_CHAR = 1    '�ꕶ��������
Const UMSG_RCLOAD_END = 2     '�����񑗐M�I��

Dim WmRCLoad As Long

Private Sub Form_Load()
' ���p�E�B���h�E���b�Z�[�WID(WM_RIGHTCHIP_LOAD)�o�^
  WmRCLoad = RegisterWindowMessageA("WM_RIGHTCHIP_LOAD")

End Sub

Private Sub Command1_Click()
    
    '���肽��������i�p�X�t���t�@�C�����j��:VB��String��Unicode
    filenameW = "C:\Rigid15B19\Data\BigFoot.txt"
    'Unicode����V�t�gJIS�ւ̕ϊ�
    filenameA = StrConv(filenameW, vbFromUnicode)
  
    '�E�B���h�E���b�Z�[�W���M
    '�����񑗐M�J�n
    PostMessageA HWND_BROADCAST, WmRCLoad, UMSG_RCLOAD_START, 0
    '�ꕶ��������
    For i = 1 To LenB(filenameA)
      PostMessageA HWND_BROADCAST, WmRCLoad, UMSG_RCLOAD_CHAR, AscB(MidB(filenameA, i, 1))
    Next
    '�����񑗐M�I��
    PostMessageA HWND_BROADCAST, WmRCLoad, UMSG_RCLOAD_END, 0

End Sub
