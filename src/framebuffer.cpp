#include <vidd/framebuffer.hpp>

#include <cassert>

FrameBufferRow FrameBufferSubArea::getRow(int y) {
//    assert(y >= 0 && y < mSize.y);
	Vec2 inSize = mFb->getSize();
	return FrameBufferRow(&mFb->collasped()[(mPos.y + y) * inSize.x + mPos.x], mSize.x, y);
}

FrameBufferCol FrameBufferSubArea::getCol(int x) {
	return {};
}

FrameBufferRow FrameBufferSubArea::collasped(void) {
	return {};
}

FrameBuffer::FrameBuffer(Vec2 size)
: mSize(size) {
	mData = new Pixel[mSize.x * mSize.y];
}

FrameBuffer::~FrameBuffer(void) {
	delete[] mData;
}

void FrameBuffer::resize(Vec2 size) {
	if (size.x < 0 || size.y < 0 || mSize.x < 0 || mSize.y < 0) return;

	FrameBuffer copy = *this;

	mSize = size;
	mData = new Pixel[size.x * size.y];

	Vec2 overlapSize = Vec2(
		std::min(size.x, copy.getSize().x),
		std::min(size.y, copy.getSize().y)
	);

	auto overlap = subArea(Vec2(0, 0), overlapSize);

	for (auto row : overlap) {
		auto origRow = copy.getRow(row.getY());
		std::copy(origRow.begin(),
				  origRow.begin() + overlapSize.x,
				  row.begin()
		);
	}
}

void FrameBuffer::copy(FrameBuffer& other) {
	if (other.getSize() != mSize) return;
	auto myBuf = collasped();
	auto otherBuf = other.collasped();
	std::copy(otherBuf.begin(), otherBuf.end(), myBuf.begin());
}

void FrameBuffer::merge(FrameBuffer& other, Vec2 at) {
	if (other.getSize() < 1) return;

	for (auto row : other) {
		std::copy(
			row.begin(),
			row.end(),
			this->getRow(row.getY() + at.y).begin() + at.x
		);
	}
}

FrameBufferSubArea FrameBuffer::subArea(Vec2 pos, Vec2 size) {
	return FrameBufferSubArea(this, pos, size);
}

FrameBufferRow FrameBuffer::collasped(void) {
	return FrameBufferRow(mData, mSize.x * mSize.y);
}

FrameBufferRow FrameBuffer::getRow(int y) {
//    assert(y >= 0 && y < mSize.y);
	return FrameBufferRow(&mData[y * mSize.x], mSize.x, y);
}

FrameBufferCol FrameBuffer::getCol(int x) {
//    assert(x >= 0 && x < mSize.x);
	return FrameBufferCol(mData, mSize.y, mSize.x, x);
}

void FrameBuffer::clear(void) {
	auto clspdBuf = collasped();
	std::fill(clspdBuf.begin(), clspdBuf.end(), Pixel());
}
