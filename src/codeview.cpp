#include <vidd/codeview.hpp>

#include <vidd/texteditor.hpp>
#include <vidd/log.hpp>
#include <vidd/cursor.hpp>
#include <vidd/wchar.hpp>
#include <vidd/style.hpp>
#include <vidd/charsets.hpp>
#include <vidd/syntaxer.hpp>
#include <vidd/log.hpp>
#include <vidd/format.hpp>
#include <vidd/language.hpp>
#include <vidd/theme.hpp>
#include <vidd/vidd.hpp>
#include <vidd/charsets.hpp>

#include <map>
#include <memory>
#include <utility>


CodeView::CodeView(const TextEditor* editor)
: Component(Vec2::zero(), Vec2::ones()), mEditor(editor) {
}

void CodeView::enterJumpMode(const std::vector<std::pair<std::string, Vec2>>& words) {
	mJumpModeWords = words;
}

void CodeView::exitJumpMode(void) {
	mJumpModeWords.clear();
}

void CodeView::onPrerender(void) {
	requireRedraw();
}

void CodeView::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->text);
	
	drawFilledBox(Vec2(0, 0), mSize, ' ');

	const Cursor& cur = mEditor->getCursor();
	const ViewPort& view = mEditor->getViewPort();

	Line* first = cur.y->get(view.y);
	Line* last = first->skip(view.height - 1);

	const Language* lang = mEditor->getLanguage();
	if (lang != getLanguageByName("") && mJumpModeWords.size() == 0) {
		Syntaxer syntax(lang, theme);

		Line* ptr = first->first();
		while (ptr != first) {
			syntax.skimState(ptr->data);
			ptr = ptr->next();
		}
		int previousIndentLevel = 0;
		int y = 0;
		while (ptr != last->next()) {
			std::vector<Word> words = syntax.highlight(ptr->data);
			int x = -view.x;
			if (words.size() == 0 && previousIndentLevel > 1) {
				Style s = theme->text;
				s.fg = s.bg.augment(0.2);
				Draw::style(s);
				for (int i = 1; i < previousIndentLevel; i++) {
					drawText(Vec2(x + i * 4, y), "│"_ws);
				}
			} else {
				previousIndentLevel = 0;
			}
			for (Word& w : words) {
				Draw::style(w.style);
				drawText(Vec2(x, y), w.word);
				WStringView sv = w.word;
				int tx = x;
				while (x == -view.x && sv.length() >= 4 && sv.startsWith("    "_ws)) {
					Style s = theme->text;
					s.fg = s.bg.augment(0.2);
					Draw::style(s);
					if (tx > x) {
						drawText(Vec2(tx, y), "│"_ws);
					}
					sv = sv.subString(4, WStringView::npos);
					previousIndentLevel++;
					tx += 4;
				}
				x += w.word.length();
			}
			ptr = ptr->next();
			y++;
		}
	} else {
		Line* ptr = first;
		Style style = theme->text;
		if (mJumpModeWords.size() > 0) {
			style.fg = style.fg.darken(0.2);
		}
		Draw::style(style);
		int x = -view.x;
		int y = 0;
		while (ptr != last->next()) {
			drawText(Vec2(x, y), ptr->data);
			ptr = ptr->next();
			y++;
		}
	}

	{
		Style style(Color::none(), theme->bg.bg.augment(0.02));
		paintFormat(Vec2(0, cur.y->number - view.y), Vec2(view.width, 1), style);
	}

	if (mEditor->isSelecting()) {
		Selection sel = mEditor->getSelection();
		sel.curEnd = mEditor->getCursor();
		sel = sel.ordered();
		for (auto range : sel) {
			if (sel.type == SelectionType::Block && range.range.x == range.range.y) continue;
			int y = range.line->number - view.y;
			int x = std::max(0, range.range.x - view.x);
			int xDiff = x - (range.range.x - view.x);
			int minWidth = 0;
			if (range.range.y == (int)range.line->data.length()) minWidth = 1;
			Vec2 size(
				std::max(0, std::min(view.width, std::max(minWidth, range.range.y - range.range.x) - xDiff)),
				1
			);
			paintFormat(Vec2(x, y), size, theme->highlight);
		}
	}

	if (view.width >= 80 - view.x && Vidd::getShow80Line() && lang != getLanguageByName("")) {
		Style style(Color::none(), theme->bg.bg.augment(0.02));
		paintFormat(Vec2(80 - view.x, 0), Vec2(1, view.height), style);
	}

	if (cur.x < cur.y->data.length() && CharSets::brackets.contains(cur.y->data[cur.x])) {
		char bracket = cur.y->data[cur.x];
		char opposite = CharSets::oppositeBracket(bracket);
		if (CharSets::openBrackets.contains(cur.y->data[cur.x])) {
			int layer = 1;
			Line* line = cur.y;
			int x = cur.x + 1;
			while (line && line->number < view.y + view.height) {
				for (; x < line->data.length(); x++) {
					if (line->data[x] == opposite) layer -= 1;
					else if (line->data[x] == bracket) layer += 1;
					if (layer == 0) break;
				}
				if (layer == 0) break;
				x = 0;
				line = line->next();
			}
			if (line && line->number < view.y + view.height) {
				paintFormat(Vec2(cur.x - view.x, cur.y->number - view.y), Vec2(1, 1), theme->highlight);
				paintFormat(Vec2(x - view.x, line->number - view.y), Vec2(1, 1), Style(Style::reverse));
			}
		} else {
			int layer = 1;
			Line* line = cur.y;
			int x = cur.x - 1;
			while (line && line->number >= view.y) {
				for (; x >= 0; x--) {
					if (line->data[x] == opposite) layer -= 1;
					else if (line->data[x] == bracket) layer += 1;
					if (layer == 0) break;
				}
				if (layer == 0) break;
				line = line->prev();
				if (line) x = line->data.length() - 1;
			}
			if (line && line->number >= view.y) {
				paintFormat(Vec2(cur.x - view.x, cur.y->number - view.y), Vec2(1, 1), theme->highlight);
				paintFormat(Vec2(x - view.x, line->number - view.y), Vec2(1, 1), Style(Style::reverse));
			}
		}
	}

	if (mJumpModeWords.size() > 0) {
		Vec2 vpos = Vec2(view.x, view.y);
		Draw::style(theme->text + Style(theme->important, Color()));
		for (auto [key, pos] : mJumpModeWords) {
			drawText(pos - vpos, WString(key));
		}
	}

}
