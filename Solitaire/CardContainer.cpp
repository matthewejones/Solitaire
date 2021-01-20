#include "CardContainer.h"
#include <algorithm>
#include <ctime>


CardContainer::CardContainer()
{
	std::srand(std::time(0));
}

//functions to return some basic parameters
std::vector<Card*>::size_type CardContainer::size()
{
	return cards.size();
}

std::vector<Card*>::iterator CardContainer::begin()
{
	return cards.begin();
}

std::vector<Card*>::iterator CardContainer::end()
{
	return cards.end();
}

void CardContainer::Add(Card* card)
{
	cards.push_back(card);
	Update();
}

Card* CardContainer::operator[] (int b)
{
	return cards[b];
}

Card* CardContainer::back()
{
	return cards.size() > 0 ? cards.back() : NULL;
}

Card* CardContainer::Top()
{
	return cards.size() > 0 ? cards[0] : NULL;
}

void CardContainer::Add(CardContainer* pAdd)
{
	//concatenate vectors
	Add(pAdd->begin(), pAdd->end());
}

void CardContainer::Add(std::vector<Card*>::iterator start, std::vector<Card*>::iterator end)
{
	//concatenate vectors
	cards.insert(cards.end(), start, end);
	Update();
}

void CardContainer::Move(float x, float y)
{
	xPos = x;
	yPos = y;
	ox = x;
	oy = y;
	Update();
}

void CardContainer::RemoveCards(size_t n, CardContainer* pRemoved)
{
	//return the cards removed in the pile passed
	//the removed cards will likely have to be drawn during drag and drop
	// first clear pile passed

	//check the list is big enough
	if (n < cards.size())
	{
		//get an iterator n from the end

		auto start = std::prev(cards.end(), n);
		// copy these elements into the new pile, using addRange
		pRemoved->Add(start, cards.end());
		//remove these from the vector
		cards.erase(start, cards.end());
		//move the position of the new pile
	}
	else
	{
		//if there aren't enough lets just do all of them
		pRemoved->Add(cards.begin(), cards.end());
		cards.erase(cards.begin(), cards.end());
	}
}

void CardContainer::AddToContainer(CardContainer* pAdd)
{
	pAdd->Add(this);
}

void CardContainer::Transfer(CardContainer* pTrans)
{
	AddToContainer(pTrans);
	cards.clear();
}

void CardContainer::clear()
{
	//clear the vector
	cards.clear();
}

void CardContainer::Shuffle()
{
	random_shuffle(cards.begin(), cards.end());
}

CardContainer::~CardContainer()
{
	clear();
}