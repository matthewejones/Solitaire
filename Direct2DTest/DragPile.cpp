#include "DragPile.h"
DragPile::DragPile() : oDrag(NULL)
{
}

DragPile* DragPile::StartDrag(float x, float y, Pile* pDrag)
{
	CardObject::StartDrag(x, y);
	oDrag = pDrag;
	return this;
}

void DragPile::Return()
{
	CardObject::Return();
	Transfer(oDrag);
}

void DragPile::EndDrag()
{
	oDrag->EndDrag();
}