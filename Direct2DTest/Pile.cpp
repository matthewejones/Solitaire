#include "Pile.h"
#include "Card.h"
#include "DragPile.h"


Pile::Pile() :
	height(0.f),
	dPile(NULL)
{
}

float Pile::GetHeight() 
{
	return height;
}

void Pile::Add(Card* card)
{
	//overlapp top card
	if (cards.size() > 0)
	{
		cards.back()->Overlap();
	}
	//add t o vectore
	cards.push_back(card);
	//move the card into place
	card->Move(xPos, yPos + (size() - 1) * Card::height / 5);
	//update height
	height += Card::height / 5;
}

/*
//not needed because it should just call the add(range)
void Pile::Add(CardContainer* pAdd)
{
	
	//overlapp top card if it's there
	if (cards.size() > 0)
	{
		cards.back()->Overlap();
	}
	//while not completely efficient, i just call update here, a better way would be to just 
	//loop through the cards added, these piles are small and performance isn't too important in
	//this application
	CardContainer::Add(pAdd);
	//make sure all the cards added are overlapped
	if (pAdd->size() > 0)
	{
		for (auto it = pAdd->begin(); it != pAdd->end(); it++)
		{
			(*it)->Overlap();
		}
		pAdd->back()->Unoverlap();
	}
}*/

void Pile::Add(std::vector<Card*>::iterator start, std::vector<Card*>::iterator end)
{
	//overlap top card
	if (cards.size() > 1)
	{
		cards.back()->Overlap();
	}

	CardContainer::Add(start, end);
	for (auto it = start; it != end; it++)
	{
		(*it)->Overlap();
	}
	cards.back()->Unoverlap();
	
}

void Pile::RemoveCards(size_t n, CardContainer* pRemoved)
{
	
	

	CardContainer::RemoveCards(n, pRemoved);

	//unoverlap the final card
	if (cards.size() > 0) cards.back()->Unoverlap();
	//move the pile of removed cards to where the cards were
	pRemoved->Move(xPos, yPos + size() * Card::height / 5);
	height = (size() + 3) * Card::height / 5;
}

void Pile::Update()
{
	//update the positions of all the cards
	for (size_t i = 0; i < cards.size(); i++)
	{
		cards[i]->Move(xPos, yPos + (i * Card::height / 5));
	}
	height = (size() + 4) * Card::height / 5;
}

BOOL Pile::HitTest(float x, float y)
{
	return (y > yPos && y < yPos + height) && (x > xPos && x < xPos + width);
}

Pile* Pile::StartDrag(float x, float y)
{
	dPile = new DragPile;
	GetDrag(x, y, dPile);
	if (dPile->size() == 0)
	{
		delete dPile;
		return NULL;
	}
	dPile->StartDrag(x, y, this);
	return dPile;
}

void Pile::EndDrag()
{
	delete dPile;
}

void Pile::GetDrag(float x, float y, Pile* pRemoved)
{
	//hittest all the cards and work out which is being clicked
	for (size_t i = 0; i < size(); i++)
	{
		if (cards[i]->HitTest(x, y) &&
			!cards[i]->IsFlipped())	//don't start if the card is flipped over
		{
			RemoveCards(size() - i, pRemoved);
			return;
		}
	}
	//if we're still here, we shou
}

void Pile::MoveDrag(float x, float y)
{
	xPos = x - hsx;
	yPos = y - hsy;
	Update();
}

void Pile::Draw(ID2D1HwndRenderTarget* pRenderTarget)
{
	for (auto it = cards.begin(); it < cards.end(); it++)
	{
		(*it)->Draw(pRenderTarget);
	}
}


void Pile::clear()
{
	//unoverlap the cards
	if (cards.size() > 1)
	{
		auto end = --cards.end();
		
		for (std::vector<Card*>::iterator it = cards.begin(); it < end; it++)
		{
			(*it)->Unoverlap();
		}
		
	}
	//do parent stuff
	CardContainer::clear();
}
