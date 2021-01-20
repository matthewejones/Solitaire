#pragma once
#include "CardContainer.h"
#include "DragCard.h"
#include <vector>

//class for showing cards overturned from the draw pile
class HPile :
    public CardContainer
{
public:
    HPile();
    ~HPile();

    void Add(std::vector<Card*>::iterator start, std::vector<Card*>::iterator end);

    CardObject* StartDrag(float x, float y);
    void MoveDrag(float x, float y);
    void EndDrag();

    void Update();

    void Reset(CardContainer* pContainer);

    BOOL HitTest(float x, float y);

    void Draw(ID2D1HwndRenderTarget* pRenderTarget);

private:
    DragCard* pDrag;
    BOOL dragging;
};

