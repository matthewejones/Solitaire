#pragma once
#include "CardObject.h"
#include "Card.h"
#include <vector>
//ABC for containers to allow polymorphism
//this will allow a lot more flexiility in the the rest of the program
class CardContainer :
    public CardObject
{
public:
	CardContainer();
	~CardContainer();

	virtual std::vector<Card*>::size_type size();
	virtual std::vector<Card*>::iterator begin();
	virtual std::vector<Card*>::iterator end();

	virtual Card* operator[] (int b);
	virtual Card* back();

	virtual Card* Top();

	virtual void Add(Card* card);
	virtual void Add(CardContainer* pAdd);
	virtual void RemoveCards(size_t n, CardContainer* pRemoved);
	virtual void Add(std::vector<Card*>::iterator start, std::vector<Card*>::iterator end);
	virtual void Move(float x, float y);

	virtual void AddToContainer(CardContainer* pAdd);

	virtual void Transfer(CardContainer* pTrans);

	virtual void Update() = 0;

	virtual void Shuffle();

	virtual void clear();

protected:
	std::vector<Card*> cards;
};

