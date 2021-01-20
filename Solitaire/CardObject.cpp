#include "CardObject.h"
#include "CardContainer.h"

CardObject::CardObject() :
    hsx(0.f),
    hsy(0.f),
    ox(0.f),
    oy(0.f),
    xPos(0.f),
    yPos(0.f)
{
}

CardObject* CardObject::StartDrag(float x, float y)
{
    hsx = x - xPos;
    hsy = y - yPos;
    ox = xPos;
    oy = yPos;
    return this;
}

void CardObject::Return()
{
    Move(ox, oy);
}

size_t CardObject::Size()
{
    return 1;
}

void CardObject::EndDrag()
{
    return;
}

float CardObject::GetX()
{
    return xPos;
}

float CardObject::GetY()
{
    return yPos;
}


CardObject::~CardObject() {}