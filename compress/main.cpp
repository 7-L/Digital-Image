// BMPEdit.cpp : implementation file
//

#include "StdAfx.h"
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

double QuantTbl[8][8]={	
	 // 0     1     2     3     4     5     6     7   
    	0,    0,    0,    0,    0,    0,    0,    0,  //7 
		0,    0,    0,    0,    0,    0,    0,    0,  //6 
		0,    0,    0,    0,    0,    0,    0,    0,  //5 
		0,    0,    0,    0,    0,    0,    0,    0,  //4 
		1,    1,    1,    1,    0,    0,    0,    0,  //3 
		1,    1,    1,    1,    0,    0,    0,    0,  //2 
		1,    1,    1,    1,    0,    0,    0,    0,  //1 
		2,    1,    1,    1,    0,    0,    0,    0   //0 			

};



CBMPEdit::CBMPEdit()
{
	//Add your code here


}

CBMPEdit::~CBMPEdit()
{
	//Add your code here

}



/////////////////////////////////////////////////////////////////////////////
// CBMPEdit message handlers


BOOL CBMPEdit::LoadBmpFile(const CString &sBmpFileName)
{
	m_bReady = FALSE;

	//ÊÍ·ÅÖ®Ç°¶ÁÈ¡µÄÊý¾Ý///////
	//ÊÍ·ÅÍ¼ÏñÊý¾Ý
	if(m_hgImageData)
		GlobalFree(m_hgImageData);
	
	m_sBMPFileFullName = sBmpFileName;
	if(m_sBMPFileFullName.IsEmpty())
		return FALSE;
	CFile BmpFile;
	if(!BmpFile.Open(m_sBMPFileFullName,CFile::modeRead))
	{
		AfxMessageBox("ÎÄ¼þ´ò¿ªÊ±³ö´í!");
		return FALSE;
	}
	//¸ñÊ½¼ì²é
	if(BmpFile.Read(&m_BmpFileHeader,sizeof(BITMAPFILEHEADER)) < sizeof(BITMAPFILEHEADER))
	{
		AfxMessageBox("ÎÄ¼þÊý¾ÝÒÑËð»µ!");
		BmpFile.Close();
		return FALSE;
	}
	if(m_BmpFileHeader.bfType != 0x4D42)
	{
		AfxMessageBox("·ÇÎ»Í¼ÎÄ¼þ");
		BmpFile.Close();
		return FALSE;
	}
	if(BmpFile.Read(&m_BmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER)) < sizeof(BITMAPINFOHEADER))
	{
		AfxMessageBox("ÎÄ¼þÊý¾ÝÒÑËð»µ!");
		BmpFile.Close();
		return FALSE;
	}
	//SetDIBitsToDevice()ºÍStretchDIBits()»á×ÔÐÐ´¦Àíµ÷É«°å£¬²»ÐèÒªÔÙÏòµ±Ç°DCÖÐÑ¡Èëµ÷É«°åÁË£¡£¡£¡
	
	//¶ÁÈ¡µ÷É«°å
	if(m_BmpInfo.bmiHeader.biBitCount < 24)
		BmpFile.Read(&m_BmpInfo.bmiColors,GetColorNum(m_BmpInfo.bmiHeader) * sizeof(RGBQUAD));
	
	//¶ÁÈ¡Í¼ÏñÊý¾Ý
	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth*m_BmpInfo.bmiHeader.biBitCount + 31)/32*4;//Í¼ÏñÃ¿ÐÐËùÕ¼Êµ¼Ê×Ö½ÚÊý£¨ÐëÊÇ4µÄ±¶Êý£©
	m_hgImageData = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

	if(m_hgImageData == NULL)
	{
		AfxMessageBox("ÄÚ´æ²»×ã");
		BmpFile.Close();
		return FALSE;
	}
	byte *pImageData = (byte *)GlobalLock(m_hgImageData);
	BmpFile.Read(pImageData,nBytePerLine * m_BmpInfo.bmiHeader.biHeight);

    //can add some processing

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

BOOL CBMPEdit::PlayBmpDCT(CDC *pDC, const CPoint &StartPoint)
{
	if(!m_bReady)
		return FALSE;
	byte *pData = (byte *)GlobalLock(m_DCTResultD);
	
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
		GlobalUnlock(m_DCTResultD);
		return FALSE;
	}
	
	GlobalUnlock(m_DCTResultD);
	return TRUE;

}

