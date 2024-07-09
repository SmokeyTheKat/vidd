#include <vidd/texteditor.hpp>

#include <vidd/charsets.hpp>
#include <vidd/selection.hpp>
#include <vidd/language.hpp>
#include <vidd/log.hpp>
#include <vidd/utils.hpp>
#include <vidd/filesystem.hpp>

#include <iostream>
#include <fstream>

CopyBuffer TextEditor::mCopyBuffer;

namespace { //HELPPPPPPPPPPPPP

int getWordStartAtPos(WStringView data, int pos) {
	if (data.length() == 0) return 0;
	if (pos >= (int)data.length()) pos = data.length() - 1;
	if (CharSets::characters.contains(data[pos])) {
		while (pos > 0 && CharSets::characters.contains(data[pos])) pos--;
		if (pos == 0 && CharSets::characters.contains(data[pos])) return pos;
		return pos + 1;
	} else {
		return pos;
	}
}

}; // namespace

TextEditor::TextEditor(Input input)
: mName(input.getName()), mBuffer(std::make_unique<Buffer>(std::move(input))),
  mCursor(0, mBuffer->head), mView(0, 0), mUndoBuffer(this) {
	loadFileData();
}

TextEditor::~TextEditor(void) {
	mBuffer->head = mCursor.y;
	saveFileData();
}

std::string TextEditor::getFileDataPath(void) {
	std::string pathId = FileSystem::realPath(mName);
	Utils::stringReplace(pathId, "/", "+");
	return std::string(PREFIX) + "/share/vidd/filedata/" + pathId;
}

void TextEditor::saveFileData(void) {
	std::string path = getFileDataPath();
	Log::log(path);
	std::FILE* fp = std::fopen(path.c_str(), "w");
	if (fp == nullptr) return;
	intmax_t x = mCursor.x;
	intmax_t y = mCursor.y->number + 1;
	intmax_t vx = mView.x;
	intmax_t vy = mView.y;
	std::fwrite(&x, sizeof(x), 1, fp);
	std::fwrite(&y, sizeof(y), 1, fp);
	std::fwrite(&vx, sizeof(x), 1, fp);
	std::fwrite(&vy, sizeof(y), 1, fp);
	std::fclose(fp);
}

void TextEditor::loadFileData(void) {
	std::string path = getFileDataPath();
	std::FILE* fp = std::fopen(path.c_str(), "r");
	if (fp == nullptr) {
		return;
	}
	intmax_t x, y;
	intmax_t vx, vy;
	std::fread(&x, sizeof(x), 1, fp);
	std::fread(&y, sizeof(y), 1, fp);
	std::fread(&vx, sizeof(x), 1, fp);
	std::fread(&vy, sizeof(y), 1, fp);
	std::fclose(fp);
	mView.x = vx;
	mView.y = vy;
	cursorQuietMoveTo(x, y - 1);
}

void TextEditor::resize(Vec2 size) {
	mView.width = size.x;
	mView.height = size.y;
	restrictCursor();
	adjustView();
}

void TextEditor::editFile(Input input) {
	saveFileData();
	mName = input.getName();
	mBuffer = std::make_unique<Buffer>(std::move(input));
	mView = ViewPort(0, 0, mView.width, mView.height);
	mCursor = Cursor(0, mBuffer->head);
	mViewChanged = true;
	mUnsavedChanges = false;
	mUndoBuffer.clear();
	loadFileData();
}

void TextEditor::saveFile(std::string name) {
	if (name.length() == 0) name = mName;
	std::ofstream ofs(name);
	Line* ptr = mCursor.y->first();
	WString tabstr(std::string(4, ' '));
	while (ptr) {
		WStringView str = ptr->data;
		bool pastIndent = false;
		for (std::size_t i = 0; i < str.length(); i++) {
			if (str[i] != ' ') {
				pastIndent = true;
			}
			if (
				mOutputTabs &&
				pastIndent == false &&
				i + 4 <= str.length() &&
				str.subString(i, i + 4) == tabstr
			) {
				ofs << '\t';
				i += 3;
			} else {
				ofs << str[i].view();
			}
		}
		if (ptr->next()) ofs << '\n';
		ptr = ptr->next();
	}
	ofs.close();
	mUnsavedChanges = false;
}

void TextEditor::viewChangedIfSelecting(void) {
	if (isSelecting()) {
		mViewChanged = true;
	}
}

int TextEditor::getLineCount(void) {
	return mCursor.y->last()->number + 1;
}

void TextEditor::startSelection(void) {
	mSelecting = true;
	mSelection.curStart = mCursor;
	mSelection.type = SelectionType::Normal;
}

