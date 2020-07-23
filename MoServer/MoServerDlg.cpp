
// MoServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MoServer.h"
#include "MoServerDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#pragma comment (lib,"ws2_32")
#include "resource.h"
#define WM_THREADMSG WM_USER+1


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMoServerDlg 对话框

CMoServerDlg::CMoServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOSERVER_DIALOG, pParent)
    , m_ip(_T(""))
    //, m_chat(_T(""))
    , m_send(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMoServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_IP, m_ip);
    DDX_Text(pDX, IDC_INPUT, m_send);
    DDX_Control(pDX, IDC_ACCEPT, m_List);
}

BEGIN_MESSAGE_MAP(CMoServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CMoServerDlg::OnBnClickedSend)
    ON_BN_CLICKED(IDC_CONNECT, &CMoServerDlg::OnBnClickedConnect)
    ON_EN_CHANGE(IDC_IP, &CMoServerDlg::OnEnChangeIp)
    ON_BN_CLICKED(IDC_CANCEL, &CMoServerDlg::OnBnClickedCancel)
    ON_EN_CHANGE(IDC_INPUT, &CMoServerDlg::OnEnChangeInput)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_EMPTY, &CMoServerDlg::OnBnClickedEmpty)
    ON_BN_CLICKED(IDC_IS_SERVER, &CMoServerDlg::OnBnClickedIsServer)
    ON_LBN_SELCHANGE(IDC_ACCEPT, &CMoServerDlg::OnLbnSelchangeAccept)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CMoServerDlg 消息处理程序

BOOL CMoServerDlg::OnInitDialog()	// 初始化
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	SetWindowText(_T("Mo"));		// 设置标题

    // 得到初始化时客户区宽高
    CRect rect;
    GetClientRect(&rect);           // 取客户区的大小
    Old.x = rect.right - rect.left; // 宽
    Old.y = rect.bottom - rect.top; // 高

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMoServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMoServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMoServerDlg::OnBnClickedConnect()             // 确认按钮，确认服务端ip
{
    // TODO: 在此添加控件通知处理程序代码
    GetDlgItem(IDC_IP)->GetWindowText(m_ip);
    GetDlgItem(IDC_IP)->EnableWindow(FALSE);       // ip编辑框不可编辑
    GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);  // ip确认按钮不可点击
    GetDlgItem(IDC_CANCEL)->EnableWindow(TRUE);    // 清空按钮可点击

    //  启动
    WSAStartup(MAKEWORD(2, 2), &wsaData); 

    // 创建套接字
    sServer = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 填充结构体
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.S_un.S_addr = inet_addr(m_ip);
    ServerAddr.sin_port = htons(7777);

    // 如果是服务端则绑定
    if (BST_CHECKED == IsDlgButtonChecked(IDC_IS_SERVER))
    {
        bind(sServer, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
        nSize = sizeof(ClientAddr);
        recvfrom(sServer, szMsg, MAXBYTE, 0, (SOCKADDR*)&ClientAddr, &nSize);
        ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(szMsg);

        GetLocalTime(&time);
        m_chat.Format(_T("%04d年%02d月%02d日 %02d:%02d:%02d\r\n"),
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
        ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);

        m_chat.Format(_T("对方IP:%s:%d\r\n"), inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
        ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);
        ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString("\r\n");

        lstrcpy(szMsg, "默通道已建立！\r\n");
        nSize = sizeof(ClientAddr);
        sendto(sServer, szMsg, MAXBYTE, 0, (SOCKADDR*)&ClientAddr, nSize);
    }
    else
    {
        sClient = sServer;
        ClientAddr = ServerAddr;      
        lstrcpy(szMsg, "默通道已建立！\r\n");
        nSize = sizeof(ServerAddr);
        sendto(sClient, szMsg, MAXBYTE, 0, (SOCKADDR*)&ServerAddr, nSize);
       
    }
    //GetDlgItem(IDC_ACCEPT)->EnableWindow(FALSE);
    isThreadBegin = true;
    m_pThread = AfxBeginThread((AFX_THREADPROC)ThreadFuntion, this);    // 创建并启动线程   
}

void CMoServerDlg::OnBnClickedCancel()              // ip取消按钮，清空IP输入的内容
{
    // TODO: 在此添加控件通知处理程序代码
    GetDlgItem(IDC_IP)->SetWindowText(_T(""));      // 清空ip框
    GetDlgItem(IDC_IP)->EnableWindow(TRUE);         // ip编辑框可编辑
    GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);    // ip确认按钮可点击
    GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);    // 清空按钮不可点击
    //ExitThread(1);
    if (m_pThread == NULL)
    {
        return;
    }
    isThreadBegin = false;
    //WaitForSingleObject(m_pThread->m_hThread,INFINITE);  //等待线程退出
    WSACleanup();                                   // 结束
}

