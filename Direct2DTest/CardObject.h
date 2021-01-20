#pragma once
#include "framework.h"
class CardContainer;
class Card;

//Called CardObject, this would probably be better named Dragable card object bc that's what it is
//it defines the methods required for something that can be drag and ddropped
class CardObject
{
public:
	CardObject();
	~CardObject();

	virtual CardObject* StartDrag(float x, float y);
	virtual void EndDrag();
	virtual void Return();

	virtual void MoveDrag(float x, float y) = 0;
	virtual void Move(float x, float y) = 0;

	virtual size_t Size();

	virtual void AddToContainer(CardContainer* pAdd) = 0;

	virtual BOOL HitTest(float x, float y) = 0;
	virtual void Draw(ID2D1HwndRenderTarget* pRenderTarget) = 0;
	//another important thing for the logic of dragging from pile to pile
	virtual Card* Top() = 0;

	virtual float GetX();
	virtual float GetY();

protected:
	float xPos;
	float yPos;

	float ox;
	float oy;

	float hsx;
	float hsy;
};