void TextEditor::startLineSelection(void) {
	mSelecting = true;
	mSelection.curStart = mCursor;
	mSelection.type = SelectionType::Line;
}

void TextEditor::startWordSelection(void) {
	mSelecting = true;
	mSelection.curStart = mCursor;
	mSelection.type = SelectionType::Word;
}

void TextEditor::startBlockSelection(void) {
	mSelecting = true;
	mSelection.curStart = mCursor;
	mSelection.type = SelectionType::Block;
}

void TextEditor::endSelection(void) {
	mSelection.curEnd = mCursor;
}

void TextEditor::stopSelection(void) {
	mSelecting = false;
}

void TextEditor::editDeleteSelection(void) {
	endSelection();
	Selection selection = mSelection.ordered();

	Line* firstLine = selection.curStart.y;
	Line* lineBefore = firstLine->prev();

	cursorMoveTo(selection.curEnd);

	Line* lineAfter = lineBefore != nullptr ? lineBefore : selection.curEnd.y->next();
	for (auto range : selection) {
		range.line->data.remove(range.range.x, range.range.y);
		switch (range.type) {
		case SelectionRangeType::MiddleLine: {
			if (range.line->isIsolated() == false) {
				range.line->remove();
			} else {
				range.line->number = 0;
				range.line->data.clear();
				lineAfter = range.line;
			}
		} break;
		case SelectionRangeType::LastLine: {
			Line* last = mergeLineUpAtCursor();
			if (lineBefore == nullptr) lineAfter = last;
		} break;
		case SelectionRangeType::OnlyLine: {
			lineBefore = firstLine;
			lineAfter = firstLine;
		} break;
		default: break;
		}
	}

	if (lineBefore != nullptr && lineAfter->next() && lineAfter != firstLine) {
		lineAfter = lineAfter->next();
	}
	cursorMoveTo(Cursor(selection.curStart.x, lineAfter, selection.curStart.lx));

	mUnsavedChanges = true;
	mViewChanged = true;
}

void TextEditor::deleteSelection(void) {
	endSelection();
	Selection selection = mSelection.ordered();

	if (selection.type == SelectionType::Line) {
		Vec2 start = selection.curStart.toVec2();
		Vec2 end = selection.curEnd.toVec2();
		registerUndoAction<UndoDeleteAction>(
			Vec2(0, start.y),
			Vec2(0, end.y + 1),
			readNormalSelectionToString()
		);
	} else if (selection.type == SelectionType::Block) {
		registerUndoAction<UndoBlockDeleteAction>(
			selection.curStart.toVec2(),
			selection.curEnd.toVec2(),
			readLineSelectionToString()
		);
	} else {
		registerUndoAction<UndoDeleteAction>(
			selection.curStart.toVec2(),
			selection.curEnd.toVec2(),
			readNormalSelectionToString()
		);
	}
	mTrackUndos = false;
	editDeleteSelection();
	mTrackUndos = true;
	mUnsavedChanges = true;
	mViewChanged = true;
}

WString TextEditor::readNormalSelectionToString(void) {
	endSelection();
	Selection selection = mSelection.ordered();

	WString data;
	for (auto range : selection) {
		WStringView text = range.getText();
		data.insert(data.end(), text.begin(), text.end());
		switch (range.type) {
		case SelectionRangeType::FirstLine:
		case SelectionRangeType::MiddleLine: {
			data.pushBack('\n');
		} break;
		default: break;
		}
	}

	return data;
}

void TextEditor::copyNormalSelection(void) {
	mCopyBuffer.data = readNormalSelectionToString();
	mCopyBuffer.type = CopyType::Normal;
}

WString TextEditor::readLineSelectionToString(void) {
	endSelection();
	Selection selection = mSelection.ordered();

	WString data;
	for (auto range : selection) {
		WStringView text = range.getText();
		data.insert(data.end(), text.begin(), text.end());
		data.pushBack('\n');
	}
	data.popBack();
	return data;
}
	
void TextEditor::copyLineSelection(void) {
	mCopyBuffer.data = readLineSelectionToString();
	mCopyBuffer.type = CopyType::Line;
}
	
void TextEditor::copySelection(void) {
	switch (mSelection.type) {
	case SelectionType::Line:
	case SelectionType::Block: {
		copyLineSelection();
	} break;
	default: {
		copyNormalSelection();
	} break;
	}
}

void TextEditor::indentSelection(void) {
	endSelection();
	for (auto range : mSelection.ordered()) {
		range.line->data.insert(0, ' ', gTabWidth);
	}
	registerUndoAction<UndoIndentAction>(
		mSelection.curStart.y->number,
		mSelection.curEnd.y->number
	);
	mViewChanged = true;
}

