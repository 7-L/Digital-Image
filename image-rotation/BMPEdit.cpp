// BMPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "02test.h"
#include "BMPEdit.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMPEdit

CBMPEdit::CBMPEdit()
{
}

CBMPEdit::~CBMPEdit()
{
}



/////////////////////////////////////////////////////////////////////////////
// CBMPEdit message handlers


BOOL CBMPEdit::LoadBmpFile(const CString &sBmpFileName)
{
	m_bReady = FALSE;

	//释放之前读取的数据///////
	//释放图像数据
	if(m_hgImageData)
		GlobalFree(m_hgImageData);
	
	m_sBMPFileFullName = sBmpFileName;
	if(m_sBMPFileFullName.IsEmpty())
		return FALSE;
	CFile BmpFile;
	if(!BmpFile.Open(m_sBMPFileFullName,CFile::modeRead))
	{
		AfxMessageBox("文件打开时出错!");
		return FALSE;
	}
	//格式检查
	if(BmpFile.Read(&m_BmpFileHeader,sizeof(BITMAPFILEHEADER)) < sizeof(BITMAPFILEHEADER))
	{
		AfxMessageBox("文件数据已损坏!");
		BmpFile.Close();
		return FALSE;
	}
	if(m_BmpFileHeader.bfType != 0x4D42)
	{
		AfxMessageBox("非位图文件");
		BmpFile.Close();
		return FALSE;
	}
	if(BmpFile.Read(&m_BmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER)) < sizeof(BITMAPINFOHEADER))
	{
		AfxMessageBox("文件数据已损坏!");
		BmpFile.Close();
		return FALSE;
	}
	//SetDIBitsToDevice()和StretchDIBits()会自行处理调色板，不需要再向当前DC中选入调色板了！！！
	
	//读取调色板
	if(m_BmpInfo.bmiHeader.biBitCount < 16)
		BmpFile.Read(&m_BmpInfo.bmiColors,GetColorNum(m_BmpInfo.bmiHeader) * sizeof(RGBQUAD));
	
	//读取图像数据
	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth*m_BmpInfo.bmiHeader.biBitCount + 31)/32*4;//图像每行所占实际字节数（须是4的倍数）
	m_hgImageData = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	if(m_hgImageData == NULL)
	{
		AfxMessageBox("内存不足");
		BmpFile.Close();
		return FALSE;
	}
	byte *pImageData = (byte *)GlobalLock(m_hgImageData);
	BmpFile.Read(pImageData,nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
    
	//add your code here
	//图像颠倒
     long int i,j,n;
	 n=640*400*3;
	 for(i=0;i<=n/2;i+=3)
	 {
		 j= *(pImageData+i+0);
		 *(pImageData+i+0)=*(pImageData+n-i+0);
		 *(pImageData+n-i+0)=j;

		 j= *(pImageData+i+1);
		 *(pImageData+i+1)=*(pImageData+n-i+1);
		 *(pImageData+n-i+1)=j;

		 j= *(pImageData+i+2);
		 *(pImageData+i+2)=*(pImageData+n-i+2);
		 *(pImageData+n-i+2)=j;
	 }
  

	

	GlobalUnlock(m_hgImageData);
	
	BmpFile.Close();
	m_bReady = TRUE;
	

	return TRUE;
}

int CBMPEdit::GetColorNum(const BITMAPINFOHEADER &BmpInf)
{
	if(BmpInf.biClrUsed >0)
		return BmpInf.biClrUsed;
	else
		return 1 << BmpInf.biBitCount;
}

BOOL CBMPEdit::PlayBmp(CDC *pDC, const CPoint &StartPoint)
{	
	if(!m_bReady)
		return FALSE;
	byte *pData = (byte *)GlobalLock(m_hgImageData);

	if(!SetDIBitsToDevice(pDC->GetSafeHdc(),
						StartPoint.x,StartPoint.y,
						m_BmpInfo.bmiHeader.biWidth,m_BmpInfo.bmiHeader.biHeight,
						0,0,
						0,m_BmpInfo.bmiHeader.biHeight,//
						pData,
					//	pBmpInf,
					    (BITMAPINFO*)&m_BmpInfo,
						DIB_RGB_COLORS
						))
	 {
		 GlobalUnlock(m_hgImageData);
		 return FALSE;
	 }

	GlobalUnlock(m_hgImageData);
	return TRUE;

}
