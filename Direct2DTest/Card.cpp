#include <sstream>
#include "Card.h"
#include "Pile.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#ifdef UNICODE
typedef std::wostringstream tstringstream;
#else
typedef std::ostringstream tstringstream;
#endif


float Card::height = 134.f;
float Card::width = 96.f;
const char Card::suits[4] = { SPADES, CLUBS, DIAMONDS, HEARTS };

Card::Card() :
    rank(0),
    suit(SPADES),
    drawHeight(height),
    m_pBack(NULL),
    m_pConvertedSourceBack(NULL),
    m_pD2DBitmap(NULL),
    m_pConvertedSourceBitmap(NULL),
    flipped(false),
    overlapped(false)
{
}

HRESULT Card::Initialise(IWICImagingFactory *pIWICFactory, int pRank, char pSuit, float x, float y, IWICFormatConverter* pConvertedSourceBack, BOOL faceUp)
{
    xPos = x;
    yPos = y;

    rank = pRank;
    suit = pSuit;

    m_pConvertedSourceBack = pConvertedSourceBack;

    //do stuff with resources
    //some variables
    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void* pImageFile = NULL;
    DWORD imageFileSize = 0;
    IWICStream* pStream = NULL;

    //get resource name
    tstringstream ss;

    ss << (TCHAR) _toupper(suit) << rank;
    auto stre = ss.str();

    //locate resource
    imageResHandle = FindResource(HINST_THISCOMPONENT, ss.str().c_str(), _T("IMAGE"));
    HRESULT hr = imageResHandle ? S_OK : E_FAIL;

    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
        hr = imageResDataHandle ? S_OK : E_FAIL;
    }

    //lock the resource
    if (SUCCEEDED(hr))
    {
        //lock resource
        pImageFile = LockResource(imageResDataHandle);
        hr = pImageFile ? S_OK : E_FAIL;
    }

    //calculate size
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
        hr = imageFileSize ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
        );
    }

    IWICBitmapDecoder* pDecoder = NULL;

    

    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
        );
    }

    //pointer to frame
    IWICBitmapFrameDecode* pFrame = NULL;

    if (SUCCEEDED(hr))
    {
        //get frame from decoder
        hr = pDecoder->GetFrame(0, &pFrame);
    }

    if (SUCCEEDED(hr))
    {
        // release source
        SafeRelease(&m_pConvertedSourceBitmap);
        hr = pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
    }

    if (SUCCEEDED(hr))
    {
        //initialise this thing
        hr = m_pConvertedSourceBitmap->Initialize(
            pFrame,                          // Input bitmap to convert
            GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
            WICBitmapDitherTypeNone,         // Specified dither pattern
            NULL,                            // Specify a particular palette 
            0.f,                             // Alpha threshold
            WICBitmapPaletteTypeCustom       // Palette translation type
        );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pStream);

    if (!faceUp)
    {
        Flip();
    }

    return hr;
}

Card::~Card() {}

void Card::Draw(ID2D1HwndRenderTarget* pRenderTarget)
{
    //rectangle on screen
	D2D1_RECT_F rectangle = D2D1::RectF(
		xPos,
		yPos,
		xPos + width,
		yPos + drawHeight
	);
    //rectangle in bitmap
    D2D1_RECT_F sourceRectangle = D2D1::RectF(
        0,
        0,
        iwidth,
        iheight * drawHeight / height
    );

    if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
    {
        //create a d2d bitmap from the wic bitmap
        pRenderTarget->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
    }

    if (m_pD2DBitmap)
    {
        //draw our d2d bitmap on the screen
        pRenderTarget->DrawBitmap(m_pD2DBitmap, &rectangle, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &sourceRectangle);
    }
}

void Card::Overlap()
{
    overlapped = TRUE;
}

void Card::Unoverlap()
{
    overlapped = FALSE;
}

void Card::Flip()
{
    flipped = !flipped;
    //swap the pictures around
    IWICFormatConverter* pSwapSource = m_pConvertedSourceBitmap;
    m_pConvertedSourceBitmap = m_pConvertedSourceBack;
    m_pConvertedSourceBack = pSwapSource;

    ID2D1Bitmap* pSwap = m_pD2DBitmap;
    m_pD2DBitmap = m_pBack;
    m_pBack = pSwap;
}

BOOL Card::HitTest(float x, float y)
{
    if (overlapped) {
        //check width and height
        return (x > xPos && x < xPos + width) && (y > yPos && y < yPos + height / 5);
    }
    return (x > xPos && x < xPos + width) && (y > yPos && y < yPos + height);
}


void Card::MoveDrag(float x, float y)
{
    xPos = x - hsx;
    yPos = y - hsy;
}

void Card::AddToContainer(CardContainer* pAdd)
{
    pAdd->Add(this);
    //note this doesn't stop the card being rendered
}

Card* Card::Top()
{
    return this;
}

int Card::GetRank()
{
    return rank;
}

char Card::GetSuit()
{
    return suit;
}

BOOL Card::IsFlipped()
{
    return flipped;
}

void Card::Move(float x, float y)
{
    xPos = x;
    yPos = y;
    ox = x;
    oy = y;
}