BOOL CBMPEdit::DCTTransform()
{
	//
	//added by tonglijing
	//AfxMessageBox("Test!");
	//Í¼ÏñÃ¿ÐÐËùÕ¼Êµ¼Ê×Ö½ÚÊý£¨ÐëÊÇ4µÄ±¶Êý£©
	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth*m_BmpInfo.bmiHeader.biBitCount + 31)/32*4;
    byte *pImageData = (byte *)GlobalLock(m_hgImageData);

	m_DCTResultD = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
    byte *pDCTResultD = (byte *)GlobalLock(m_DCTResultD);
	
	int u,v,x,y,height,width;
	double Fuv;
	height = m_BmpInfo.bmiHeader.biHeight;
	width = m_BmpInfo.bmiHeader.biWidth;
    pFuvArray = new double[height*width];
    pImage8 = new double[64];
	pFuv8 = new double[64];

    for (v=0; v<height/8; v++)
    {
		for (u=0; u<width/8; u++)
		{			
            //prepare pImage8
			for (y=0;y<8;y++)
			{
				for (x=0;x<8;x++)
				{

                    *(pImage8+y*8+x) = *(pImageData + v*8*width + y*width + u*8 + x);

				}
			}

			//do  DCT
            DCTTransform8();

            //get result
			for (y=0;y<8;y++)
			{
				for (x=0;x<8;x++)
				{
					
					pFuvArray[v*8*width + y*width + u*8 + x] = pFuv8[y*8+x];

					//prepare display 
					Fuv = pFuv8[y*8+x];
					if (Fuv<0)
					{
                         Fuv = Fuv * (-1);
					}

					//512 ÓÃ 1000
					//Fuv = Fuv * 1000.0;
					
					//128ÓÃ100
					//Fuv = Fuv * 100.0;

					//8 ÓÃ
                    Fuv = Fuv * 10;

					if (Fuv>254)
					{
						//AfxMessageBox("Fuv>255");
						Fuv = 255;
					}
					*(pDCTResultD + v*8*width + y*width + u*8 + x) = (byte)(Fuv+0.5);

				}
			}
		}
    }

	GlobalUnlock(m_hgImageData);
	GlobalUnlock(m_DCTResultD);

	return TRUE;
}

BOOL CBMPEdit::DCTTransform8()
{
	//added by tonglijing
	//AfxMessageBox("Test!");
	
	int u,v,x,y;
	double Fuv,pre,post,Cu,Cv,fxy;
	double pi=3.1415926;
	
    for (v=0;v<8;v++)
    {
		for (u=0;u<8;u++)
		{
			post = 0.0;
            if (v==0)
            {
				Cv = 1 / 1.414;
            } 
            else
            {
				Cv = 1;
            }
            if (u==0)
            {
				Cu = 1 / 1.414;
            } 
            else
            {
				Cu = 1;
            }
			pre = (double)4 / (8*8) * Cu * Cu * Cv * Cv;
			for (y=0;y<8;y++)
			{
				for (x=0;x<8;x++)
				{
                    fxy = *(pImage8+y*8+x);
                    post = post + fxy * cos(pi/2/8*(2*x+1)*u) * cos(pi/2/8*(2*y+1)*v);
				}
			}
			Fuv = pre * post;
            pFuv8[v*8+u] = Fuv;

		}
    }
	

	return TRUE;
}

//¶ÔDCTµÄÏµÊýpFuvArray[]½øÐÐÁ¿»¯
//¼´¶ÔpFuvArray[]ÖÐµÄÊý¾Ý³ýÒÔÁ¿»¯±íQuantTbl[][]£¬
//ÐèÒª¿¼ÂÇÁ¿»¯±í8*8µÄÎ»ÖÃ¶ÔÓ¦£¬ÒÔ¼°Á¿»¯±íÖÐÎª0Êý¾ÝµÄÁ¿»¯´¦Àí
//Á¿»¯½á¹û·ÅÈëÁ¿»¯½á¹û»º³åÇøpQuantResult[]
BOOL CBMPEdit::DCTQuantify()
{
	int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;
	//pQuantResult = new __int8[height*width];
	//add your code here================================================ 
	//pQuantResult = (__int8 *)malloc(Height * Width * sizeof(__int8));
	pQuantResult = (char *)malloc(height * width * sizeof(double));
	int u,v,x,y;
	for(v = 0;v < height; v++)
	{
		for(u = 0;u < width; u++)
		{
			x = u % 8;
			y = v % 8;
			if(QuantTbl[7 - y][x] != 0)
			{
				pQuantResult[v * width + u] = pFuvArray[v * width + u] / QuantTbl[7 - y][x];
			}
			else
			{
				pQuantResult[v * width + u] = 0;
			}
		}
	}

	return TRUE;
}

