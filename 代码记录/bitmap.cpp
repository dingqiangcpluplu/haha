hDib= (HDIB)GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(BITMAPINFOHEADER) + WIDTHBYTES(m_XLeng*8) *  m_YLeng * 3 + 1024);
		if (NULL == hDib)
		{
			DBG_PRINT(DBG_ERROR, ("Function [%s] failed to allocate required memory [%d] Bytes\n", 
				__FUNCTION__,
				sizeof(BITMAPINFOHEADER) + WIDTHBYTES(m_XLeng*8) *  m_YLeng * 3 + 1024));
			return FALSE; // Error
		}

		//BYTE*lpPal				= NULL;
		LPBITMAPINFOHEADER lpbi;
		lpbi					= (LPBITMAPINFOHEADER)GlobalLock(hDib);
		lpbi->biSize            = sizeof(BITMAPINFOHEADER);
		lpbi->biWidth           = m_XLeng;
		lpbi->biHeight          = -m_YLeng; // If biHeight is negative, the bitmap is a top-down DIB with the origin at the upper left corner.
		lpbi->biPlanes          = 1;
		lpbi->biBitCount        = 24;
		lpbi->biCompression     = BI_RGB;
		lpbi->biSizeImage       = WIDTHBYTES(m_XLeng*8)*m_YLeng*3;
		lpbi->biXPelsPerMeter   = 0;
		lpbi->biYPelsPerMeter   = 0;
		lpbi->biClrUsed         = 0;
		lpbi->biClrImportant    = 0;
		::GlobalUnlock((HGLOBAL) hDib);

		double dataHeight = m_nMaxPsudoValue - m_nMinPsudoValue;
		int nCountOfPsudeoMap = _countof(PseudoColorMap);
		LPSTR lpDIBHdr = (LPSTR)::GlobalLock((HGLOBAL)hDib);
		BYTE *lpPseudoColorBits = (BYTE *)FindDIBBits(lpDIBHdr);
		register int val;
		int start = center - window/2-1;
		float fRate = 256.0f/(float)window;
		for (int nColumnIndex = 0; nColumnIndex < m_XLeng; nColumnIndex++)
		{
			for (int nRowIndex = 0; nRowIndex < m_YLeng; nRowIndex++)
			{
				int nPixelIndexRowFrist = m_XLeng * nRowIndex + nColumnIndex;
				int nColorMapIndex = (int)((m_pData[nPixelIndexRowFrist] - m_nMinPsudoValue) / dataHeight * (double)(nCountOfPsudeoMap - 1));
				if (nColorMapIndex < 0)
				{
					nColorMapIndex = 0;
				}
				if (nColorMapIndex > nCountOfPsudeoMap - 1)
				{
					nColorMapIndex = nCountOfPsudeoMap - 1;
				}

				if (m_bPsudoData == NULL)
				{
					lpPseudoColorBits[nPixelIndexRowFrist * 3] = GetBValue(PseudoColorMap[nColorMapIndex]);
					lpPseudoColorBits[nPixelIndexRowFrist * 3 + 1] = GetGValue(PseudoColorMap[nColorMapIndex]);
					lpPseudoColorBits[nPixelIndexRowFrist * 3 + 2] = GetRValue(PseudoColorMap[nColorMapIndex]);
					//lpPseudoColorBits[nPixelIndexRowFrist * 4 + 3] = 255;
				}
				else
				{
					if (m_bPsudoData[nPixelIndexRowFrist])
					{
						lpPseudoColorBits[nPixelIndexRowFrist * 3] = GetBValue(PseudoColorMap[nColorMapIndex]);
						lpPseudoColorBits[nPixelIndexRowFrist * 3 + 1] = GetGValue(PseudoColorMap[nColorMapIndex]);
						lpPseudoColorBits[nPixelIndexRowFrist * 3 + 2] = GetRValue(PseudoColorMap[nColorMapIndex]);
						//lpPseudoColorBits[nPixelIndexRowFrist * 4 + 3] = 255;
					}
					else
					{
						val = static_cast<int>((m_pData[nPixelIndexRowFrist] - start)*fRate);
						if (val < 0)
						{
							val = 0;
						}
						if (val > 255)
						{
							val = 255;
						}

						lpPseudoColorBits[nPixelIndexRowFrist * 3] = (BYTE)val;
						lpPseudoColorBits[nPixelIndexRowFrist * 3 + 1] = (BYTE)val;
						lpPseudoColorBits[nPixelIndexRowFrist * 3 + 2] = (BYTE)val;
						//lpPseudoColorBits[nPixelIndexRowFrist * 4 + 3] = 255;
					}
				}

			}
		}

		::GlobalUnlock((HGLOBAL) hDib);