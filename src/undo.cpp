#include <vidd/undo.hpp>

#include <vidd/texteditor.hpp>
#include <vidd/utils.hpp>

void UndoBuffer::registerAction(std::unique_ptr<UndoAction> action) {
	if (mLocked == true) return;
	if (mPos < mActions.size()) {
		mActions.erase(
			mActions.begin() + mPos,
			mActions.end()
		);
	}

	mActions.emplace_back(std::move(action));
	mPos += 1;
}

void UndoBuffer::clear(void) {
	mPos = 0;
	mActions.clear();
	mLocked = false;
}

bool UndoBuffer::undo(void) {
	if (mPos == 0) return false;
	mPos -= 1;
	mLocked = true;
	mActions[mPos]->undo(mEditor);
	mLocked = false;
	return true;
}

bool UndoBuffer::redo(void) {
	if (mPos == mActions.size()) return false;
	mLocked = true;
	mActions[mPos]->redo(mEditor);
	mLocked = false;
	mPos += 1;
	return true;
}

void UndoLineMergeAction::undo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveToY(line - 1);
	editor->cursorMoveToX(mergePoint);
	editor->splitLineAtCursor();

	editor->setLineOverflow(false);
}

void UndoLineMergeAction::redo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveTo(Vec2(0, line));
	editor->mergeLineUpAtCursor();

	editor->setLineOverflow(false);
}

void UndoLineSplitAction::undo(TextEditor* editor) const {
	oppisite.redo(editor);
}

void UndoLineSplitAction::redo(TextEditor* editor) const {
	oppisite.undo(editor);
}

void UndoInsertLineDownAction::undo(TextEditor* editor) const {
	editor->cursorMoveToY(line + 1);
	editor->deleteLineAtCursor();
	editor->cursorMoveToY(line);
}

void UndoInsertLineDownAction::redo(TextEditor* editor) const {
	editor->cursorMoveToY(line);
	editor->insertLineDownFromCursor();
	editor->cursorMoveY(1);
}

void UndoInsertLineUpAction::undo(TextEditor* editor) const {
	editor->cursorMoveToY(line);
	editor->deleteLineAtCursor();
	editor->cursorMoveToY(line);
}

void UndoInsertLineUpAction::redo(TextEditor* editor) const {
	editor->cursorMoveToY(line);
	editor->insertLineUpFromCursor();
	editor->cursorMoveY(-1);
}

void UndoInsertAction::undo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveTo(start);
	editor->startSelection();
	editor->cursorMoveTo(end);
	editor->deleteSelection();
	editor->cursorMoveTo(start);
	editor->stopSelection();

	editor->setLineOverflow(false);
}

void UndoInsertAction::redo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveTo(start);
	for (WChar chr : text) {
		editor->insertCharAtCursor(chr);
	}
	editor->cursorMoveTo(start);

	editor->setLineOverflow(false);
}

void UndoDeleteAction::undo(TextEditor* editor) const {
	oppisite.redo(editor);
}

void UndoDeleteAction::redo(TextEditor* editor) const {
	oppisite.undo(editor);
}

void UndoBlockDeleteAction::undo(TextEditor* editor) const {
	std::vector<WStringView> lines = Utils::splitAt(text, '\n');
	editor->setLineOverflow(true);

	Vec2 p = start;
	for (std::size_t i = 0; i < lines.size(); i++) {
		editor->cursorMoveTo(p);
		while (i < lines.size() && lines[i].length() == 0) {
			p.y += 1;
			i++;
			editor->cursorMoveTo(p);
		}
		if (i >= lines.size()) break;
		for (WChar chr : lines[i]) editor->insertCharAtCursor(chr);
		p.y += 1;
	}

	editor->cursorMoveTo(start);

	editor->setLineOverflow(false);
}

void UndoBlockDeleteAction::redo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveTo(start);
	editor->startBlockSelection();
	editor->cursorMoveTo(end);
	editor->deleteSelection();
	editor->cursorMoveTo(start);
	editor->stopSelection();

	editor->setLineOverflow(false);
}

void UndoCharacterReplaceAction::undo(TextEditor* editor) const {
	editor->cursorMoveTo(pos);
	editor->replaceCharAtCursor(from);
}

void UndoCharacterReplaceAction::redo(TextEditor* editor) const {
	editor->cursorMoveTo(pos);
	editor->replaceCharAtCursor(to);
}

void UndoIndentAction::undo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveTo(Vec2(0, lineFrom));
	editor->startLineSelection();
	editor->cursorMoveTo(Vec2(0, lineTo));
	editor->deindentSelection();
	editor->stopSelection();

	editor->cursorMoveTo(Vec2(0, lineFrom));

	editor->setLineOverflow(false);
}

void UndoIndentAction::redo(TextEditor* editor) const {
	editor->setLineOverflow(true);

	editor->cursorMoveTo(Vec2(0, lineFrom));
	editor->startLineSelection();
	editor->cursorMoveTo(Vec2(0, lineTo));
	editor->indentSelection();
	editor->stopSelection();

	editor->cursorMoveTo(Vec2(0, lineFrom));

	editor->setLineOverflow(false);
}

void UndoDeindentAction::undo(TextEditor* editor) const {
	oppisite.redo(editor);
}

void UndoDeindentAction::redo(TextEditor* editor) const {
	oppisite.undo(editor);
}
