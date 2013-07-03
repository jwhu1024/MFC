// MyMFCProgramView.cpp : implementation of the CMyMFCProgramView class
//

#include "stdafx.h"
#include "MyMFCProgram.h"

#include "MyMFCProgramDoc.h"
#include "MyMFCProgramView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramView

IMPLEMENT_DYNCREATE(CMyMFCProgramView, CView)

BEGIN_MESSAGE_MAP(CMyMFCProgramView, CView)
	//{{AFX_MSG_MAP(CMyMFCProgramView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramView construction/destruction

CMyMFCProgramView::CMyMFCProgramView()
{
	// TODO: add construction code here

}

CMyMFCProgramView::~CMyMFCProgramView()
{
}

BOOL CMyMFCProgramView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramView drawing

void CMyMFCProgramView::OnDraw(CDC* pDC)
{
	CMyMFCProgramDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramView printing

BOOL CMyMFCProgramView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMyMFCProgramView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMyMFCProgramView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramView diagnostics

#ifdef _DEBUG
void CMyMFCProgramView::AssertValid() const
{
	CView::AssertValid();
}

void CMyMFCProgramView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMyMFCProgramDoc* CMyMFCProgramView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyMFCProgramDoc)));
	return (CMyMFCProgramDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramView message handlers
