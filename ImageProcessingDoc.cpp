
// ImageProcessingDoc.cpp : Implement a CImageProcessingDoc Class
//

#include "stdafx.h"
#include "ImageProcessing.h"

#include "DlgBrightnessOption.h"
#include "DlgMosaicOption.h"
#include "DlgCompositeOption.h"
#include "DlgIntensityTransformOption.h"
#include "DlgContrastStretchOption.h"

#include "ImageProcessingDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
	ON_COMMAND(ID_PROCESS_MOSAIC, &CImageProcessingDoc::OnProcessMosaic)
	ON_COMMAND(ID_PROCESS_BRIGHTNESS, &CImageProcessingDoc::OnProcessBrightness)
	ON_COMMAND(ID_PROCESS_COMPOSITE, &CImageProcessingDoc::OnProcessSquidGame)
	ON_COMMAND(ID_PROCESS_INTENSITY_TRANSFORM, &CImageProcessingDoc::OnProcessIntensityTransform)
	ON_COMMAND(ID_PROCESS_CONTRAST_STRETCH, &CImageProcessingDoc::OnProcessContrastStretch)
	ON_COMMAND(ID_PROCESS_EQUALIZATION, &CImageProcessingDoc::OnProcessEqualization)
	ON_COMMAND(ID_32783, &CImageProcessingDoc::OnHistogramSpecification)
END_MESSAGE_MAP()


// CImageProcessingDoc Contruction/Destuction

CImageProcessingDoc::CImageProcessingDoc()
{
	//// TODO: Add an one-time generating code here
	m_pImage = NULL;
}

CImageProcessingDoc::~CImageProcessingDoc()
{
	if (NULL != m_pImage)
		delete m_pImage;
}

BOOL CImageProcessingDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO: load imagefile // DONE
	m_pImage = new CxImage;
	m_pImage->Load(lpszPathName, FindType(lpszPathName));

	CalculateHistogram();

	return TRUE;
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//// TODO: Add a re-initialization code here
	//// SDI documents will reuse this article

	return TRUE;
}




// CImageProcessingDoc serialization

void CImageProcessingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//// TODO: Add a saving code here
	}
	else
	{
		//// TODO: Add a loading code here
	}
}


// CImageProcessingDoc diagnosis

#ifdef _DEBUG
void CImageProcessingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageProcessingDoc command

CString CImageProcessingDoc::FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(i+1);
		}
	}
	return CString(_T(""));
}

CString CImageProcessingDoc::RemoveExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(0,i);
		}
	}
	return name;
}

int CImageProcessingDoc::FindType(const CString& ext)
{
	return CxImage::GetTypeIdFromName(ext);
}

void CImageProcessingDoc::CalculateHistogram()
{
	// TODO: Add a calculating histogram code here
	if (m_pImage) {
		// Histogram function, which is implemented in Cximage
		 //m_histogramMax = m_pImage->Histogram(m_histogramRed, m_histogramGreen, m_histogramBlue, m_histogramGray);

		 
		// 가짜코드
		for (int i = 0; i < 256; i++){
			m_histogramRed[i]   = rand() % 200;
			m_histogramGreen[i] = rand() % 200;
			m_histogramBlue[i]  = rand() % 200;
			m_histogramGray[i]  = rand() % 200;
		}

		m_histogramMax = 0;
		for (int i = 0; i < 256; i++){
			m_histogramMax = max(m_histogramMax, m_histogramRed[i]  );
			m_histogramMax = max(m_histogramMax, m_histogramGreen[i]);
			m_histogramMax = max(m_histogramMax, m_histogramBlue[i] );
			m_histogramMax = max(m_histogramMax, m_histogramGray[i] );
		}
		//////////////////////////////////////////////////////////////
	}
}

