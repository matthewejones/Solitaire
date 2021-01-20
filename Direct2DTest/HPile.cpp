#include "HPile.h"
#include <algorithm>

HPile::HPile() : dragging(FALSE), pDrag(NULL)
{
}

void HPile::Add(std::vector<Card*>::iterator start, std::vector<Card*>::iterator end)
{
	//ranges are added in reverse using reverse iterators
	//some corrections - rstart and rend need incrementing
	
	//define some reverse iterators
	//create a vector of the range;
	std::vector<Card*> range(start, end);
	//flip everything in range
	for (auto it = range.begin(); it != range.end(); it++)
	{
		(*it)->Flip();
	}

	cards.insert(cards.end(), range.rbegin(), range.rend());
	Update();
}

CardObject* HPile::StartDrag(float x, float y)
{
	if (cards.size() > 0)
	{
		dragging = TRUE;
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

void HPile::MoveDrag(float x, float y)
{
	xPos = x - hsx;
	yPos = y - hsy;
	Update();
}

void HPile::EndDrag()
{
	delete pDrag;
	dragging = FALSE;
	pDrag = NULL;
}

void HPile::Update()
{
	//the top three are the only ones displayed so update their positions

	for (std::vector<Card*>::size_type i = 1; i <= size(); i++)
	{	
		cards[size() - i]->Move(xPos + (3 - i) * Card::width / 4, yPos);
	}
}

BOOL HPile::HitTest(float x, float y)
{
	//just hittest the final card, this is the only one which is "active"
	if (size() > 0)
	{
		return cards.back()->HitTest(x, y);
	}
	return NULL;
}

void HPile::Draw(ID2D1HwndRenderTarget* pRenderTarget)
{
	//draw the last three cards in order from left to right
	for (std::vector<Card*>::iterator it = std::prev(cards.end(), (dragging ? min(size(), 2) : min(size(), 3))); it != cards.end(); it++)
	{
		(*it)->Draw(pRenderTarget);
	}
}

void HPile::Reset(CardContainer* pContainer)
{
	//all cards are flipped individually anyway so I need to iterate
	//might as well iterate through backwards
	for (std::vector<Card*>::reverse_iterator it = cards.rbegin(); it != cards.rend(); it++)
	{
		//flip card
		(*it)->Flip();
		//add to pContainer
		pContainer->Add(*it);
	}
	clear();
	Update();
}

HPile::~HPile()
{
	if (pDrag) delete pDrag;
}