// MyMFCProgramDoc.h : interface of the CMyMFCProgramDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYMFCPROGRAMDOC_H__2A9F0EAD_A52F_49EE_B7C2_7B1661FEC18D__INCLUDED_)
#define AFX_MYMFCPROGRAMDOC_H__2A9F0EAD_A52F_49EE_B7C2_7B1661FEC18D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMyMFCProgramDoc : public CDocument
{
protected: // create from serialization only
	CMyMFCProgramDoc();
	DECLARE_DYNCREATE(CMyMFCProgramDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyMFCProgramDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyMFCProgramDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMyMFCProgramDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYMFCPROGRAMDOC_H__2A9F0EAD_A52F_49EE_B7C2_7B1661FEC18D__INCLUDED_)
