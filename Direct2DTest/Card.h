#pragma once
#include "framework.h"
#include "SafeRelease.h"
#include "CardObject.h"

//For loading the pile

class CardContainer;

//some useful definitions
#define SPADES 's'
#define CLUBS 'c'
#define DIAMONDS 'd'
#define HEARTS 'h'

#define ACE 1
#define JACK 11
#define QUEEN 12
#define KING 13

class Card : public CardObject
{
public:
	static const char suits[4];
	static const int iwidth = 500;
	static const int iheight = 726;
	static float width;
	static float height;


	Card();
	~Card();
	//Given an IWICClass factory loads a bitmap for the face
	HRESULT Initialise(IWICImagingFactory* pIWICFactory, int pRank, char pSuit, float x, float y, IWICFormatConverter* pConvertedSourceBack = NULL, BOOL faceUp = FALSE);
	void Draw(ID2D1HwndRenderTarget* pRenderTarget);
	void MoveDrag(float x, float y);
	void Move(float x, float y);
	void AddToContainer(CardContainer* pAdd);
	void Overlap();
	void Unoverlap();
	void Flip();
	BOOL IsFlipped();
	int GetRank();
	char GetSuit();
	Card* Top();

	BOOL HitTest(float x, float y);

private:
	
	float drawHeight;
	int rank;
	char suit;
	BOOL flipped;
	BOOL overlapped;
	ID2D1Bitmap* m_pBack;
	IWICFormatConverter* m_pConvertedSourceBack;
	ID2D1Bitmap* m_pD2DBitmap;
	IWICFormatConverter* m_pConvertedSourceBitmap;
};

