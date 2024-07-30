#ifndef __VIDD_FRAMEBUFFER_HPP__
#define __VIDD_FRAMEBUFFER_HPP__

#include <vidd/pos.hpp>
#include <vidd/pixel.hpp>

#include <string>
#include <cassert>
#include <utility>

class IFrameBufferArea;
class FrameBuffer;
class FrameBufferSubArea;
class FrameBufferCol;
class FrameBufferRowPixelIterator;
class FrameBufferRow;
class FrameBufferColPixelIterator;
class FrameBufferRowIterator;

class FrameBufferCol {
	Pixel* mData;
	int mLength;
	int mOutterWidth;
	int mX;

public:
	FrameBufferCol(void);
	FrameBufferCol(Pixel* data, int length, int outterWidth, int x);

	Pixel& operator[](int y);

	int getX(void);

	FrameBufferRowPixelIterator begin(void);
	FrameBufferRowPixelIterator end(void);
};

class FrameBufferRowPixelIterator {
	FrameBufferCol* mCol;
	int mY = 0;

public:
	FrameBufferRowPixelIterator(FrameBufferCol* col, int y);

	Pixel& operator*(void);
	Pixel* operator->(void);

	FrameBufferRowPixelIterator& operator++(void);
	FrameBufferRowPixelIterator operator++(int);
	FrameBufferRowPixelIterator operator+(int v);

	friend bool operator==(const FrameBufferRowPixelIterator& lhs, const FrameBufferRowPixelIterator& rhs);
	friend bool operator!=(const FrameBufferRowPixelIterator& lhs, const FrameBufferRowPixelIterator& rhs);
};

class FrameBufferRow {
	Pixel* mData;
	int mLength;
	int mY = 0;

public:
	FrameBufferRow(void);
	FrameBufferRow(Pixel* data, int length);
	FrameBufferRow(Pixel* data, int length, int y);

	Pixel& operator[](int x);

	int getY(void);
	int length(void);

	FrameBufferColPixelIterator begin(void);
	FrameBufferColPixelIterator end(void);
};

class FrameBufferColPixelIterator {
	Pixel* mData;
	int mX = 0;

public:
	FrameBufferColPixelIterator(Pixel* data, int length, int y, int x);

	Pixel& operator*(void);
	Pixel* operator->(void);

	FrameBufferColPixelIterator& operator++(void);
	FrameBufferColPixelIterator operator++(int);
	FrameBufferColPixelIterator operator+(int v);

	friend bool operator==(const FrameBufferColPixelIterator& lhs, const FrameBufferColPixelIterator& rhs);
	friend bool operator!=(const FrameBufferColPixelIterator& lhs, const FrameBufferColPixelIterator& rhs);
};

class FrameBufferRowIterator {
	FrameBufferRow mRow;
	IFrameBufferArea* mFb;
	int mY;

public:
	FrameBufferRowIterator(IFrameBufferArea* fb, int y);

	void setRow(void);

	FrameBufferRow operator*(void);
	FrameBufferRow* operator->(void);
	FrameBufferRowIterator& operator++(void);
	FrameBufferRowIterator operator++(int);

	friend bool operator==(const FrameBufferRowIterator& lhs, const FrameBufferRowIterator& rhs);
	friend bool operator!=(const FrameBufferRowIterator& lhs, const FrameBufferRowIterator& rhs);
};

class IFrameBufferArea {
public:
	virtual ~IFrameBufferArea(void) = default;
	virtual Vec2 getSize(void) = 0;
	virtual FrameBufferRow getRow(int y) = 0;
	virtual FrameBufferCol getCol(int y) = 0;
	virtual FrameBufferRow collasped(void) = 0;
	virtual FrameBufferRowIterator begin(void) = 0;
	virtual FrameBufferRowIterator end(void) = 0;
	virtual FrameBufferRow operator[](int y) = 0;
};

class FrameBufferSubArea : public IFrameBufferArea {
	FrameBuffer* mFb = 0;
	Vec2 mPos;
	Vec2 mSize;

public:
	FrameBufferSubArea(FrameBuffer* fb, Vec2 pos, Vec2 size)
	: mFb(fb), mPos(pos), mSize(size) {};
	~FrameBufferSubArea(void) {};

	Vec2 getSize(void) { return mSize; };
	FrameBufferRow getRow(int y);
	FrameBufferCol getCol(int x);
	FrameBufferRow collasped(void);

	FrameBufferRow operator[](int y) { return getRow(y); };

	FrameBufferRowIterator begin(void) {
		return FrameBufferRowIterator(this, 0);
	};
	FrameBufferRowIterator end(void) {
		return FrameBufferRowIterator(this, mSize.y);
	};
};

class FrameBuffer : public IFrameBufferArea {
	Pixel* mData = 0;
	Vec2 mSize;

public:
	FrameBuffer(Vec2 size);
	~FrameBuffer(void);

	FrameBuffer(const FrameBuffer& other);
	FrameBuffer(FrameBuffer&& other);

	FrameBuffer& operator=(const FrameBuffer& other);
	FrameBuffer& operator=(FrameBuffer&& other);

	void resize(Vec2 size);

	Vec2 getSize(void) { return mSize; };

	void copy(FrameBuffer& other);
	void merge(FrameBuffer& other, Vec2 at);

	FrameBufferRow collasped(void);
	FrameBufferSubArea subArea(Vec2 pos, Vec2 size);

	FrameBufferRow getRow(int y);
	FrameBufferCol getCol(int x);

	void clear(void);

	FrameBufferRow operator[](int y) { return getRow(y); };

	FrameBufferRowIterator begin(void) {
		return FrameBufferRowIterator(this, 0);
	};
	FrameBufferRowIterator end(void) {
		return FrameBufferRowIterator(this, mSize.y);
	};
};

#include <vidd/impl/framebuffer.ipp>

#endif
