#include "Stack.h"
#include "Card.h"
#include "Pile.h"
#include "DragCard.h"

Stack::Stack() : pDrag(NULL)
{
}

void Stack::Update()
{
	//update the positions of the final card - the only one that will be drawn
	//only do this if there are cards
	if (cards.size() > 0) cards.back()->Move(xPos, yPos);
}

BOOL Stack::HitTest(float x, float y)
{
	return (y > yPos && y < yPos + height) && (x > xPos && x < xPos + width);
}

CardObject* Stack::StartDrag(float x, float y)
{
	if (cards.size() > 0)
	{
		DragCard* pDrag = new DragCard(this, cards.back());
		pDrag->StartDrag(x, y);
		cards.pop_back();
		return pDrag;
	}
	else
	{
		return NULL;
	}
}

void Stack::MoveDrag(float x, float y)
{
	xPos = x - hsx;
	yPos = y - hsy;
	Update();
}

void Stack::EndDrag()
{
	delete pDrag;
	pDrag = NULL;
}

void Stack::Draw(ID2D1HwndRenderTarget* pRenderTarget)
{
	//draw last card
	if (cards.size() > 0) cards.back()->Draw(pRenderTarget);
}

Stack::~Stack()
{
	CardContainer::~CardContainer();
	//just incase pDrag doesn't get deleted
	if (pDrag) delete pDrag;
}