void TextEditor::deindentSelection(void) {
	endSelection();
	WString tab(gTabWidth, ' ');
	for (auto range : mSelection.ordered()) {
		if (range.line->data.startsWith(tab)) {
			range.line->data.removeNAt(0, gTabWidth);
		}
	}
	registerUndoAction<UndoDeindentAction>(
		mSelection.curStart.y->number,
		mSelection.curEnd.y->number
	);
	mViewChanged = true;
}

void TextEditor::indentAtCursor(void) {
	mCursor.y->data.insert(0, ' ', gTabWidth);
	cursorMoveX(gTabWidth);
	mViewChanged = true;
	registerUndoAction<UndoInsertAction>(
		Vec2(0, mCursor.y->number),
		Vec2(gTabWidth, mCursor.y->number),
		WString(gTabWidth, ' ')
	);
}

void TextEditor::deindentAtCursor(void) {
	WString tab(gTabWidth, ' ');
	if (mCursor.y->data.startsWith(tab)) {
		mCursor.y->data.removeNAt(0, gTabWidth);
		registerUndoAction<UndoDeleteAction>(
			Vec2(0, mCursor.y->number),
			Vec2(gTabWidth, mCursor.y->number),
			WString(gTabWidth, ' ')
		);
		cursorMoveX(-gTabWidth);
		mViewChanged = true;
	}
}

void TextEditor::toggleCaptailizationAtCursor(void) {
	WChar& chr = mCursor.y->data[mCursor.x];
	WChar beforeChr = chr;
	if (CharSets::letters.contains(chr)) {
		if (std::isupper(chr)) {
			chr = std::tolower(chr);
		} else {
			chr = std::toupper(chr);
		}
		registerUndoAction<UndoCharacterReplaceAction>(
			Vec2(mCursor.x, mCursor.y->number),
			beforeChr,
			chr
		);
		mViewChanged = true;
		mUnsavedChanges = true;
	}
}

void TextEditor::toggleCommentAtCursor(void) {
	const Language* lang = getLanguageFromFileName(getFileName());
	if (lang->syntax.singleLineCommentSymbols.size() == 0) {
		return;
	}

	std::string_view commentChar = lang->syntax.singleLineCommentSymbols[0];

	WStringView line = mCursor.y->data;

	if (
		line.length() >= commentChar.length() &&
		line.subString(0, commentChar.length()) == commentChar
	) {
		mCursor.y->data.removeNAt(0, commentChar.length());
	} else {
		mCursor.y->data.insert(mCursor.y->data.begin(), commentChar.begin(), commentChar.end());
	}

	mViewChanged = true;
}

void TextEditor::toggleCommentAtSelection(void) {
	Cursor saved = mCursor;

	endSelection();
	Selection selection = mSelection.ordered();

	for (auto range : selection) {
		mCursor.y = range.line;
		toggleCommentAtCursor();
	}

	mCursor = saved;

	mViewChanged = true;
}

void TextEditor::paste(void) {
	bool savedOverflow = mLineEndOverflow;
	mLineEndOverflow = true;

	if (mCopyBuffer.type == CopyType::Normal) {
		mTrackUndos = false;

#ifndef VIDD_DEFAULT_MODE_INSERT
		cursorMoveX(1);
#endif
		Vec2 regionStart = mCursor.toVec2();
		for (auto c : mCopyBuffer.data) {
			insertCharAtCursor(c);
		}

		mTrackUndos = true;
		registerUndoAction<UndoInsertAction>(
			regionStart,
			mCursor.toVec2(),
			mCopyBuffer.data
		);

#ifndef VIDD_DEFAULT_MODE_INSERT
		cursorMoveX(-1);
#endif
	} else if (mCopyBuffer.type == CopyType::Line) {
		mTrackUndos = false;

		Cursor start = mCursor;
		Vec2 regionStart = Vec2(start.y->data.length(), start.y->number - 0);
		insertLineDownFromCursor();
		cursorMoveY(1);
		for (auto c : mCopyBuffer.data) {
			insertCharAtCursor(c);
		}

		mTrackUndos = true;
		Vec2 regionEnd = Vec2(mCursor.y->data.length(), mCursor.y->number);
		registerUndoAction<UndoInsertAction>(
			regionStart,
			regionEnd,
			'\n' + mCopyBuffer.data
		);
		cursorMoveTo(0, start.y->number + 1);
	}

	mLineEndOverflow = savedOverflow;
	restrictCursor();
}

