// MyMFCProgramDoc.cpp : implementation of the CMyMFCProgramDoc class
//

#include "stdafx.h"
#include "MyMFCProgram.h"

#include "MyMFCProgramDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramDoc

IMPLEMENT_DYNCREATE(CMyMFCProgramDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyMFCProgramDoc, CDocument)
	//{{AFX_MSG_MAP(CMyMFCProgramDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramDoc construction/destruction

CMyMFCProgramDoc::CMyMFCProgramDoc()
{
	// TODO: add one-time construction code here

}

CMyMFCProgramDoc::~CMyMFCProgramDoc()
{
}

BOOL CMyMFCProgramDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramDoc serialization

void CMyMFCProgramDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramDoc diagnostics

#ifdef _DEBUG
void CMyMFCProgramDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMyMFCProgramDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyMFCProgramDoc commands
