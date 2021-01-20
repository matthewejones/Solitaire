#pragma once
#include "Pile.h"

class DragPile :
    public Pile
{
public:
    DragPile();
    DragPile* StartDrag(float x, float y, Pile* pDrag);
    void Return();
    void EndDrag();

private:
    Pile* oDrag = NULL;
};
