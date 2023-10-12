#ifndef __VIDD_FRAMEBUFFER_IPP__
#define __VIDD_FRAMEBUFFER_IPP__

inline FrameBufferColPixelIterator::FrameBufferColPixelIterator(Pixel* data, int length, int y, int x)
: mData(data), mX(x) {
}

inline Pixel& FrameBufferColPixelIterator::operator*(void) {
	return mData[mX];
}

inline Pixel* FrameBufferColPixelIterator::operator->(void) {
	return &mData[mX];
}

inline FrameBufferColPixelIterator& FrameBufferColPixelIterator::operator++(void) {
	mX++; return *this;
}

inline FrameBufferColPixelIterator FrameBufferColPixelIterator::operator++(int) {
	FrameBufferColPixelIterator tmp = *this;
	mX++;
	return tmp;
}

inline FrameBufferColPixelIterator FrameBufferColPixelIterator::operator+(int v) {
	mX += v; return *this;
}

inline bool operator==(const FrameBufferColPixelIterator& lhs, const FrameBufferColPixelIterator& rhs) {
	return lhs.mX == rhs.mX;
}

inline bool operator!=(const FrameBufferColPixelIterator& lhs, const FrameBufferColPixelIterator& rhs) {
	return lhs.mX != rhs.mX;
}

/////////////////////////////////////////////////////////

inline FrameBufferRow::FrameBufferRow(void) {
}

inline FrameBufferRow::FrameBufferRow(Pixel* data, int length)
: mData(data), mLength(length) {
}

inline FrameBufferRow::FrameBufferRow(Pixel* data, int length, int y)
: mData(data), mLength(length), mY(y) {
}

inline Pixel& FrameBufferRow::operator[](int x) {
//    assert(x >= 0 && x < mLength);
	return mData[x];
}

inline int FrameBufferRow::getY(void) {
	return mY;
}

inline int FrameBufferRow::length(void) {
	return mLength;
}

inline FrameBufferColPixelIterator FrameBufferRow::begin(void) {
	return FrameBufferColPixelIterator(mData, mLength, mY, 0);
}
inline FrameBufferColPixelIterator FrameBufferRow::end(void) {
	return FrameBufferColPixelIterator(mData, mLength, mY, mLength);
}

/////////////////////////////////////////////////////////

inline FrameBufferRowIterator::FrameBufferRowIterator(IFrameBufferArea* fb, int y)
: mFb(fb), mY(y) {
	setRow();
}

inline void FrameBufferRowIterator::setRow(void) {
	if (mY < mFb->getSize().y)
		mRow = mFb->getRow(mY);
}

inline FrameBufferRow FrameBufferRowIterator::operator*(void) {
	return mRow;
}

inline FrameBufferRow* FrameBufferRowIterator::operator->(void) {
	return &mRow;
}

inline FrameBufferRowIterator& FrameBufferRowIterator::operator++(void) {
	mY++;
	setRow();
	return *this;
}

inline FrameBufferRowIterator FrameBufferRowIterator::operator++(int) {
	FrameBufferRowIterator tmp = *this;
	mY++;
	setRow();
	return tmp;
}

inline bool operator==(const FrameBufferRowIterator& lhs, const FrameBufferRowIterator& rhs) {
	return lhs.mY == rhs.mY;
}

inline bool operator!=(const FrameBufferRowIterator& lhs, const FrameBufferRowIterator& rhs) {
	return lhs.mY != rhs.mY;
}

/////////////////////////////////////////////////////////

inline FrameBufferRowPixelIterator::FrameBufferRowPixelIterator(FrameBufferCol* col, int y)
: mCol(col), mY(y) {
}

inline Pixel& FrameBufferRowPixelIterator::operator*(void) {
	return (*mCol)[mY];
}

inline Pixel* FrameBufferRowPixelIterator::operator->(void) {
	return &(*mCol)[mY];
}

inline FrameBufferRowPixelIterator& FrameBufferRowPixelIterator::operator++(void) {
	mY++; return *this;
}

inline FrameBufferRowPixelIterator FrameBufferRowPixelIterator::operator++(int) {
	FrameBufferRowPixelIterator tmp = *this;
	mY++;
	return tmp;
}

inline FrameBufferRowPixelIterator FrameBufferRowPixelIterator::operator+(int v) {
	mY += v; return *this;
}

inline bool operator==(const FrameBufferRowPixelIterator& lhs, const FrameBufferRowPixelIterator& rhs) {
	return lhs.mY == rhs.mY;
}

inline bool operator!=(const FrameBufferRowPixelIterator& lhs, const FrameBufferRowPixelIterator& rhs) {
	return lhs.mY != rhs.mY;
}

/////////////////////////////////////////////////////////

inline FrameBufferCol::FrameBufferCol(void) {
}

inline FrameBufferCol::FrameBufferCol(Pixel* data, int length, int outterWidth, int x)
: mData(data), mLength(length), mOutterWidth(outterWidth), mX(x) {
}

inline Pixel& FrameBufferCol::operator[](int y) {
	return mData[y * mOutterWidth + mX];
}

inline int FrameBufferCol::getX(void) {
	return mX;
}

inline FrameBufferRowPixelIterator FrameBufferCol::begin(void) {
	return FrameBufferRowPixelIterator(this, 0);
}

inline FrameBufferRowPixelIterator FrameBufferCol::end(void) {
	return FrameBufferRowPixelIterator(this, mLength);
}

#endif
