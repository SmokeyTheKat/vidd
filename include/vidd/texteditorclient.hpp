#ifndef __VIDD_TEXTEDITORCLIENT_HPP__
#define __VIDD_TEXTEDITORCLIENT_HPP__

#include <vidd/client.hpp>
#include <vidd/scrollbar.hpp>
#include <vidd/texteditor.hpp>
#include <vidd/codeview.hpp>
#include <vidd/textprompt.hpp>
#include <vidd/input.hpp>
#include <vidd/procstream.hpp>

#include <string>
#include <array>
#include <map>
#include <optional>

enum class EditMode {
	Normal,
	Insert,
	MultiLineInsert,
	Replace,
	Select,
	LineSelect,
	WordSelect,
	BlockSelect,
	WindowMove,
	Prompt,
	Jump,
};

class TextEditorClient : public Client {
	enum class StatusMessageType {
		None,
		FileWrite,
		UnsavedChanges,
	};

	struct StatusMessage {
		StatusMessageType type = StatusMessageType::None;
		std::string text;
	};

	EditMode mMode = EditMode::Normal;
	TextEditor mEditor;
	CodeView mCodeView;
	ScrollBar mScrollBar;
	WString mLastFind;
	TextPrompt mPrompt;
	std::string mNumberRepeatBuffer;
	std::map<char, Vec2> mMarkers;
	int mOldLineCount = 0;
	bool mShowLineNumbers = true;
	bool mShowStatusBar = true;
	bool mAutoSave = false;
	StatusMessage mStatus;
	Vec2 mWindowDragLatch;
	Vec2 mWindowDragOrigSize;
	std::vector<std::pair<std::string, Vec2>> mJumpPairs;
	std::string mJumpInput;
	std::vector<WChar> mMultiLineBuffer;
	int mMultiLineBufferPtr = 0;
	bool mStartedWindowModeFromMouse = false;
	bool mNoSave = false;

	std::optional<Process> mExecuter;
	int mExecuterPos;

	static bool sIsRecordingMacro;
	static std::vector<Key> sMacroBuffer;

public:
	TextEditorClient(Tab* tab, Input input);

	TextEditor* getEditor(void) { return &mEditor; };
	const TextEditor* getEditor(void) const { return &mEditor; };

	WString getTitle(void) override;

	void showStatusBar(bool state) { mShowStatusBar = state; };
	void toggleStatusBar(void) { showStatusBar(!mShowStatusBar); };

	void resizeEditor(void);

	void toggleLineNumbers(void);

	std::optional<Process>& getExecuter(void) { return mExecuter; };

	void saveFile(void);
	void editFile(std::string_view file);
	void floatFile(std::string_view file);
	void openFile(std::string_view file);
	TextEditorClient* openFloatingFile(std::string_view file);

	void setNoSave(bool value) { mNoSave = value; };

	void tryClose(void);

	void setNoStatus(void);

	void duplicate(void);

	void setNormalBinds(void);
	void setInsertBinds(void);
	void setMultiLineInsertBinds(void);
	void setReplaceBinds(void);
	void setWindowMoveBinds(void);
	void setSelectBinds(void);
	void setCommandBinds(void);
	void setNoBinds(void);

	Key getKey(void);

	void enterDefaultMode(void);
	void enterInsertMode(void);
	void enterInsertModeRight(void);
	void enterInsertModeOnNewLineDown(void);
	void enterInsertModeOnNewLineUp(void);
	void exitInsertMode(void);
	void enterMultiLineInsertMode(void);
	void deleteThenEnterMultiLineInsertMode(void);
	void exitMultiLineInsertMode(void);

	void enterReplaceMode(void);
	void exitReplaceMode(void);
	void replaceChar(void);

	void enterSelectMode(void);
	void enterLineSelectMode(void);
	void enterWordSelectMode(void);
	void enterBlockSelectMode(void);
	void exitSelectMode(void);

	void enterCommandMode(void);
	void exitCommandMode(void);

	void enterJumpMode(void);
	void exitJumpMode(void);

	void exitPrompt(void);

	void enterWindowMoveMode(void);
	void exitWindowMoveMode(void);

	void runMakeText(void);
	void openEmpty(void);

	void getEnv(std::string_view);

	void cursorMoveToNextChar(void);
	void cursorMoveToPrevChar(void);

	void deleteMovement(void);
	void copyMovement(void);
	void lineCopyMovement(void);

	void copyEditorCopyBufferToSystemCopyBuffer(void);

	void openFuzzy(const std::string& title, const std::vector<std::string>& data, std::function<void(std::string)> callback);
	void fuzzyEditFile(void);
	void fuzzyOpenFile(void);
	void fuzzyOpenFloatingFile(void);
	void fuzzyGoto(void);
	void fuzzyGrep(void);

	void toggleRecordingMacro(void);
	void runMacro(void);

	void inputFindNext(void);
	void inputFindPrev(void);
	void repeatFindNext(void);
	void repeatFindPrev(void);
	void findNextUnderCursor(void);
	void findPrevUnderCursor(void);

	void inputReplace(void);

	void setMarker(void);
	void gotoMarker(void);

	void openDirectory(void);
	void openFloatingDirectory(void);
	void openTermianl(void);
	void openFloatingTermianl(void);
	void openLogClient(void);

	void commentHeader(void);

	void executeLine(void);

	void renderStatusBar(void);
	void renderStatus(void);
	void renderLineNumbers(void);

	void failedToOpenBinary(void);

	void onAttach(void) override;
	void onMouseButtonUp(Vec2 pos) override;
	void onRightMouseButtonDown(Vec2 pos) override;
	void onRightMouseButtonDrag(Vec2 pos) override;
	void onLeftMouseButtonDown(Vec2 pos) override;
	void onLeftMouseButtonDrag(Vec2 pos) override;
	void onLeftMouseButtonDoubleDown(Vec2 pos) override;
	void onLeftMouseButtonDoubleDrag(Vec2 pos) override;
	void onLeftMouseButtonTripleDown(Vec2 pos) override;
	void onLeftMouseButtonTripleDrag(Vec2 pos) override;
	Vec2 getCursor(void) override;
	void onResize(void) override;
	void onSelect(void) override;
	void onDeselect(void) override;
	void onPaste(WStringView data) override;
	void unhandledKey(Key key) override;
	void onKeyDown(Key key) override;
	void onPrerender(void) override;
	void render(void) override;

private:
	std::vector<std::pair<std::string, Vec2>> generateJumpKeys(std::vector<Vec2> locations);
};

#endif
