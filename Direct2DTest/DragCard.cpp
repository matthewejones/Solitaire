#include "DragCard.h"
#include "CardContainer.h"

DragCard::DragCard(CardContainer* container, Card* pCard)
{
    card = pCard;
    Move(card->GetX(), card->GetY());
    pContainer = container;
}

DragCard::DragCard() :
    pContainer(NULL),
    card(NULL)
{}

void DragCard::MoveDrag(float x, float y)
{
    xPos = x - hsx;
    yPos = y - hsy;
    Update();
}

void DragCard::Move(float x, float y)
{
    xPos = x;
    yPos = y;
    Update();
}

void DragCard::EndDrag()
{
    pContainer->EndDrag();
}

void DragCard::Update()
{
    card->Move(xPos, yPos);
}

void DragCard::AddToContainer(CardContainer* pAdd)
{
    pAdd->Add(card);
}

BOOL DragCard::HitTest(float x, float y)
{
    return card->HitTest(x, y);
}

void DragCard::Draw(ID2D1HwndRenderTarget* pRenderTarget)
{
    card->Draw(pRenderTarget);
}

Card* DragCard::Top()
{
    return card;
}

void DragCard::Return()
{
    pContainer->Add(card);
}
