// FiveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Five.h"
#include "FiveDlg.h"
#include "Engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFiveDlg �Ի���




CFiveDlg::CFiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFiveDlg::IDD, pParent)
    , m_width(0)
    , m_height(0)
    , m_blockSize(0)
    , m_left(0)
    , m_top(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    memset(m_chessBoard, 0, sizeof(m_chessBoard));

    m_boardImg = ImageFromIDResource(IDB_BOARD, L"png");
    m_whiteImg = ImageFromIDResource(IDB_WHITE, L"png");
    m_blackImg = ImageFromIDResource(IDB_BLACK, L"png");

    m_engine = new Engine(this);
}


CFiveDlg::~CFiveDlg()
{
    if (m_engine != NULL)
    {
        delete m_engine;
        m_engine = NULL;
    }

//     if (m_blackImg != NULL)
//     {
//         delete m_blackImg;
//         m_blackImg = NULL;
//     }
// 
//     if (m_whiteImg != NULL)
//     {
//         delete m_whiteImg;
//         m_whiteImg = NULL;
//     }
// 
//     if (m_boardImg != NULL)
//     {
//         delete m_boardImg;
//         m_boardImg = NULL;
//     }
}

void CFiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFiveDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDB_BACK, &CFiveDlg::OnBnClickedBack)
END_MESSAGE_MAP()


// CFiveDlg ��Ϣ�������

BOOL CFiveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFiveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFiveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        CRect rect;
        GetClientRect(&rect);
        
        m_width = rect.Width();
        m_height = rect.Height();

        if (m_width > m_height)
        {
            m_blockSize = m_height / 16;
            m_top = m_blockSize;
            m_left = m_blockSize;
        }
        else
        {
            m_blockSize = m_width / 16;
            m_left = m_blockSize;
            m_top = (m_height - 14 * m_blockSize) / 2;
        }

        // ����һ�����⻭��
        Bitmap bmp(m_width, m_height);
        Graphics bmpGraphics(&bmp);

        // ��������
        DrawChessBoard(&bmpGraphics);

        // ��������
        DrawChess(&bmpGraphics);

        // �������ڶԻ����ϻ�ͼ��
        CPaintDC dc(this);
        Graphics graphics(dc.m_hDC);

        // ����һ��CacheBitmap���ڿ��ٻ�ͼ
        CachedBitmap cachedBmp(&bmp, &graphics);
        graphics.DrawCachedBitmap(&cachedBmp,0,0);
        
		CDialog::OnPaint();

        if (GAME_OVER == m_engine->GetStatus())
        {
            LPCTSTR winer;
            if (m_engine->GetWiner() == 1)
            {
                winer = L"��Ӯ�ˣ��Ҽ�����?";
            }
            else
            {
                winer = L"��Ӯ�ˣ�������?";
            }

            if (MessageBox(winer, L"��Ϸ����", MB_YESNO) == IDNO)
            {
                // �˳�����
                CDialog::OnCancel();
            }
            else
            {
                m_engine->StartGame();
                Invalidate();
            }
        }
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFiveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFiveDlg::DrawChessBoard( Graphics *g )
{
    // �������̱���
    if (m_boardImg == NULL)
    {
        return;
    }

    g->DrawImage(m_boardImg, m_left - m_blockSize / 2, m_top - m_blockSize / 2, 15 * m_blockSize, 15 * m_blockSize);

    // �������
    Pen myPen(Color(255, 100, 100, 100), 1);
    for (int i = 0; i < 15; i++)
    {
        g->DrawLine(&myPen, m_left, m_top + m_blockSize * i, m_left + m_blockSize * 14, m_top + m_blockSize * i);
        g->DrawLine(&myPen, m_left + m_blockSize * i, m_top, m_left + m_blockSize * i, m_top + m_blockSize * 14);
    }
}

void CFiveDlg::DrawChess(Graphics *g)
{
    if ((m_whiteImg == NULL) || (m_blackImg == NULL))
    {
        return;
    }

    int chessLeft = m_left - m_blockSize / 2;
    int chessTop = m_top - m_blockSize / 2;
    int chessSize = m_blockSize;
    int x = 0, y = 0;

    vector<CHESS_POINT*> chessList = m_engine->getChessList();
    vector<CHESS_POINT*>::iterator itor = chessList.begin();
    while (itor != chessList.end())
    {
        x = (*itor)->x;
        y = (*itor)->y;
        if ((*itor)->type == CHESS_WHITE)
        {
            g->DrawImage(m_whiteImg, chessLeft + x * m_blockSize, chessTop + y * m_blockSize, chessSize, chessSize);
        }
        else if ((*itor)->type = CHESS_BLACK)
        {
            g->DrawImage(m_blackImg, chessLeft + x * m_blockSize, chessTop + y * m_blockSize, chessSize, chessSize);
        }

        itor++;
    }

    if (chessList.size() > 0)
    {
        SolidBrush myBrush(Color(255, 225, 0, 0));

        g->FillEllipse(&myBrush, chessLeft + x * m_blockSize + m_blockSize / 2 - 2, chessTop + m_blockSize/ 2 + y * m_blockSize - 2, 4, 4);
    }
}

void CFiveDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    CDialog::OnMouseMove(nFlags, point);
}

void CFiveDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    int x = (point.x + m_blockSize / 2 - m_left) / m_blockSize;
    int y = (point.y + m_blockSize / 2 - m_top) / m_blockSize;

    if (m_engine->UserDown(x, y))
    {
        // �ػ�
        //Invalidate();
        RedrawWindow();

        m_engine->TurnAI();
    }
}

Image* CFiveDlg::ImageFromIDResource(UINT nID, LPCTSTR sTR)  
{  
    HINSTANCE hInst = AfxGetResourceHandle();  
    HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type  
    if (!hRsrc)  
        return NULL;  
    // load resource into memory  
    DWORD len = SizeofResource(hInst, hRsrc);  
    BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
    if (!lpRsrc)  
        return NULL;  
    // Allocate global memory on which to create stream  
    HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
    BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
    memcpy(pmem,lpRsrc,len);  
    IStream* pstm;  
    CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
    // load from stream  
    Image *pImg=Gdiplus::Image::FromStream(pstm);  
    // free/release stuff  
    GlobalUnlock(m_hMem);  
    pstm->Release();  
    FreeResource(lpRsrc);  
    return pImg;  
}
BOOL CFiveDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    return TRUE;//CDialog::OnEraseBkgnd(pDC);
}

void CFiveDlg::OnBnClickedBack()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if (TRUE == m_engine->Retract())
    {
        Invalidate();
    }
}