//°ÑÁ¿»¯ºóµÄÊý¾ÝpQuantResult[]Ð´ÈëÎÄ¼þ
//¶ÔÓÚÁ¿»¯±íQuantTbl[][]ÖÐÎª0µÄÊý¾ÝµÄ¶ÔÓ¦Î»ÖÃ£¬ÎÞÐè´æ´¢¡£
//ÏòÎÄ¼þÐ´ÈëÊý¾ÝÊ¾Àý£º
//CFile QuantFile("quantify.txt",CFile::modeCreate|CFile::modeReadWrite );
//QuantFile.Write( pQuantResult , n);
//QuantFile.Close();
BOOL CBMPEdit::DCTWrite()
{
	//add your code here================================================== 
	CFile QuantFile("quantify.txt",CFile::modeCreate|CFile::modeReadWrite );
	//QuantFile.Write( pQuantResult , COLUMN*ROW);
    int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;

	int u,v,x,y;
	for(v = 0;v < height; v++)
	{
		for(u = 0;u < width; u++)
		{
			x = u % 8;
			y = v % 8;
			if(QuantTbl[7 - y][x] != 0)
			{
				//pQuantResult[v * Width + u] = pFuvArray[v * Width + u] / QuantTbl[7 - y][x];
				/*QuantFile.Write(&pQuantResult[v * Width + u],sizeof(__int8));*/
				QuantFile.Write(&pQuantResult[v * width + u],sizeof(double));
			}
		}
	}
	QuantFile.Close();
	return TRUE;
}

//°ÑÎÄ¼þÖÐµÄÊý¾Ý¶ÁÈëÁ¿»¯½á¹û»º³åÇøpQuantResult[]
//¶ÔÓÚÁ¿»¯±íQuantTbl[][]ÖÐÎª0Êý¾ÝËù¶ÔÓ¦µÄÎ»ÖÃ£¬²¹Èë0¡£
//´ÓÎÄ¼þ¶ÁÈ¡Êý¾ÝÊ¾Àý£º
//CFile QuantFile("quantify.txt",CFile::modeReadWrite );
//QuantFile.Read(pQuantResult , n);
//QuantFile.Close();
BOOL CBMPEdit::IDCTRead()
{
	//add your code here================================================== 
	CFile QuantFile("quantify.txt",CFile::modeCreate|CFile::modeReadWrite );
	//QuantFile.Write( pQuantResult , COLUMN*ROW);
    int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;

	int u,v,x,y;
	for(v = 0;v < height; v++)
	{
		for(u = 0;u < width; u++)
		{
			x = u % 8;
			y = v % 8;
			if(QuantTbl[7 - y][x] != 0)
			{
				/*QuantFile.Read(&pQuantResult[v * Width + u],sizeof(__int8));*/
				QuantFile.Read(&pQuantResult[v * width + u],sizeof(double));
			}
			else
			{
				pQuantResult[v * width + u] = 0;
			}
		}
	}
	QuantFile.Close();
	
	return TRUE;
}

//¶ÔÎÄ¼þ¶ÁÈëµÄÊý¾ÝpQuantResult[]½øÐÐ·´Á¿»¯
//¼´¶ÔpQuantResult[]ÖÐµÄÊý¾Ý³ËÒÔÁ¿»¯±íQuantTbl[][]
//ÐèÒª¿¼ÂÇÁ¿»¯±í8*8µÄÎ»ÖÃ¶ÔÓ¦
//·´Á¿»¯½á¹û·ÅÈëpFuvArray[]ÖÐ
BOOL CBMPEdit::IDCTQuantify()
{
	//add your code here================================================== 
	int u,v,x,y;
	int height = m_BmpInfo.bmiHeader.biHeight;
	int width = m_BmpInfo.bmiHeader.biWidth;
	for(v = 0;v < height; v++)
	{
		for(u = 0;u < width; u++)
		{
			x = u % 8;
			y = v % 8;
		    pFuvArray[v * width + u] =  QuantTbl[7 - y][x] * pQuantResult[v * width + u];
		}
	}
	
	return TRUE;
}