void TextEditor::pasteBack(void) {
	bool savedOverflow = mLineEndOverflow;
	mLineEndOverflow = true;

	if (mCopyBuffer.type == CopyType::Normal) {
		mTrackUndos = false;

		Vec2 regionStart = mCursor.toVec2();
		for (auto c : mCopyBuffer.data) {
			insertCharAtCursor(c);
		}

		mTrackUndos = true;
		registerUndoAction<UndoInsertAction>(
			regionStart,
			mCursor.toVec2(),
			mCopyBuffer.data
		);
	} else if (mCopyBuffer.type == CopyType::Line) {
		mTrackUndos = false;

		Cursor start = mCursor;

		Vec2 regionStart = Vec2(0, start.y->number);

		insertLineUpFromCursor();
		cursorMoveY(-1);

		for (auto c : mCopyBuffer.data) {
			insertCharAtCursor(c);
		}

		Vec2 regionEnd = Vec2(0, mCursor.y->number + 1);
		mTrackUndos = true;
		registerUndoAction<UndoInsertAction>(
			regionStart,
			regionEnd,
			mCopyBuffer.data + '\n'
		);
		cursorMoveTo(0, start.y->number);

	}

	mLineEndOverflow = savedOverflow;
	restrictCursor();
}

void TextEditor::swapCursorSelectionEnd(void) {
	std::swap(mCursor, mSelection.curStart);
	adjustView();
}

int TextEditor::getLineEnd(void) {
	if (mLineEndOverflow) {
		return mCursor.y->data.length();
	} else {
		int end = mCursor.y->data.length();
		return std::max(0, end - 1);
	}
}

void TextEditor::insertLineDownFromCursor(void) {
	registerUndoAction<UndoInsertLineDownAction>(
		mCursor.y->number
	);
	mCursor.y->insertLine();
	mViewChanged = true;
	mUnsavedChanges = true;
}

void TextEditor::insertLineUpFromCursor(void) {
	registerUndoAction<UndoInsertLineUpAction>(
		mCursor.y->number
	);
	mCursor.y->insertLineUp();
	mViewChanged = true;
	mUnsavedChanges = true;
}

void TextEditor::splitLineAtCursor(void) {
	Line* line = mCursor.y;
	line->splitAt(mCursor.x);

	registerUndoAction<UndoLineSplitAction>(
		line->number + 1,
		line->data.length()
	);

	mViewChanged = true;
	mUnsavedChanges = true;

	cursorMoveY(1);
	cursorMoveToX(0);
}

Line* TextEditor::mergeLineUpAtCursor(void) {
	Line* line = mCursor.y;
	Line* pline = line->prev();
	if (!pline) return line;
	int plineLength = pline->data.length();

	registerUndoAction<UndoLineMergeAction>(
		mCursor.y->number,
		pline->data.length()
	);

	cursorMoveY(-1);
	cursorMoveToX(plineLength - 1);
	pline->data.insert(pline->data.end(), line->data.begin(), line->data.end());
	line->remove();
	if (plineLength > 0) {
		cursorMoveX(1);
	}
	mViewChanged = true;
	mUnsavedChanges = true;
	return pline;
}

void TextEditor::insertAtCursor(WStringView data) {
	bool savedOverflow = mLineEndOverflow;
	mLineEndOverflow = true;

	mTrackUndos = false;

	Vec2 regionStart = mCursor.toVec2();
	for (auto c : data) {
		insertCharAtCursor(c);
	}

	mTrackUndos = true;
	registerUndoAction<UndoInsertAction>(
		regionStart,
		mCursor.toVec2(),
		mCopyBuffer.data
	);

	mLineEndOverflow = savedOverflow;
}

bool TextEditor::insertCharAtCursor(WChar chr) {
	bool res = false;
	switch (chr) {
	case '\n': {
		splitLineAtCursor();
		res = true;
	} break;
	case '\t': {
		registerUndoAction<UndoInsertAction>(
			mCursor.toVec2(),
			mCursor.toVec2() + Vec2(4, 0),
			WString(4, ' ')
		);
		mCursor.y->data.insert(mCursor.x, ' ', gTabWidth);
		cursorMoveX(4);
		res = true;
	} break;
	default: {
		if (isVisibleWChar(chr)) {
			registerUndoAction<UndoInsertAction>(
				mCursor.toVec2(),
				mCursor.toVec2() + Vec2(1, 0),
				WString(1, chr)
			);
			mCursor.y->data.insert(mCursor.x, chr);
			cursorMoveX(1);
			res = true;
		}
	} break;
	}
	mUnsavedChanges = true;
	mViewChanged = true;
	return res;
}

