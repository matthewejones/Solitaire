#pragma once
#include "CardObject.h"
class Card;
class CardContainer;

//wrapper for card for the purpose of draggin - will have the functionality to return to parent container
//just take Card Container with only one card;
class DragCard :
    public CardObject
{
public:
	DragCard(CardContainer* container, Card* pCard);
	DragCard();

	void EndDrag();
	void Return();
	void Update();

	Card* Top();

	void MoveDrag(float x, float y);
	void Move(float x, float y);

	void AddToContainer(CardContainer* pAdd);

	BOOL HitTest(float x, float y);
	void Draw(ID2D1HwndRenderTarget* pRenderTarget);

private:
    Card* card;
	CardContainer* pContainer;
};