BOOL CBMPEdit::IDCTTransform()
{
	int u,v,x,y,height,width;

	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth*m_BmpInfo.bmiHeader.biBitCount + 31)/32*4;//Í¼ÏñÃ¿ÐÐËùÕ¼Êµ¼Ê×Ö½ÚÊý£¨ÐëÊÇ4µÄ±¶Êý£©
    m_IDCTResult = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
    byte *pIDCTResult = (byte *)GlobalLock(m_IDCTResult);

	height = m_BmpInfo.bmiHeader.biHeight;
	width = m_BmpInfo.bmiHeader.biWidth;

    for (y=0;y<height/8;y++)
    {
		for (x=0;x<width/8;x++)
		{
			//prepare pFuv8
			for (v=0; v<8; v++)
			{
				for (u=0; u<8; u++)
				{
					pFuv8[v*8+u]=pFuvArray[y*8*width+v*width+x*8+u];
				}
			}

			//do IDCT
			IDCTTransform8();

			//get result
			for (v=0; v<8; v++)
			{
				for (u=0; u<8; u++)
				{
					*(pIDCTResult+y*8*width+v*width+x*8+u) = (byte)pImage8[v*8+u];

				}
			}
		}
    }


	delete [] pFuvArray;
	delete [] pImage8;
	delete [] pFuv8;

	GlobalUnlock(m_IDCTResult);
    return TRUE;
}


BOOL CBMPEdit::IDCTTransform8()
{
	int u,v,x,y;
    double Fuv,post;
	double pi=3.1415926;
	
    for (y=0;y<8;y++)
    {
		for (x=0;x<8;x++)
		{
			post = 0.0;
			for (v=0;v<8;v++)
			{
				for (u=0;u<8;u++)
				{
					Fuv=pFuv8[v*8+u];
					post = post + Fuv*cos(pi/2/8*(2*x+1)*u) * cos(pi/2/8*(2*y+1)*v);
				}
			}
			
			post = post +0.5;
			if (post>255)
			{
				post = 255.0;
			}
			if (post<0)
			{
				post = 0.0;
			}
			*(pImage8+y*8+x) = (byte)post;
		}
    }
	
	
	GlobalUnlock(m_IDCTResult);
    return TRUE;
}



BOOL CBMPEdit::PlayBmpIDCT(CDC *pDC, const CPoint &StartPoint)
{
	if(!m_bReady)
		return FALSE;
	byte *pData = (byte *)GlobalLock(m_IDCTResult);
	
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
		GlobalUnlock(m_IDCTResult);
		return FALSE;
	}
	
	GlobalUnlock(m_IDCTResult);
	return TRUE;
}

BOOL CBMPEdit::Error()
{
	int x,y,height,width;
	
	long nBytePerLine = (m_BmpInfo.bmiHeader.biWidth*m_BmpInfo.bmiHeader.biBitCount + 31)/32*4;//Í¼ÏñÃ¿ÐÐËùÕ¼Êµ¼Ê×Ö½ÚÊý£¨ÐëÊÇ4µÄ±¶Êý£©
    m_Error = GlobalAlloc(GHND, nBytePerLine * m_BmpInfo.bmiHeader.biHeight);
    byte *pError = (byte *)GlobalLock(m_Error);

    byte *pImageData = (byte *)GlobalLock(m_hgImageData);
	byte *pIDCTResult = (byte *)GlobalLock(m_IDCTResult);

	height = m_BmpInfo.bmiHeader.biHeight;
	width = m_BmpInfo.bmiHeader.biWidth;	
	
    for (y=0;y<height;y++)
    {
		for (x=0;x<width;x++)
		{
			*(pError+y*width+x) =abs( *(pIDCTResult+y*width+x) - *(pImageData+y*width+x))*10;
            if (*(pError+y*width+x)!=0)
            {
				//AfxMessageBox("Not Zero!");
            }

		}
    }
	
	
	GlobalUnlock(m_Error);
	GlobalUnlock(m_IDCTResult);
	GlobalUnlock(m_hgImageData);
    return TRUE;
}

BOOL CBMPEdit::PlayBmpError(CDC *pDC, const CPoint &StartPoint)
{
	if(!m_bReady)
		return FALSE;
	byte *pData = (byte *)GlobalLock(m_Error);
	
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
		GlobalUnlock(m_IDCTResult);
		return FALSE;
	}
	
	GlobalUnlock(m_IDCTResult);
	return TRUE;
}