void TextEditor::replaceCharAtCursor(WChar chr) {
	if (isVisibleWChar(chr) == false) return;

	WString& line = mCursor.y->data;
	if (mCursor.x == (int)line.length()) {
		line.pushBack(' ');
	}

	registerUndoAction<UndoCharacterReplaceAction>(
		Vec2(mCursor.x, mCursor.y->number),
		line[mCursor.x],
		chr
	);

	line[mCursor.x] = chr;

	mUnsavedChanges = true;
	mViewChanged = true;
}

void TextEditor::deleteLineAtCursor(void) {
	mViewChanged = true;
	mUnsavedChanges = true;

	Line* line = mCursor.y;

	registerUndoAction<UndoDeleteAction>(
		Vec2(0, mCursor.y->number),
		Vec2(0, mCursor.y->number + 1),
		line->data + "\n"
	);

	mCursor.y = line->remove();
}

void TextEditor::deleteAtCursor(void) {
	WString& line = mCursor.y->data;
	if (line.length() == 0) {
		deleteLineAtCursor();
	} else {
		Vec2 deleteBegin = mCursor.toVec2();
		Vec2 deleteEnd = deleteBegin;
		deleteEnd.x += 1;
		WString data(1, line[mCursor.x]);

		if (cursorIsAtTab()) {
			deleteEnd.x += 0;
			deleteBegin.x -= 3;
			cursorMoveX(-3);
			data = WString(4, ' ');
			line.removeAt(mCursor.x);
			line.removeAt(mCursor.x);
			line.removeAt(mCursor.x);
			line.removeAt(mCursor.x);
		} else {
			line.removeAt(mCursor.x);
		}
		registerUndoAction<UndoDeleteAction>(
			deleteBegin,
			deleteEnd,
			data
		);
		fixCursor();
	}
	mUnsavedChanges = true;
	mViewChanged = true;
}

void TextEditor::backspaceAtCursor(void) {
	if (mCursor.x == 0) {
		mergeLineUpAtCursor();
	} else {
		cursorMoveX(-1);
		deleteAtCursor();
	}
	mViewChanged = true;
}

void TextEditor::replaceNextFromCursor(WStringView find, WStringView replace) {
	Cursor findPos = findNextFromCursor(find);
	WString& line = findPos.y->data;
	line.removeNAt(findPos.x, find.length());
	line.insert(line.begin() + findPos.x, replace.begin(), replace.end());
	mUnsavedChanges = true;
	mViewChanged = true;
}

void TextEditor::adjustView(void) {
	if (mView.x < 0) {
		mView.x = 0;
		mViewChanged = true;
	}
	if (mCursor.x < mView.x) {
		mView.x = mCursor.x;
		mViewChanged = true;
	}
	if (mCursor.x >= mView.x + mView.width) {
		if (mCursor.x == mView.x + mView.width) mView.x++;
		else mView.x = mCursor.x - mView.width / 2;
		mViewChanged = true;
	}
	if (mCursor.y->number < mView.y) {
		if (mCursor.y->number == mView.y - 1) mView.y--;
		else mView.y = std::max(0, mCursor.y->number - mView.height / 2);
		mViewChanged = true;
	}
	if (mCursor.y->number >= mView.y + mView.height) {
		if (mCursor.y->number == mView.y + mView.height) mView.y++;
		else mView.y = mCursor.y->number - mView.height / 4;
		mViewChanged = true;
	}
}

void TextEditor::fixCursor(void) {
	if (mCursor.x == mView.x - 1) {
		mCursor.x = mView.x;
		mCursor.lx = mCursor.x;
	} else if (mCursor.x < mView.x) {
		mCursor.x = mView.x;
		mCursor.lx = mCursor.x;
	}

	if (mCursor.x == mView.x + mView.width) {
		mCursor.x--;
		mCursor.lx = mCursor.x;
	} else if (mCursor.x > mView.x + mView.width) {
		mCursor.x = mView.x + mView.width;
		mCursor.lx = mCursor.x;
	}

	if (mCursor.y->number == mView.y - 1) {
		mCursor.y = mCursor.y->get(mView.y);
	} else if (mCursor.y->number < mView.y) {
		mCursor.y = mCursor.y->get(mView.y);
	}

	if (mCursor.y->number == mView.y + mView.height) {
		mCursor.y = mCursor.y->prev();
	} else if (mCursor.y->number > mView.y + mView.height) {
		mCursor.y = mCursor.y->get(mView.y + mView.height);
	}

	if (mCursor.x > getLineEnd()) {
		mCursor.x = getLineEnd();
	}
	if (mCursor.x < 0) {
		mCursor.x = 0;
	}

	adjustView();
}

