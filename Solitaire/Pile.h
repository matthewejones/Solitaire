#pragma once
#include <vector>
#include "framework.h"
#include "CardContainer.h"

class DragPile;
class Card;


class Pile : public CardContainer
{
public:
	Pile();

	void Add(Card* card);
	//void Add(CardContainer* pAdd);
	void RemoveCards(size_t n, CardContainer* pRemoved);
	void Add(std::vector<Card*>::iterator start, std::vector<Card*>::iterator end);

	Pile* StartDrag(float x, float y);
	void EndDrag();

	void GetDrag(float x, float y, Pile* pRemoved);

	void MoveDrag(float x, float y);

	float GetHeight();
	void Update();

	BOOL HitTest(float x, float y);

	void Draw(ID2D1HwndRenderTarget* pRenderTarget);
	
	void clear();


private:
	const float width = 96.f;
	float height;
	DragPile* dPile;
};

