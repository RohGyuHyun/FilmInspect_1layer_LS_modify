//Tracker
class C_CtrlTracker{
protected:
	//�׷�ȭ
	BOOL m_bGroup;
	int m_iType;//���� ���� 0: rectangle(no angle), 1: line(angle), 2: circle, 3: ellipse
	CString m_sCaption;//���� ����
	BOOL m_bEnable;//Ȱ��ȭ ����
	COLORREF m_cColor;//�� ��
	CWnd *m_pParentWnd;//�θ� ������ �ڵ鷯
	BOOL m_bDown;//���콺 ��ư �÷��� 
	CRect m_rectOld;//���� ���� ��ǥ ���� ����
	//���콺�� �����ڽ����� 8������� �̵� ��ġ�� ������� �ִ°��� 
	//���� �����ϰ� �ִ� ����
	int m_nChoNum;
	//���� ���μ����� �����ϰ� �ִ� ����
	int m_nProcess;
	//8�� ������ ��ǥ�� 4�������� �̵���ǥ ���� ����
	CRect m_rectDirect[12];
	//���� ���콺 ������ ����
	CPoint m_OldPoint;
	//m_rect������ �����Ǹ� �̿����� ���� m_rectDirect�� ������ ������ִ� 
	//�Լ�
	void MakeDirect();
	//8�� ���� �ڽ� ���� ������ �󸶳� �β���� �����ϴ� ����
	int m_nLineDot;
	//�����ڽ� ������ �β� 
	int m_nLindTick;
	//m_rectDirect������ �����Ǿ� ������ TRUE�׷��� ������ FALSE
	int m_bSet;

	//Zoom
	long m_lZoomStartX;
	long m_lZoomStartY;
	long m_lZoomEndX;
	long m_lZoomEndY;

	//Rectangle//////////////////////////////////
	//�̵��Ǵ� ���� ��ǥ ���� ����
	CRect m_rectTrack;
	//���� �����ڽ��� ��ǥ
	CRect m_rect;
	BOOL m_bActive;
	double m_dRectL;
	double m_dRectT;
	double m_dRectR;
	double m_dRectB;


	int m_nGroupIdx;
	int m_nSelectGroupCnt;
public:
	void GetZoom(long *start_x, long *start_y, long *end_x, long *end_y);
	void SetZoom(long start_x, long start_y, long end_x, long end_y);
	void GetRect(double *left, double *top, double *right, double *bottom);
	void SetRect(double left, double top, double right, double bottom);

	void SetActive(BOOL active);
	void Draw(int idx);
	BOOL GetGroup();
	void SetEnableGroup(BOOL enable);

	int GetType();
	BOOL GetEnable();
	void SetEnable(BOOL enable);
	void SetCaption(char* caption);
	void SetColor(COLORREF color);
	C_CtrlTracker();
	C_CtrlTracker(CWnd *pWnd, int type = 0, int dot = 2, int tick = 3);

	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void SetProcess(int num);
	int  GetProcess();

	int PtInRect(CPoint point);
	void SetMouseCursor();
	int GetChoNum(){return m_nChoNum;}
	BOOL OnLButtonDblClk(UINT nFlags, CPoint point);


	void SetGroupIdx(int idx){m_nGroupIdx = idx;};
	int GetGroupIdx(){return m_nGroupIdx;};
	void SetSelectGroupCnt(int nCnt){m_nSelectGroupCnt = nCnt;};
private:
	void SetRect(CRect rect);
	CRect GetRect();
	
};