void TextEditor::restrictCursor(void) {
	if (mCursor.x > getLineEnd()) {
		mCursor.x = getLineEnd();
	}
	if (mCursor.x < 0) {
		mCursor.x = 0;
	}
}

void TextEditor::viewCenterAroundCursor(void) {
	mView.y = std::max(0, mCursor.y->number - mView.height / 2);
	if (mCursor.x > mView.width * 0.90) {
		mView.x = mCursor.x - mView.width / 2;
	}
	mViewChanged = true;
}

void TextEditor::viewScrollY(int dy) {
	mView.y = std::max(mView.y + dy, 0);
	fixCursor();
	mViewChanged = true;
}

void TextEditor::viewScrollX(int dx) {
	mView.x = std::max(mView.x + dx, 0);
	if (mView.x >= getLineEnd()) {
		cursorMoveToInView();
	}
	fixCursor();
	mViewChanged = true;
}

void TextEditor::viewScrollToY(int y) {
	mView.y = y;
	fixCursor();
	mViewChanged = true;
}

char TextEditor::getCharAtCursor(void) {
	return mCursor.y->data.length() == 0 ? ' ' : mCursor.y->data[mCursor.x].bytes[0];
}

WStringView TextEditor::getWordUnderCursor(void) {
	Line* line = mCursor.y;

	if (line->data.length() == 0) {
		return {};
	}
	if (CharSets::characters.contains(getCharAtCursor()) == false) {
		return {};
	}

	std::size_t start;
	std::size_t i = mCursor.x;
	if (i > 0) {
		while (i > 0 && CharSets::characters.contains(line->data[i])) i--;
		start = i + 1;
	} else {
		start = i;
	}

	i = mCursor.x;
	while (i < line->data.length() && CharSets::characters.contains(line->data[i])) i++;
//    if (i < mCursor.y->data.length() && mCursor.y->data[i] == '(') i++;
	std::size_t end = i;

	return WStringView(line->data.begin() + start, line->data.begin() + end);
}

bool TextEditor::cursorIsAtStartOfWord(void) {
	int x = mCursor.x;
	const WString& data = mCursor.y->data;

	if (x <= 0) return true;

	auto cat1 = CharSets::getCharSetOf(data[x]);
	auto cat2 = CharSets::getCharSetOf(data[x-1]);

	return cat1 != cat2;
}

bool TextEditor::cursorIsAtEndOfWord(void) {
	int x = mCursor.x;
	WStringView data(mCursor.y->data);

	if (x == getLineEnd()) return true;

	auto cat1 = CharSets::getCharSetOf(data[x]);
	auto cat2 = CharSets::getCharSetOf(data[x+1]);

	return cat1 != cat2;
}

bool TextEditor::cursorIsAtTab(void) {
	if ((mCursor.x + 1) % 4 != 0) return false;

	const WString& line = mCursor.y->data;

	int i = 0;
	while (i < mCursor.x && line[i] == ' ') i++;
	return i == mCursor.x && line[i] == ' ';
}

void TextEditor::cursorMoveNextWord(void) {
	if (mCursor.x == getLineEnd()) {
		cursorMoveY(1);
		cursorMoveToX(0);
		while (mCursor.x != getLineEnd() && CharSets::whitespace.contains(getCharAtCursor())) {
			cursorMoveX(1);
		}
		return;
	}

	auto cat = CharSets::getCharSetOf(getCharAtCursor());

	while (cat.contains(getCharAtCursor()) && mCursor.x != getLineEnd()) {
		cursorMoveX(1);
	}

	if (mCursor.x == getLineEnd() && cat.contains(getCharAtCursor())) {
		cursorMoveY(1);
		cursorMoveToX(0);
		while (mCursor.x != getLineEnd() && CharSets::whitespace.contains(getCharAtCursor())) {
			cursorMoveX(1);
		}
	} else if (CharSets::whitespace.contains(getCharAtCursor())) {
		cursorMoveNextWord();
	}

	viewChangedIfSelecting();
}

