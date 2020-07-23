
// MoServerDlg.h: 头文件
//

#pragma once


// CMoServerDlg 对话框
class CMoServerDlg : public CDialogEx
{
// 构造
public:
	CMoServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_ip;	// 服务端端ip
	afx_msg void OnBnClickedConnect();	// ip确认按钮
	afx_msg void OnEnChangeIp();		// ip编辑框
	afx_msg void OnBnClickedCancel();	// ip取消按钮
	//afx_msg void OnEnChangeChat();		// 聊天接受框
	afx_msg void OnClose();				// 系统框的关闭
	afx_msg void OnBnClickedIsServer();	//是否是服务端
	
	CString m_chat;						// 接收框显示的文本
	afx_msg void OnBnClickedEmpty();	// 清空接收框按钮
	afx_msg void OnBnClickedSend();		// 发送按钮
	afx_msg void OnEnChangeInput();		// 发送框
	CString m_send;						// 发送框内容

	WSADATA wsaData;					// 声明
	SOCKET sServer;						// 声明服务端套接字描述符
	SOCKET sClient;						// 声明客户端套接字描述符
	SYSTEMTIME time;					// 声明系统时间
	struct sockaddr_in ServerAddr;		// 声明服务端结构体
	char szMsg[MAXBYTE] = { 0 };		// 声明发送和接收的文本
	struct sockaddr_in ClientAddr;		// 声明客户端结构体
	int nSize;							// 声明保存结构体长度
	void  CMoServerDlg::Chat(CString m_chat);		// 内容显示
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLbnSelchangeAccept();
	//afx_msg void OnLbnSelchangeList2();
	//CListBox m_List;

	//线程
	
	CWinThread* m_pThread;		// 线程
	bool isThreadBegin = false;	// 判断线程结束
	static UINT ThreadFuntion(LPVOID pParam);	// 线程中实现接受
	CListBox m_List;	// 列表框的控件
	int nIndex;			// 列表框的索引
	int nCount;			// 列表框的长度

	POINT Old;			//存放客户区的宽和高
	afx_msg void OnSize(UINT nType, int cx, int cy);	// 等比例缩放宽高
	void ShowInfo(CListBox* pListBox, CString infoStr);
	CListBox* m_listBox;

};
