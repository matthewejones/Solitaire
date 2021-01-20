#pragma once
#include <vector>
#include "CardContainer.h"

class Card;
class DragCard;

class Stack : public CardContainer
{
public:
	Stack();
	~Stack();

	CardObject* StartDrag(float x, float y);
	void EndDrag();

	void MoveDrag(float x, float y);

	void Update();

	BOOL HitTest(float x, float y);

	void Draw(ID2D1HwndRenderTarget* pRenderTarget);

private:
	DragCard* pDrag;
	const float width = 96.f;
	const float height = 134.f;
};

