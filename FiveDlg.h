// FiveDlg.h : 头文件
//

#pragma once

class Engine;

// CFiveDlg 对话框
class CFiveDlg : public CDialog
{
// 构造
public:
	CFiveDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CFiveDlg();

// 对话框数据
	enum { IDD = IDD_FIVE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
    void DrawChessBoard(Graphics *g);
    void DrawChess(Graphics *g);

    Image* ImageFromIDResource(UINT nID, LPCTSTR sTR);

private:
    int m_width;
    int m_height;
    int m_blockSize;
    int m_left;
    int m_top;

    int m_chessBoard[15][15];

    Image *m_boardImg;
    Image *m_whiteImg;
    Image *m_blackImg;

    Engine *m_engine;

public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnBnClickedBack();
};
