#ifndef __VIDD_UNDO_HPP__
#define __VIDD_UNDO_HPP__

#include <vidd/wchar.hpp>
#include <vidd/pos.hpp>

#include <vector>
#include <memory>

class TextEditor;

struct UndoAction {
	virtual ~UndoAction(void) {};
	virtual void undo(TextEditor* editor) const = 0;
	virtual void redo(TextEditor* editor) const = 0;
};

class UndoBuffer {
	using ActionStack = std::vector<std::unique_ptr<UndoAction>>;

	TextEditor* mEditor;
	bool mLocked = false;
	ActionStack mActions;
	std::size_t mPos = 0;

public:
	UndoBuffer(TextEditor* editor)
	: mEditor(editor) {};

	void clear(void);

	bool undo(void);
	bool redo(void);

	void registerAction(std::unique_ptr<UndoAction> action);
};

struct UndoLineMergeAction : public UndoAction {
	int line;
	int mergePoint;

	UndoLineMergeAction(int line, int mergePoint)
	: line(line), mergePoint(mergePoint) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoLineSplitAction : public UndoAction {
	UndoLineMergeAction oppisite;

	UndoLineSplitAction(int line, int mergePoint)
	: oppisite(line, mergePoint) {};

	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoInsertLineDownAction : public UndoAction {
	int line;

	UndoInsertLineDownAction(int line)
	: line(line) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoInsertLineUpAction : public UndoAction {
	int line;

	UndoInsertLineUpAction(int line)
	: line(line) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoInsertAction : public UndoAction {
	Vec2 start;
	Vec2 end;
	WString text;

	UndoInsertAction(Vec2 start, Vec2 end, WString text)
	: start(start), end(end), text(text) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoDeleteAction : public UndoAction {
	UndoInsertAction oppisite;

	UndoDeleteAction(Vec2 start, Vec2 end, WString text)
	: oppisite(start, end, text) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoBlockDeleteAction : public UndoAction {
	Vec2 start;
	Vec2 end;
	WString text;

	UndoBlockDeleteAction(Vec2 start, Vec2 end, WString text)
	: start(start), end(end), text(text) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoCharacterReplaceAction : public UndoAction {
	Vec2 pos;
	WChar from;
	WChar to;

	UndoCharacterReplaceAction(Vec2 pos, WChar from, WChar to)
	: pos(pos), from(from), to(to) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoIndentAction : public UndoAction {
	int lineFrom;
	int lineTo;

	UndoIndentAction(int lineFrom, int lineTo)
	: lineFrom(lineFrom), lineTo(lineTo) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

struct UndoDeindentAction : public UndoAction {
	UndoIndentAction oppisite;

	UndoDeindentAction(int lineFrom, int lineTo)
	: oppisite(lineFrom, lineTo) {};
	void undo(TextEditor* editor) const override;
	void redo(TextEditor* editor) const override;
};

#endif
