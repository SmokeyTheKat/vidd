#include <vidd/codeview.hpp>

#include <vidd/texteditor.hpp>
#include <vidd/log.hpp>
#include <vidd/cursor.hpp>
#include <vidd/wchar.hpp>
#include <vidd/style.hpp>
#include <vidd/charsets.hpp>
#include <vidd/syntaxer.hpp>
#include <vidd/log.hpp>
#include <vidd/language.hpp>
#include <vidd/theme.hpp>
#include <vidd/vidd.hpp>

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

void CodeView::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->text);
	drawFilledBox(Vec2(0, 0), mSize, ' ');

	const Cursor& cur = mEditor->getCursor();
	const ViewPort& view = mEditor->getViewPort();

	Line* first = cur.y->get(view.y);
	Line* last = first->skip(view.height - 1);

	const Language* lang = getLanguageFromFileName(mEditor->getFileName());
	if (lang != getLanguageByName("") && mJumpModeWords.size() == 0) {
		Syntaxer syntax(lang, theme);
	
		Line* ptr = first->first();
		while (ptr != first) {
			syntax.skimState(ptr->data);
			ptr = ptr->next;
		}
	
		int y = 0;
		while (ptr != last->next) {
			std::vector<Word> words = syntax.highlight(ptr->data);
			int x = -view.x;
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
						drawText(Vec2(tx, y), "┆"_ws);
					}
					sv = sv.subString(4, WStringView::npos);
					tx += 4;
				}
				x += w.word.length();
			}
			ptr = ptr->next;
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
		while (ptr != last->next) {
			drawText(Vec2(x, y), ptr->data);
			ptr = ptr->next;
			y++;
		}
	}

	if (mEditor->isSelecting()) {
		Selection sel = mEditor->getSelection();
		sel.curEnd = mEditor->getCursor();
		sel = sel.ordered();
		int y = sel.curStart.y->number - view.y;
		for (auto range : sel) {
			int x = std::max(0, range.range.x - view.x);
			int xDiff = x - (range.range.x - view.x);
			int minWidth = 0;
			if (range.range.y == (int)range.line->data.length()) minWidth = 1;
			Vec2 size(
				std::max(0, std::min(view.width, std::max(minWidth, range.range.y - range.range.x) - xDiff)),
				1
			);
			paintFormat(Vec2(x, y++), size, theme->highlight);
		}
	}

	if (mJumpModeWords.size() > 0) {
		Vec2 view = Vec2(mEditor->getViewPort().x, mEditor->getViewPort().y);
		Draw::style(theme->text + Style(theme->important, Color()));
		for (auto [key, pos] : mJumpModeWords) {
			drawText(pos - view, WString(key));
		}
	}

}
