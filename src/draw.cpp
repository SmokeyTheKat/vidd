#include <vidd/draw.hpp>

#include <vidd/terminal.hpp>
#include <vidd/utils.hpp>
#include <vidd/framebuffer.hpp>

namespace Cursor = Terminal::Cursor;

static Style sStyle;

void Draw::style(Style style) {
	sStyle = style;
}

void Draw::box(Vec2 tl, Vec2 size, WChar chr, FrameBuffer* fb) {
	if (!(tl >= 0 && tl + size <= fb->getSize())) return;

	Vec2 br = tl + size - Vec2(1, 1);

	Draw::hLine(tl, size.x, chr, fb);
	Draw::vLine(tl, size.y, chr, fb);
	Draw::hLine(br, -size.x, chr, fb);
	Draw::vLine(br, -size.y, chr, fb);
}

void Draw::box(Vec2 tl, Vec2 size, Draw::Line line, FrameBuffer* fb) {
	if (!(tl >= 0 && tl + size <= fb->getSize())) return;

	Vec2 br = tl + size - Vec2(1, 1);

	Draw::hLine(tl, size.x, line, fb);
	Draw::vLine(tl, size.y, line, fb);
	Draw::hLine(br, -size.x, line, fb);
	Draw::vLine(br, -size.y, line, fb);

	(*fb)[tl.y][tl.x] = Pixel(line.tlCorner, sStyle);
	(*fb)[tl.y][br.x] = Pixel(line.trCorner, sStyle);
	(*fb)[br.y][tl.x] = Pixel(line.blCorner, sStyle);
	(*fb)[br.y][br.x] = Pixel(line.brCorner, sStyle);
}

void Draw::filledBox(Vec2 tl, Vec2 size, WChar chr, FrameBuffer* fb) {
	if (!(tl >= 0 && tl + size <= fb->getSize())) return;

	Pixel pixel(chr, sStyle);

	for (auto row : fb->subArea(tl, size)) {
		std::fill(row.begin(), row.end(), pixel);
	}
}

void Draw::paintFormat(Vec2 tl, Vec2 size, Style format, FrameBuffer* fb) {
	if (!(tl >= 0 && tl + size <= fb->getSize())) return;

	for (auto row : fb->subArea(tl, size)) {
		for (auto& px : row) {
			px.style += format;
		}
	}
}

void Draw::vLine(Vec2 pos, int length, WChar chr, FrameBuffer* fb) {
	if (length < 0) {
		length = -length;
		pos.y -= length - 1;
	}

	if (pos.y < 0 || pos.y + length > fb->getSize().y) return;

	Pixel pixel(chr, sStyle);
	auto col = fb->getCol(pos.x);
	std::fill(
		col.begin() + pos.y,
		col.begin() + pos.y + length,
		pixel
	);
}

void Draw::vLine(Vec2 pos, int length, Draw::Line line, FrameBuffer* fb) {
	Draw::vLine(pos, length, line.vLine, fb);
};

void Draw::hLine(Vec2 pos, int length, WChar chr, FrameBuffer* fb) {
	if (length < 0) {
		length = -length;
		pos.x -= length - 1;
	}

	if (pos.x < 0 || pos.x + length > fb->getSize().x) return;

	Pixel pixel(chr, sStyle);
	auto row = fb->getRow(pos.y);
	std::fill(
		row.begin() + pos.x,
		row.begin() + pos.x + length,
		pixel
	);
}

void Draw::hLine(Vec2 pos, int length, Draw::Line line, FrameBuffer* fb) {
	Draw::hLine(pos, length, line.hLine, fb);
}

void Draw::text(Vec2 pos, WStringView text, FrameBuffer* fb) {
	Vec2 fbs = fb->getSize();
	if (!(pos.y >= 0 && pos < fbs)) return;

	int start = 0;
	if (pos.x < 0) {
		start = -pos.x;
	}

	auto row = fb->getRow(pos.y);
	for (int i = start; i < (int)text.length() && i + pos.x < fbs.x; i++) {
		row[i + pos.x] = Pixel(text[i], sStyle);
	}
}