void CImageProcessingDoc::OnProcessBrightness()
{
	if (m_pImage) {
		DlgBrightnessOption dlg;

		if (dlg.DoModal() == IDOK) {

			int nPlusMinus = dlg.m_nPlusMinus;
			BYTE byModifyValue = dlg.m_byModifyValue;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();

			RGBQUAD color;
			RGBQUAD newcolor;

			for (DWORD y = 0; y < height; y++) {
				for (DWORD x = 0; x < width; x++) {

					// 현재 픽셀의 RGB 값을 가져온다.
					color = m_pImage->GetPixelColor(x, y);

					if (nPlusMinus == 0) {
						// 밝기 증가
						newcolor.rgbRed =
							min(255, color.rgbRed + byModifyValue);

						newcolor.rgbGreen =
							min(255, color.rgbGreen + byModifyValue);

						newcolor.rgbBlue =
							min(255, color.rgbBlue + byModifyValue);
					}
					else {
						// 밝기 감소
						newcolor.rgbRed =
							max(0, color.rgbRed - byModifyValue);

						newcolor.rgbGreen =
							max(0, color.rgbGreen - byModifyValue);

						newcolor.rgbBlue =
							max(0, color.rgbBlue - byModifyValue);
					}

					m_pImage->SetPixelColor(x, y, newcolor);
				}
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessMosaic()
{
	if (m_pImage) {
		DlgMosaicOption dlg;

		if (dlg.DoModal() == IDOK) {

			DWORD dwWindowSize = dlg.m_dwWindowSize;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();

			RGBQUAD color;
			RGBQUAD newcolor;

			// 0: Blurring
			// 1: Sharpening
			// 2 이상: Mosaic
			if (dwWindowSize == 0 || dwWindowSize == 1) {

				// 3x3 mask
				float mask[3][3];

				if (dwWindowSize == 0) {
					// Blurring mask
					for (int j = 0; j < 3; j++) {
						for (int i = 0; i < 3; i++) {
							mask[j][i] = 1.0f / 9.0f;
						}
					}
				}
				else {
					// Sharpening mask
					mask[0][0] = 0; mask[0][1] = -1; mask[0][2] = 0;
					mask[1][0] = -1; mask[1][1] = 5; mask[1][2] = -1;
					mask[2][0] = 0; mask[2][1] = -1; mask[2][2] = 0;
				}

				// Store processed pixels separately
				CxImage* buffer = new CxImage;
				buffer->Create(width, height, 24, CXIMAGE_FORMAT_BMP);

				for (DWORD y = 0; y < height; y++) {
					for (DWORD x = 0; x < width; x++) {

						float sumRed = 0;
						float sumGreen = 0;
						float sumBlue = 0;

						// Apply mask to the 3x3 neighborhood
						for (int j = 0; j < 3; j++) {
							for (int i = 0; i < 3; i++) {

								int xx = (int)x + i - 1;
								int yy = (int)y + j - 1;

								// Ignore pixels outside the image
								if (xx >= 0 && xx < (int)width &&
									yy >= 0 && yy < (int)height) {

									color = m_pImage->GetPixelColor(xx, yy);

									sumRed += color.rgbRed * mask[j][i];
									sumGreen += color.rgbGreen * mask[j][i];
									sumBlue += color.rgbBlue * mask[j][i];
								}
							}
						}

						// Clamp values to the valid RGB range
						newcolor.rgbRed = (BYTE)max(0, min(255, (int)sumRed));
						newcolor.rgbGreen = (BYTE)max(0, min(255, (int)sumGreen));
						newcolor.rgbBlue = (BYTE)max(0, min(255, (int)sumBlue));

						buffer->SetPixelColor(x, y, newcolor);
					}
				}

				// Copy the result back to the original image
				for (DWORD y = 0; y < height; y++) {
					for (DWORD x = 0; x < width; x++) {
						color = buffer->GetPixelColor(x, y);
						m_pImage->SetPixelColor(x, y, color);
					}
				}

				delete buffer;
			}
			else {
				// Original Mosaic implementation
				DWORD XStep = width / dwWindowSize;
				DWORD YStep = height / dwWindowSize;

				for (DWORD y = 0; y < height; y += YStep) {
					for (DWORD x = 0; x < width; x += XStep) {

						DWORD sumRed = 0;
						DWORD sumGreen = 0;
						DWORD sumBlue = 0;
						DWORD count = 0;

						for (DWORD yy = y;
							yy < y + YStep && yy < height; yy++) {
							for (DWORD xx = x;
								xx < x + XStep && xx < width; xx++) {

								color = m_pImage->GetPixelColor(xx, yy);

								sumRed += color.rgbRed;
								sumGreen += color.rgbGreen;
								sumBlue += color.rgbBlue;

								count++;
							}
						}

						newcolor.rgbRed = (BYTE)(sumRed / count);
						newcolor.rgbGreen = (BYTE)(sumGreen / count);
						newcolor.rgbBlue = (BYTE)(sumBlue / count);

						for (DWORD yy = y;
							yy < y + YStep && yy < height; yy++) {
							for (DWORD xx = x;
								xx < x + XStep && xx < width; xx++) {

								m_pImage->SetPixelColor(xx, yy, newcolor);
							}
						}
					}
				}
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessComposite()
{
	// TODO: Add a composite code here
	if (m_pImage) {
		DlgCompositeOption dlg;

		if (dlg.DoModal() == IDOK) {
			int nOperatorID = dlg.GetCompositeOperatorID();
			CxImage * pSecondImage = dlg.GetSecondImage();

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD firstColor;
			RGBQUAD secondColor;
			RGBQUAD newColor;

			for (DWORD y = 0; y < height; y++) {
				for (DWORD x = 0; x < width; x++) {
					firstColor = m_pImage->GetPixelColor(x, y);
					secondColor = pSecondImage->GetPixelColor(x, y);

					if (nOperatorID == 0) {		// Addition의 경우
						newColor.rgbBlue = min(255,
							firstColor.rgbBlue + secondColor.rgbBlue);

						newColor.rgbGreen = min(255,
							firstColor.rgbGreen + secondColor.rgbGreen);

						newColor.rgbRed = min(255,
							firstColor.rgbRed + secondColor.rgbRed);
					}
					else {						// Subtraction의 경우
						newColor.rgbBlue = max(0,
							firstColor.rgbBlue - secondColor.rgbBlue);

						newColor.rgbGreen = max(0,
							firstColor.rgbGreen - secondColor.rgbGreen);

						newColor.rgbRed = max(0,
							firstColor.rgbRed - secondColor.rgbRed);
					}

					m_pImage->SetPixelColor(x, y, newColor);
				}
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}


void CImageProcessingDoc::OnProcessSquidGame()
{
	// Select the three input images at once
	TCHAR szFiles[4096] = { 0 };

	CFileDialog dlg(
		TRUE,
		_T("png"),
		NULL,
		OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST |
		OFN_ALLOWMULTISELECT,
		_T("PNG Files (*.png)|*.png||"),
		AfxGetMainWnd()
	);

	dlg.m_ofn.lpstrFile = szFiles;
	dlg.m_ofn.nMaxFile = _countof(szFiles);

	if (dlg.DoModal() != IDOK)
		return;

	CString headPath;
	CString bodyPath;
	CString pointsPath;

	int selectedCount = 0;
	POSITION pos = dlg.GetStartPosition();

	// Identify each image by its filename
	while (pos != NULL) {
		CString path = dlg.GetNextPathName(pos);
		selectedCount++;

		int slash = max(
			path.ReverseFind(_T('\\')),
			path.ReverseFind(_T('/'))
		);

		CString fileName = path.Mid(slash + 1);
		fileName.MakeLower();

		if (fileName == _T("squid_head.png"))
			headPath = path;
		else if (fileName == _T("squid_body.png"))
			bodyPath = path;
		else if (fileName == _T("squid_points.png"))
			pointsPath = path;
	}

	// Check the selected files
	if (selectedCount != 3 ||
		headPath.IsEmpty() ||
		bodyPath.IsEmpty() ||
		pointsPath.IsEmpty()) {

		AfxMessageBox(
			_T("squid_head.png, squid_body.png, ")
			_T("squid_points.png 파일 세 개를 선택하세요.")
		);
		return;
	}

	// Load the three images
	CxImage head;
	CxImage body;
	CxImage points;

	if (!head.Load(headPath, CXIMAGE_FORMAT_PNG) ||
		!body.Load(bodyPath, CXIMAGE_FORMAT_PNG) ||
		!points.Load(pointsPath, CXIMAGE_FORMAT_PNG)) {

		AfxMessageBox(_T("이미지를 불러오지 못했습니다."));
		return;
	}

	DWORD width = head.GetWidth();
	DWORD height = head.GetHeight();

	// The three images must have the same canvas size
	if (body.GetWidth() != width ||
		body.GetHeight() != height ||
		points.GetWidth() != width ||
		points.GetHeight() != height) {

		AfxMessageBox(
			_T("세 이미지의 가로 및 세로 크기가 같아야 합니다.")
		);
		return;
	}

	// Create the output image
	CxImage* result = new CxImage;

	if (!result->Create(
		width, height, 24, CXIMAGE_FORMAT_BMP)) {

		delete result;
		AfxMessageBox(_T("결과 이미지를 생성하지 못했습니다."));
		return;
	}

	RGBQUAD headColor;
	RGBQUAD bodyColor;
	RGBQUAD pointsColor;
	RGBQUAD newColor;

	// 1. Add squid_head and squid_body
	// 2. Subtract squid_points from the result
	for (DWORD y = 0; y < height; y++) {
		for (DWORD x = 0; x < width; x++) {

			headColor = head.GetPixelColor(x, y);
			bodyColor = body.GetPixelColor(x, y);
			pointsColor = points.GetPixelColor(x, y);

			// Addition: clamp each channel to 255
			int red = min(
				255,
				(int)headColor.rgbRed +
				(int)bodyColor.rgbRed
			);

			int green = min(
				255,
				(int)headColor.rgbGreen +
				(int)bodyColor.rgbGreen
			);

			int blue = min(
				255,
				(int)headColor.rgbBlue +
				(int)bodyColor.rgbBlue
			);

			// Subtraction: clamp each channel to 0
			newColor.rgbRed = (BYTE)max(
				0, red - (int)pointsColor.rgbRed
			);

			newColor.rgbGreen = (BYTE)max(
				0, green - (int)pointsColor.rgbGreen
			);

			newColor.rgbBlue = (BYTE)max(
				0, blue - (int)pointsColor.rgbBlue
			);

			result->SetPixelColor(x, y, newColor);
		}
	}

	// Replace the current document image with the result
	if (m_pImage != NULL)
		delete m_pImage;

	m_pImage = result;

	CalculateHistogram();
	UpdateAllViews(NULL);
}


void CImageProcessingDoc::OnProcessIntensityTransform()
{
	// TODO: Add a Contrast conversion code here
	if (m_pImage) {
		DlgIntensityTransformOption dlg;

		if (dlg.DoModal() == IDOK) {
			int nTransformType = dlg.m_nTransformType;
			double nLinearTransfromCoefficient_a = dlg.m_nLinearTransfromCoefficient_a;
			double nLinearTransfromCoefficient_b = dlg.m_nLinearTransfromCoefficient_b;

			int byThresholdValue = dlg.m_byThresholdValue;

			BYTE byContrastStretching_Start = dlg.m_byContrastStretching_Start;
			BYTE byContrastStretching_End = dlg.m_byContrastStretching_End;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD color;
			RGBQUAD newcolor;
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessContrastStretch()
{
	// TODO: Add a Contrast stretching code here
	if (m_pImage) {
		DlgContrastStretchOption dlg;

		if (dlg.DoModal() == IDOK) {
			int nStretchType = dlg.m_nStretchType;
			float fLow = dlg.m_fLow;
			float fHigh = dlg.m_fHigh;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD color;
			RGBQUAD newcolor;

			BYTE LUT[256];

			// initialize thresholds
			int nLowTherhs = 0;
			int nHighThresh = 255;


			// compute thresholds
			if (nStretchType == 0) { // Auto

			} else if (nStretchType == 1) { // Ends-In

			}


			// compute LUT


			// trnasfer image
			for (DWORD y = 0; y < height; y++) {
				for (DWORD x = 0; x < width; x++) {
					color = m_pImage->GetPixelColor(x, y);

					// using LUT

					m_pImage->SetPixelColor(x, y, newcolor);
				}
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessEqualization()
{
	// TODO: Add a Histogram equalization code here
	if (m_pImage) 
	{

		DWORD width = m_pImage->GetWidth();
		DWORD height = m_pImage->GetHeight();
		RGBQUAD color;	// Save the current color value
		RGBQUAD newcolor;	// After the conversion, save the color value

		//(1) Histogram has already been generated histogram(Omission) 
		//(2) Create a cumulative histogram
		DWORD sum = 0;
		float scale_factor = 255.0 / (width*height);
		DWORD sum_hist[256]; // a cumulative histogram

		for(int i=0; i<256 ; i++)
		{
		// (Coding)

		}


		//(3) Calculating look-up table
		BYTE LUT[256]; // look-up table

		for(int i=0; i<256 ; i++)
		{
		// (Coding)  


		}

		// (4) Image Conversion
		for (DWORD y = 0; y < height; y++) 
		{
			for (DWORD x = 0; x < width; x++) 
			{
				color = m_pImage->GetPixelColor(x, y);
				// (Coding)



				m_pImage->SetPixelColor(x, y, newcolor);
			}
		}		
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}



void CImageProcessingDoc::OnHistogramSpecification()
{
	// TODO: Add a Histogram specifications code here
	if (m_pImage) 
	{

		DWORD width = m_pImage->GetWidth();
		DWORD height = m_pImage->GetHeight();
		RGBQUAD color;	// Save the current color value
		RGBQUAD newcolor;	// After the conversion, save the color value

		//(1) Histogram has already been generated histogram(Omission) 
		//(2) Create a cumulative histogram
		DWORD sum = 0;
		float scale_factor = 255.0 / (width*height);
		DWORD sum_hist[256]; // a cumulative histogram

		for(int i=0; i<256 ; i++)
		{
		// (coding)

		}


		//(3) Calculating look-up table
		BYTE LUT[256]; // look-up table

		for(int i=0; i<256 ; i++)
		{
		// (coding)  


		}


		// Same as histogram equalization code from here.
		// However, After a cumulative histogram convert to float sum_hist[256] and
		// modifing to be entered in the real number on calculating sum_hist


		//(4) 
		DWORD desired_histogram[256];
		// Making desired_histogram
/*
		//1.
		for (int i=0; i<=255; i++)
		{
			desired_histogram[i]=i;
		}
*/
/*
		//2.
		for (int i=0; i<=255; i++)
		{
			desired_histogram[i]=255-i;
		}
*/
/*
		//3.		
		for (int i=0; i<=127; i++)
		{
			desired_histogram[i]=127-i;
		}
		for (int i=128; i<=255; i++)
		{
			desired_histogram[i]=i-127;
		}
*/

		// redefinition
		sum = 0;
		scale_factor = 255.0 / (width*height);

		//(5) Create the cumulative histogram of histogram you want
		for(int i=0; i<256; i++)
		{
			//(coding)



		}

		//(6) Is the inverse.
		float difference;	// calculate the difference
		int min;	// Calculate the minimum of the difference
		DWORD inv_hist[256];	// reverse-histogram

		//(Hint) will use dual-route.
		// float fabs() Use library functions.
		for(int i=0; i<256; i++)
		{
		//(coding)




		}

		//(7) Create look-up table of reverse-histogram
		for(int i=0; i<256 ; i++)
		{
		//(coding)

		}
		
		

		// (8) Image Conversion
		for (DWORD y = 0; y < height; y++) 
		{
			for (DWORD x = 0; x < width; x++) 
			{
				color = m_pImage->GetPixelColor(x, y);
				// (coding)



				m_pImage->SetPixelColor(x, y, newcolor);
			}
		}		
	} // if(m_pImage)

	CalculateHistogram();
	UpdateAllViews(NULL);

	

}