void CMoServerDlg::OnBnClickedEmpty()               // 清空接收框
{
    // TODO: 在此添加控件通知处理程序代码
    ((CListBox*)GetDlgItem(IDC_ACCEPT))->ResetContent();    // 清空对话框
}

void CMoServerDlg::OnBnClickedSend()	// 发送按钮
{
    // TODO: 在此添加控件通知处理程序代码
    GetDlgItem(IDC_INPUT)->GetWindowText(m_send);
    nSize = sizeof(ClientAddr);
    sendto(sServer, m_send, strlen(m_send) + sizeof(char), 0, (SOCKADDR*)&ClientAddr, nSize);
    
    GetLocalTime(&time);
    m_chat.Format(_T("%04d年%02d月%02d日 %02d:%02d:%02d\r\n"),
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);

    m_chat.Format(_T("自己:%s \r\n"), m_send);
    //((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);

    
    m_listBox = (CListBox*)GetDlgItem(IDC_ACCEPT);
    ShowInfo(m_listBox, m_send);
    m_listBox->AddString(_T("\r\n"));

    // 发送消息自动往下滑
    nCount = ((CListBox*)GetDlgItem(IDC_ACCEPT))->GetCount();
    if (nCount > 1)
    {
        ((CListBox*)GetDlgItem(IDC_ACCEPT))->SetCurSel(nCount - 1);
    }

    


    /*
    int nIndex = m_List.GetCurSel();
    //int nCount = m_List.GetCount();
    nSize = sizeof(m_send);
    
    for (int i = 0; i < nSize; i++)
    {
        if (m_send[i] == '\r')
        {
            i++;
            m_List.SetCurSel(++nIndex);
        }
        else
        {
            m_chat.Format(_T("自己:%s \r\n"), m_send[i]);
            ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);
        }

    }*/

    GetDlgItem(IDC_INPUT)->SetWindowText(_T(""));    // 清空发送框

}

void CMoServerDlg::OnLbnSelchangeAccept()   // 接受数据
{
    GetLocalTime(&time);
    m_chat.Format(_T("%04d年%02d月%02d日 %02d:%02d:%02d"),
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);

    m_chat.Format(_T("对方:%s"), szMsg);
    //((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(m_chat);
    m_listBox = (CListBox*)GetDlgItem(IDC_ACCEPT);
    ShowInfo(m_listBox, m_chat);
    m_listBox->AddString(_T("\r\n"));

    // 收到消息自动往下滑
    nCount = ((CListBox*)GetDlgItem(IDC_ACCEPT))->GetCount();
    if (nCount > 1)
    {
        ((CListBox*)GetDlgItem(IDC_ACCEPT))->SetCurSel(nCount - 1);
    }

    /*
    if ((nIndex != LB_ERR) && (nCount > 1))
    {
        if (++nIndex < nCount)
            m_List.SetCurSel(nIndex);
        else
            m_List.SetCurSel(0);
    }

    for (int i = 0;i < sizeof(m_chat); i++)
    {
        if (*(&m_chat + i) == '\n')
        {
            ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString("");
        }
        else
        {

            ((CListBox*)GetDlgItem(IDC_ACCEPT))->AddString(*(&m_chat + i));
        }

    }*/

}

// 线程
UINT  CMoServerDlg::ThreadFuntion(LPVOID pParam)
{
    CMoServerDlg* pDlg = (CMoServerDlg*)pParam;

    while (pDlg->isThreadBegin)
    {
        pDlg->nSize = sizeof(pDlg->ClientAddr);
        recvfrom(pDlg->sServer, pDlg->szMsg, MAXBYTE, 0, (SOCKADDR*)&(pDlg->ClientAddr), &(pDlg->nSize));
        pDlg->OnLbnSelchangeAccept();
    }
    return 1;
}

void CMoServerDlg::OnBnClickedIsServer()    //是否是服务端
{
    // TODO: 在此添加控件通知处理程序代码
}

BOOL CMoServerDlg::PreTranslateMessage(MSG* pMsg)   // 重写虚函数，使mfc按ESC和回车不会自动退出
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)   // ESC
        return TRUE;
   
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   // 回车+shift 
    {
        
            ((CEdit*)GetDlgItem(IDC_INPUT))->
                SetSel(GetDlgItem(IDC_INPUT)->GetWindowTextLength(),
                    GetDlgItem(IDC_INPUT)->GetWindowTextLength());   // 将指针移至末尾
            ((CEdit*)GetDlgItem(IDC_INPUT))->ReplaceSel("\r\n");
            return TRUE;
           
      /*if (pMsg->message == WM_KEYDOWN && pMsg->message == VK_SHIFT)
        {
           }
        else     // 回车
        {
            //OnBnClickedSend();
            return TRUE;
        }
        */   
    }       
    else
    return CDialogEx::PreTranslateMessage(pMsg);
}

void CMoServerDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    WSACleanup();           // 结束
    CDialogEx::OnClose();
}

void CMoServerDlg::OnEnChangeIp()   // IP编辑框
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
}

void CMoServerDlg::OnEnChangeInput()    // 发送消息框
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码

    // 发送消息
    //lstrcpy((LPWSTR)szMsg, (LPCWSTR)"HI!Client!!!\r\n");
}


/*
void CMoServerDlg::OnEnChangeChat() // 消息接收框
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    //UpdateData(FALSE);

    //接受消息
    nSize = sizeof(ClientAddr);
    recvfrom(sServer, szMsg, MAXBYTE, 0, (SOCKADDR*)&ClientAddr, &nSize);
    GetDlgItem(IDC_CHAT)->SetWindowText(szMsg);
    m_chat.Format(_T("对方IP:%s:%d\r\n"), inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
    //Chat(m_chat);
    GetDlgItem(IDC_CHAT)->SetWindowText(m_chat);


    //while (1)
    //{
        GetLocalTime(&time);
        nSize = sizeof(ClientAddr);recvfrom(sServer,szMsg, MAXBYTE, 0, (SOCKADDR *)&ClientAddr,&nSize);
        recvfrom(sServer, szMsg, MAXBYTE, 0, (SOCKADDR*)&ClientAddr, &nSize);
        GetDlgItem(IDC_CHAT)->SetWindowText(szMsg);
        m_chat.Format(_T("%04d年%02d月%02d日 %02d:%02d:%02d\r\n"),
                        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
        //Chat(m_chat);
        GetDlgItem(IDC_CHAT)->SetWindowText(m_chat);
        m_chat.Format(_T("Client Msf:%s \r\n"), szMsg);
        //Chat(m_chat);
        GetDlgItem(IDC_CHAT)->SetWindowText(m_chat);
    //}
}
*/


void CMoServerDlg::OnSize(UINT nType, int cx, int cy)   // 等比例缩放宽高
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)//窗体大小发生变动，处理程序
    {
        float fsp[2];
        POINT Newp;                                 //获取现在对话框的大小
        CRect recta;
        GetClientRect(&recta);                      //取客户区大小   
        Newp.x = recta.right - recta.left;
        Newp.y = recta.bottom - recta.top;
        fsp[0] = (float)Newp.x / Old.x;             // 计算缩放比例
        fsp[1] = (float)Newp.y / Old.y;

        CRect Rect;
        int id;
        CPoint OldTLPoint, TLPoint;                 //左上角
        CPoint OldBRPoint, BRPoint;                 //右下角
        HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD); //列出所有控件   
        while (hwndChild)
        {
            id = ::GetDlgCtrlID(hwndChild);        //取得ID
            GetDlgItem(id)->GetWindowRect(Rect);
            ScreenToClient(Rect);
            OldTLPoint = Rect.TopLeft();
            TLPoint.x = long(OldTLPoint.x * fsp[0]);
            TLPoint.y = long(OldTLPoint.y * fsp[1]);
            OldBRPoint = Rect.BottomRight();
            BRPoint.x = long(OldBRPoint.x * fsp[0]);
            BRPoint.y = long(OldBRPoint.y * fsp[1]); //高度不可读的控件（如:combBox),不要改变此值.
            Rect.SetRect(TLPoint, BRPoint);
            GetDlgItem(id)->MoveWindow(Rect, TRUE); // 改变窗口
            hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
        }
        Old = Newp;
    }

}

void CMoServerDlg::ShowInfo(CListBox* pListBox, CString infoStr)
{
    CRect clientRect;
    pListBox->GetClientRect(&clientRect);
    int maxWidth = clientRect.Width();

    int len = infoStr.GetLength();

    CFont* pFont = pListBox->GetFont();
    CDC* pDC = pListBox->GetDC();

    CFont* pOldFont = pDC->SelectObject(pFont);

    CSize sz = pDC->GetTextExtent(infoStr, len);

    while (sz.cx > maxWidth)
    {
        len--;
        sz = pDC->GetTextExtent(infoStr, len);
    }

    pDC->SelectObject(pOldFont);

    pListBox->AddString(infoStr.Left(len));

    int strLen = infoStr.GetLength();
    if (len != strLen)
    {
        ShowInfo(pListBox, infoStr.Right(strLen - len));
    }
}

