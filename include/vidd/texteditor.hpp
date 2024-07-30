#ifndef __VIDD_TEXTEDITOR_HPP__
#define __VIDD_TEXTEDITOR_HPP__

#include <vidd/wchar.hpp>
#include <vidd/cursor.hpp>
#include <vidd/buffer.hpp>
#include <vidd/line.hpp>
#include <vidd/selection.hpp>
#include <vidd/undo.hpp>
#include <vidd/input.hpp>
#include <vidd/language.hpp>

#include <memory>
#include <string>

static const int gTabWidth = 4;

struct ViewPort {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	ViewPort(int x, int y)
	: x(x), y(y) {};

	ViewPort(int x, int y, int w, int h)
	: x(x), y(y), width(w), height(h) {};
};

enum class CopyType {
	Normal,
	Line,
};

struct CopyBuffer {
	WString data;
	CopyType type;
};

class TextEditor {
	std::string mName;
	std::unique_ptr<Buffer> mBuffer;
	Cursor mCursor;
	ViewPort mView;
	bool mLineEndOverflow = false;
	Selection mSelection;
	bool mSelecting = false;
	UndoBuffer mUndoBuffer;
	bool mTrackUndos = true;
	bool mViewChanged = false;
	bool mUnsavedChanges = false;

	bool mOutputTabs = true;

	std::string mLanguageOverride;

	static CopyBuffer mCopyBuffer;

public:
	TextEditor(Input input);
	~TextEditor(void);

	std::string getFileDataPath(void);
	void saveFileData(void);
	void loadFileData(void);

	bool viewHasChanged(void) const { return mViewChanged; };
	void unsetViewChanged(void) { mViewChanged = false; };

	bool hasUnsavedChanges(void) const { return mUnsavedChanges; };
	void unsetUnsavedChanges(void) { mUnsavedChanges = false; };

	void setNoTabs(void) { mOutputTabs = false; };

	std::string getFileName(void) const { return mName; };
	std::string_view getFileExtension(void) const {
		return std::string_view(mName).substr(mName.find_last_of(".") + 1, mName.length());
	};

	const Language* getLanguage(void) const {
		if (mLanguageOverride.length() > 0) {
			return getLanguageFromFileName(mLanguageOverride);
		} else {
			return getLanguageFromFileName(mName);
		}
	}

	void setLanguageOverride(std::string name) {
		mLanguageOverride = name;
	}

	bool isSelecting(void) const {
		return mSelecting;
	};

	void resize(Vec2 size);

	Line* getLineAtCursor(void) const { return mCursor.y; };

	Cursor& getCursor(void) { return mCursor; };
	const Cursor& getCursor(void) const { return mCursor; };
	void setCursor(Cursor cur) { mCursor = cur; };

	CopyBuffer& getCopyBuffer(void) { return mCopyBuffer; };

	void undo(void) { mUndoBuffer.undo(); };
	void redo(void) { mUndoBuffer.redo(); };

	template <class T, class... Args>
	void registerUndoAction(Args&&... args) {
		if (mTrackUndos) {
			mUndoBuffer.registerAction(std::make_unique<T>(std::forward<Args>(args)...));
		}
	}

	Vec2 getCursorRelativePosition(void) const {
		return Vec2(
			mCursor.x - mView.x,
			mCursor.y->number - mView.y
		);
	};

	Vec2 getCursorPosition(void) const {
		return Vec2(mCursor.x, mCursor.y->number);
	};

	Vec2 relativeToAbsolutePosition(Vec2 rel) const {
		return Vec2(mView.x + rel.x, mView.y + rel.y);
	};

	ViewPort& getViewPort(void) { return mView; };
	const ViewPort& getViewPort(void) const { return mView; };

	Selection& getSelection(void) { return mSelection; };
	const Selection& getSelection(void) const { return mSelection; };

	void setLineOverflow(bool value) { mLineEndOverflow = value; restrictCursor(); };

	int getLineCount(void);

	void viewChangedIfSelecting(void);

	void editFile(Input input);
	void saveFile(std::string name = "");

	void startSelection(void);
	void startLineSelection(void);
	void startWordSelection(void);
	void startBlockSelection(void);
	void endSelection(void);
	void stopSelection(void);

	void deleteSelection(void);
	WString readNormalSelectionToString(void);
	WString readLineSelectionToString(void);
	WString readBlockSelectionToString(void);
	void copyNormalSelection(void);
	void copyLineSelection(void);
	void copySelection(void);
	void indentSelection(void);
	void deindentSelection(void);

	void indentAtCursor(void);
	void deindentAtCursor(void);

	void toggleCaptailizationAtCursor(void);

	void toggleCommentAtCursor(void);
	void toggleCommentAtSelection(void);

	void paste(void);
	void pasteBack(void);

	void swapCursorSelectionEnd(void);

	int getLineEnd(void);

	void insertLineDownFromCursor(void);
	void insertLineUpFromCursor(void);

	void splitLineAtCursor(void);

	Line* mergeLineUpAtCursor(void);

	void insertAtCursor(WStringView data);

	bool insertCharAtCursor(WChar chr);
	void replaceCharAtCursor(WChar chr);

	void deleteLineAtCursor(void);
	void deleteAtCursor(void);
	void backspaceAtCursor(void);

	void replaceNextFromCursor(WStringView find, WStringView replace);

	void adjustView(void);
	void fixCursor(void);
	void restrictCursor(void);

	void viewCenterAroundCursor(void);
	void viewScrollY(int dy);
	void viewScrollX(int dx);
	void viewScrollToY(int y);

	WStringView getWordUnderCursor(void);
	char getCharAtCursor(void);

	bool cursorIsAtStartOfWord(void);
	bool cursorIsAtEndOfWord(void);
	bool cursorIsAtTab(void);

	void cursorMoveNextWord(void);
	void cursorMovePrevWord(void);
	void cursorMoveNextWordEnd(void);

	void cursorMoveNextParagraph(void);
	void cursorMovePrevParagraph(void);

	void cursorMoveToViewCenter(void);
	void cursorMoveToInView(void);
	void cursorMoveY(int dy);
	void cursorMoveX(int dx);
	void cursorMoveToY(int y);
	void cursorMoveToX(int x);
	void cursorQuietMoveTo(int x, int y);
	void cursorMoveTo(int x, int y);
	void cursorMoveTo(Vec2 pos);
	void cursorMoveTo(Cursor cur);
	void cursorMoveToWordStart(void);
	void cursorMoveToNextIndentationLevel(void);
	void cursorMoveToPrevionsIndentationLevel(void);

	void cursorMoveToFirstLine(void);
	void cursorMoveToLastLine(void);

	void cursorMoveToLineStart(void);
	void cursorMoveToLineEnd(void);

	Cursor findNextFromCursor(WStringView find) const;
	Cursor findPrevFromCursor(WStringView find) const;

	std::vector<Vec2> indexWordsLocations(void);

private:
	void editDeleteSelection(void);
};

#endif