void TextEditor::cursorMovePrevWord(void) {
	bool initialWhiteSpace = false;
	while (mCursor.x > 0 && CharSets::whitespace.contains(getCharAtCursor())) {
		cursorMoveX(-1);
		initialWhiteSpace = true;
	}

	if (mCursor.x == 0) {
		cursorMoveY(-1);
		cursorMoveToX(getLineEnd());
		if (mCursor.x > 0 && !cursorIsAtStartOfWord()) cursorMovePrevWord();
		return;
	}

	if (cursorIsAtStartOfWord()) {
		if (initialWhiteSpace) return;

		cursorMoveX(-1);

		while (mCursor.x > 0 && CharSets::whitespace.contains(getCharAtCursor())) {
			cursorMoveX(-1);
		}

		if (CharSets::whitespace.contains(getCharAtCursor())) {
			cursorMoveY(-1);
			cursorMoveToX(getLineEnd());
			if (!cursorIsAtStartOfWord()) cursorMovePrevWord();
		} else if (cursorIsAtStartOfWord()) {
			return;
		}
	}

	if (mCursor.y->data.length() == 0) return;

	auto cat = CharSets::getCharSetOf(getCharAtCursor());

	while (mCursor.x > 0 && cat.contains(getCharAtCursor())) {
		cursorMoveX(-1);
	}

	cursorMoveX(1);
	if (cursorIsAtStartOfWord() == false) {
		cursorMoveX(-1);
	}

	if (CharSets::whitespace.contains(getCharAtCursor())) {
		cursorMoveY(-1);
		cursorMoveToX(getLineEnd());
		if (!cursorIsAtStartOfWord()) cursorMovePrevWord();
	}

	viewChangedIfSelecting();
}

void TextEditor::cursorMoveNextWordEnd(void) {
	if (mCursor.x == getLineEnd()) {
		cursorMoveY(1);
		cursorMoveToX(0);
		if (!cursorIsAtEndOfWord()) {
			cursorMoveNextWordEnd();
		}
		return;
	}

	if (cursorIsAtEndOfWord()) {
		cursorMoveX(1);
	}

	while (CharSets::whitespace.contains(getCharAtCursor()) && mCursor.x != getLineEnd()) {
		cursorMoveX(1);
	}
		
	if (mCursor.x == getLineEnd()) {
		cursorMoveY(1);
		cursorMoveToX(0);
		if (!cursorIsAtEndOfWord()) {
			cursorMoveNextWordEnd();
		}
		return;
	}

	while (!cursorIsAtEndOfWord()) {
		cursorMoveX(1);
	}

	viewChangedIfSelecting();
}

void TextEditor::cursorMoveNextParagraph(void) {
	while (getLineEnd() <= 0 && mCursor.y->next())
		cursorMoveY(1);

	while (getLineEnd() > 0 && mCursor.y->next())
		cursorMoveY(1);

	while (getLineEnd() <= 0 && mCursor.y->next())
		cursorMoveY(1);

	cursorMoveY(-1);
	viewChangedIfSelecting();
}

