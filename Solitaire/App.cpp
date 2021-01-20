#include "App.h"
#include "CardObject.h"


BOOL DifferentColours(char a, char b)
{
    // consider some clauses
    //"is a red?"   a == DIAMONDS || a == HEARTS
    //"is b red?"   b == DIAMONDS || b == HEARTS
    //these must be unequal
    return (a == DIAMONDS || a == HEARTS) != (b == DIAMONDS || b == HEARTS);
}

App::App() :
    //windows things
    m_hwnd(NULL),
    m_pIWICFactory(NULL),
    m_pDirect2dFactory(NULL),
    m_pRenderTarget(NULL), 
    //card stuff
    drag(NULL),
    flip(NULL),
    dragging(FALSE),
    //pointers for card drawing
    m_pBack(NULL),
    m_pSourceBack(NULL),
    //modern computers have scaling these days
    DpiScale(0.f)
{
}


App::~App()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pIWICFactory);
    SafeRelease(&m_pRenderTarget);
}

void App::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT App::Initialise()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    //set up posiitons
    stack.Move(10, 10);
    hpile.Move(20 + Card::width, 10);
    for (int i = 0; i < 4; i++)
    {
        aces[i].Move(10 + (i+3) * (10 + Card::width), 10);
    }


    //deck
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 13; i++)
        {
            deck[i + 13 * j].Initialise(m_pIWICFactory, i+1, Card::suits[j], 0, 0, m_pSourceBack);
            //then add to stack (saves on iterating twice
            stack.Add(&deck[i + 13 * j]);
        }
    }

    //shuffle the deck
    stack.Shuffle();

    //move and deal to piles at the same time
    for (int i = 0; i < 7; i++)
    {
        piles[i].Move((Card::width + 10) * (i)+10, Card::height + 20);
        //remove the last however many cards to the pile
        stack.RemoveCards(7 - i, &piles[i]);
        //flip the bottom card;
        piles[i].back()->Flip();
    }
    

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = App::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);


        
        UINT dpi;
        dpi = GetDpiForSystem();

        // Create the window.
        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpi / 96.f)),
            static_cast<UINT>(ceil(480.f * dpi / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
        );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
        dpi = GetDpiForWindow(m_hwnd);
        DpiScale = 96.0f / dpi;
    }

    return hr;
}

HRESULT App::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
    
    if (FAILED(hr))
    {
        return hr;
    }
    // Retrieve the first frame of the image from the decoder
    

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pIWICFactory)
    );


    //do stuff with resources
    //some variables
    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void* pImageFile = NULL;
    DWORD imageFileSize = 0;
    IWICStream* pStream = NULL;

    //locate resource
    if (SUCCEEDED(hr))
    {
        imageResHandle = FindResource(HINST_THISCOMPONENT, _T("BACK"), _T("IMAGE"));
        hr = imageResHandle ? S_OK : E_FAIL;
    }

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
        hr = m_pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
        );
    }
    //CreateDecoder
    IWICBitmapDecoder* pDecoder = NULL;

    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = m_pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
        );
    }

    /*
    hr = m_pIWICFactory->CreateDecoderFromFilename(
        L"Assets/back.png",                      // Image to be decoded
        NULL,                            // Do not prefer a particular vendor
        GENERIC_READ,                    // Desired read access to the file
        WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
        &pDecoder                        // Pointer to the decoder
    );*/

    IWICBitmapFrameDecode* pFrame = NULL;

    if (SUCCEEDED(hr))
    {
        hr = pDecoder->GetFrame(0, &pFrame);
    }

    if (SUCCEEDED(hr))
    {
        SafeRelease(&m_pSourceBack);
        hr = m_pIWICFactory->CreateFormatConverter(&m_pSourceBack);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pSourceBack->Initialize(
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


    return hr;
}

HRESULT App::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
        );

    }

    

    return hr;
}

void App::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pSourceBack);
    SafeRelease(&m_pBack);
}

LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        App* pApp = (App*) pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pApp)
        );


        result = 1;
    }
    else
    {
        App* pApp = reinterpret_cast<App*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pApp)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pApp->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DPICHANGED:
            {
                int xDpi = LOWORD(wParam);
                int yDpi = HIWORD(wParam);
                pApp->OnDpiChanged(xDpi, yDpi);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                pApp->OnRender();
                ValidateRect(hwnd, NULL);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_LBUTTONDOWN:
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                pApp->OnLButtonDown(xPos, yPos, wParam);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_MOUSEMOVE:
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                pApp->OnMouseMove(xPos, yPos, wParam);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_LBUTTONUP:
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                pApp->OnLButtonUp(xPos, yPos, wParam);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

HRESULT App::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr))
    {
        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(0.15f, 0.47f, 0.08f));

        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);


        
        stack.Draw(m_pRenderTarget);
        hpile.Draw(m_pRenderTarget);

        for (int i = 0; i < 7; i++)
        {
            piles[i].Draw(m_pRenderTarget);
        }

        for (int i = 0; i < 4; i++)
        {
            aces[i].Draw(m_pRenderTarget);
        }

        if (drag) {
            drag->Draw(m_pRenderTarget);
        }


        hr = m_pRenderTarget->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
}

void App::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}

