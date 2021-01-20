#pragma once
#include "framework.h"
#include "Card.h"
#include "Pile.h"
#include "Stack.h"
#include "SafeRelease.h"
#include "HPile.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class App
{
public:
    App();
    ~App();

    HRESULT Initialise();

    void RunMessageLoop();

private:
    HRESULT CreateDeviceResources();
    HRESULT CreateDeviceIndependentResources();


    void DiscardDeviceResources();

    HRESULT OnRender();

    void OnResize(UINT width, UINT height);
    void OnDpiChanged(int xDpi, int yDpi);
    void OnLButtonDown(int xPos, int yPos, DWORD FLAGS);
    void OnMouseMove(int xPos, int yPos, DWORD FLAGS);
    void OnLButtonUp(int xPos, int yPos, DWORD FLAGS);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);

    HWND m_hwnd;
    IWICImagingFactory* m_pIWICFactory;
    BOOL dragging;
    CardObject* drag;
    HPile hpile;
    Stack stack;
    Stack aces[4];
    Pile piles[7];
    Card deck[52];
    Card* flip;
    float DpiScale;
    IWICFormatConverter* m_pSourceBack;
    ID2D1Bitmap* m_pBack;
    ID2D1Factory* m_pDirect2dFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
};