void TextEditor::cursorMovePrevParagraph(void) {
	while (getLineEnd() <= 0 && mCursor.y->prev())
		cursorMoveY(-1);

	while (getLineEnd() > 0 && mCursor.y->prev())
		cursorMoveY(-1);

	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToViewCenter(void) {
	mCursor.y = mCursor.y->get(mView.y + mView.height / 2);
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToInView(void) {
	Line* line = mCursor.y->get(mView.y);
	while (line && line->number < mView.y + mView.height) {
		if ((int)line->data.length() > mView.x) {
			mCursor.y = line;
			mCursor.x = mView.x;
			viewChangedIfSelecting();
			break;
		}
		line = line->next();
	}
}

void TextEditor::cursorMoveY(int dy) {
	Line* line = mCursor.y->skip(dy);
	if (line == nullptr) return;
	mCursor.y = line;
	mCursor.x = mCursor.lx;
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveX(int dx) {
	if (mCursor.x + dx < 0 || mCursor.x + dx > getLineEnd()) return;

	mCursor.x += dx;
	mCursor.lx = mCursor.x;
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToY(int y) {
	mCursor.y = mCursor.y->get(y);
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToX(int x) {
	mCursor.x = x;
	mCursor.lx = x;
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveTo(int x, int y) {
	mCursor.y = mCursor.y->get(y);
	mCursor.x = x;
	mCursor.lx = x;
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorQuietMoveTo(int x, int y) {
	mCursor.y = mCursor.y->get(y);
	mCursor.x = x;
	mCursor.lx = x;
}

void TextEditor::cursorMoveTo(Vec2 pos) {
	cursorMoveTo(pos.x, pos.y);
}

void TextEditor::cursorMoveTo(Cursor cur) {
	mCursor = cur;
	mCursor.lx = mCursor.x;
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToWordStart(void) {
	cursorMoveToX(getWordStartAtPos(mCursor.y->data, mCursor.x));
}

void TextEditor::cursorMoveToNextIndentationLevel(void) {
	int level = mCursor.x;
	if (mCursor.y->getFirstChar() < level) {
		level = mCursor.y->getFirstChar();
		cursorMoveToX(level);
	}
	mCursor.lx = mCursor.x;

	if (getCharAtCursor() != ' ') {
		while (mCursor.y->isEmpty() || mCursor.y->getFirstChar() == level) {
			cursorMoveY(1);
			if (!mCursor.y->next()) break;
		}

		if (getCharAtCursor() != ' ') {
			cursorMoveY(1);
		}

		if (mViewChanged) {
			viewCenterAroundCursor();
		}

		viewChangedIfSelecting();
	}

	if (mCursor.x == level && getCharAtCursor() == ' ') {
		while (mCursor.y->isEmpty() || mCursor.y->getFirstChar() > level) {
			cursorMoveY(1);
			if (!mCursor.y->next()) break;
		}

		if (mViewChanged) {
			viewCenterAroundCursor();
		}

		viewChangedIfSelecting();
	}
}

void TextEditor::cursorMoveToPrevionsIndentationLevel(void) {
	int level = mCursor.x;
	if (mCursor.y->getFirstChar() < level) {
		level = mCursor.y->getFirstChar();
		cursorMoveToX(level);
	}
	mCursor.lx = mCursor.x;

	if (getCharAtCursor() != ' ') {
		while (mCursor.y->isEmpty() || mCursor.y->getFirstChar() == level) {
			cursorMoveY(-1);
			if (!mCursor.y->prev()) break;
		}

		if (getCharAtCursor() != ' ') {
			cursorMoveY(-1);
		}

		if (mViewChanged) {
			viewCenterAroundCursor();
		}

		viewChangedIfSelecting();
	}

	if (mCursor.x == level && getCharAtCursor() == ' ') {
		while (mCursor.y->isEmpty() || mCursor.y->getFirstChar() > level) {
			cursorMoveY(-1);
			if (!mCursor.y->prev()) break;
		}

		if (mViewChanged) {
			viewCenterAroundCursor();
		}

		viewChangedIfSelecting();
	}
}

void TextEditor::cursorMoveToFirstLine(void) {
	mCursor.y = mCursor.y->first();
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToLastLine(void) {
	mCursor.y = mCursor.y->last();
	restrictCursor();
	adjustView();
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToLineStart(void) {
	cursorMoveToX(0);
	viewChangedIfSelecting();
}

void TextEditor::cursorMoveToLineEnd(void) {
	cursorMoveToX(INT_MAX);
	viewChangedIfSelecting();
}

Cursor TextEditor::findNextFromCursor(WStringView find) const {
	Line* ptr = mCursor.y;
	std::size_t x = mCursor.x + 1;
	while (ptr) {
		WStringView line(ptr->data);
		std::size_t pos = line.findSubString(find, x);
		if (pos == WStringView::npos) {
			x = 0;
			ptr = ptr->next();
		} else {
			return Cursor(pos, ptr);
		}
	}
	return mCursor;
}

Cursor TextEditor::findPrevFromCursor(WStringView find) const {
	Line* ptr = mCursor.y;
	std::size_t x = mCursor.x;
	while (ptr) {
		WStringView line(ptr->data);
		std::size_t pos = line.findSubStringRtl(find, x);
		if (pos == WStringView::npos) {
			x = WStringView::npos;
			ptr = ptr->prev();
		} else {
			return Cursor(pos, ptr);
		}
	}
	return mCursor;
}

std::vector<Vec2> TextEditor::indexWordsLocations(void) {
	auto isAtWord = [](const WString& data, std::size_t x) {
		if (
			CharSets::letters.contains(data[x]) &&
			data.length() > x + 1 &&
			CharSets::letters.contains(data[x + 1])
		) return true;
		return false;
	};

	auto skipWord = [](const WString& data, std::size_t& x) {
		if (CharSets::upperLetters.contains(data[x])) {
			while (CharSets::upperLetters.contains(data[x])) x++;
		} else if (CharSets::lowerLetters.contains(data[x])) {
			x++;
			if (!CharSets::lowerLetters.contains(data[x])) {
				while (CharSets::upperLetters.contains(data[x])) x++;
			}
		}

		while (CharSets::lowerLetters.contains(data[x])) x++;
	};

	std::vector<Vec2> words;

	Line* line = mCursor.y->get(mView.y);
	Line* end = line->skip(mView.height);
	for (; line != end; line = line->next()) {
		const WString& data = line->data;
		for (std::size_t x = 0; x < data.length();) {
			if (isAtWord(data, x)) {
				words.emplace_back(x, line->number);
				skipWord(data, x);
			} else {
				x += 1;
			}
		}
	}

	return words;
}