void App::OnDpiChanged(int xDpi, int yDpi)
{
    //x dpi = y dpi
    DpiScale = 96.f / xDpi;

    InvalidateRect(m_hwnd, NULL, FALSE);
}


void App::OnLButtonDown(int xPos, int yPos, DWORD FLAGS)
{
    //convert pixels to dips
    //get scale
    const float xDip = xPos * DpiScale;
    const float yDip = yPos * DpiScale;
    POINT pt = { xPos, yPos };
    if (DragDetect(m_hwnd, pt))
    {
        //if it hits the deck stack
        if (hpile.HitTest(xDip, yDip))
        {
            SetCapture(m_hwnd);
            drag = hpile.StartDrag(xDip, yDip);
            if (drag) dragging = TRUE;
            InvalidateRect(m_hwnd, NULL, FALSE);
        }
        else
        {
            //go through all the piles
            for (int i = 0; i < 7; i++)
            {
                if (piles[i].HitTest(xDip, yDip))
                {
                    //start drag and drop
                    SetCapture(m_hwnd);
                    drag = piles[i].StartDrag(xDip, yDip);
                    if (drag) dragging = TRUE; //check it's not picked up a null card
                    
                    //if this card reveals a flipped card store it otherwise make sure flipped is null
                    flip = piles[i].back() && piles[i].back()->IsFlipped() ? piles[i].back() : NULL;

                    InvalidateRect(m_hwnd, NULL, FALSE); //update the screen
                }
            }
            // go through aall the aces
            for (int i = 0; i < 4; i++)
            {
                if (aces[i].HitTest(xDip, yDip))
                {
                    SetCapture(m_hwnd);
                    drag = aces[i].StartDrag(xDip, yDip);
                    if (drag) dragging = TRUE;
                    InvalidateRect(m_hwnd, NULL, FALSE);
                }
            }
        }
    }
    else if (stack.HitTest(xDip, yDip)) //if we click the draw pile
    {
        //first check if it's empty
        if (stack.size() == 0)
        {
            //reset
            hpile.Reset(&stack);
        }
        else if (stack.size() < 3)
        {
            //put remaining cards in the hpile
            stack.RemoveCards(stack.size(), &hpile);
        }
        else
        {
            //put three cards in the hpile
            stack.RemoveCards(3, &hpile);
        }

        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

void App::OnMouseMove(int xPos, int yPos, DWORD FLAGS)
{
    //convert pixels to dips
    //get scale
    const float xDip = xPos * DpiScale;
    const float yDip = yPos * DpiScale;

    if (FLAGS & MK_LBUTTON)
    {
        if (dragging == TRUE)
        {
            drag->MoveDrag(xDip, yDip);
            InvalidateRect(m_hwnd, NULL, FALSE);    
        }
    }
}

void App::OnLButtonUp(int xPos, int yPos, DWORD FLAGS)
{
    //convert pixels to dips
    //get scale
    const float xDip = xPos * DpiScale;
    const float yDip = yPos * DpiScale;

    //hittest to find out what pile it's on
    if (dragging) {
        for (int i = 0; i < 7; i++)
        {
            //dropped on a pile
            if (piles[i].HitTest(xDip, yDip))
            {
                //do some logic to check validity
                //first we check if it's being dropped on another card or not;
                if (piles[i].back() == NULL && drag->Top()->GetRank() == KING || //if the pile is null the card must be a king
                    piles[i].back() != NULL &&          //otherwise the pile cannot be null
                    drag->Top()->GetRank() == piles[i].back()->GetRank() - 1 && //checks the ranks make sense
                    DifferentColours(drag->Top()->GetSuit(), piles[i].back()->GetSuit())) //checks the colours are different
                {
                    //WE'RE MOVING THE CARD



                    //move the card
                    drag->AddToContainer(&piles[i]);
                    drag->EndDrag();
                    dragging = FALSE;

                    //if the moved card reveals a flip card we must reveal it
                    if (flip != NULL)
                    {
                        flip->Flip();
                    }
                }
            }
        }
        if (dragging) {
            //if it's going on an ace there must be only one card
            if (drag->Size() == 1) {
                for (int i = 0; i < 4; i++)
                {
                    //dropped on a stack
                    if (aces[i].HitTest(xDip, yDip))
                    {
                        //do some logic to check validity
                        //first we check if it's being dropped on another card or not;
                        if (aces[i].back() == NULL && drag->Top()->GetRank() == ACE || //if the pile is null the card must be an ace
                            aces[i].back() != NULL &&          //otherwise the pile cannot be null
                            drag->Top()->GetRank() == aces[i].back()->GetRank() + 1 && //checks the ranks make sense
                            drag->Top()->GetSuit() == aces[i].back()->GetSuit()) // check suits
                        {
                            //WE'RE MOVING THE CARD



                            //move the card
                            drag->AddToContainer(&aces[i]);
                            drag->EndDrag();
                            dragging = FALSE;

                            //if the moved card reveals a flip card we must reveal it
                            if (flip != NULL)
                            {
                                flip->Flip();
                            }
                        }
                    }
                }
            }
        }
        if (dragging)
        {
            drag->Return();
            drag->EndDrag();
            dragging = FALSE;
        }
        drag = NULL;

        InvalidateRect(m_hwnd, NULL, FALSE);

        ReleaseCapture();
    }
    flip = NULL;   
}