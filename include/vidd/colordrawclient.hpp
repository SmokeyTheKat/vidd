#ifndef __VIDD_COLOR_DRAW_CLIENT_HPP__
#define __VIDD_COLOR_DRAW_CLIENT_HPP__

#include <vidd/client.hpp>
#include <vidd/scrollbar.hpp>
#include <vidd/textprompt.hpp>
#include <vidd/framebuffer.hpp>
#include <vidd/input.hpp>

#include <string>
#include <vector>
#include <array>
#include <utility>
#include <map>
#include <optional>

class ColorDrawClient : public Client {
	enum class Mode {
		Normal,
		Insert,
		Select,
		Prompt,
	};
	bool mTextMode = false;
	Mode mMode;
	TextPrompt mPrompt;
	Vec2 mWindowDragLatch;
	Vec2 mWindowDragOrigSize;
	bool mStartedWindowModeFromMouse = false;

	std::string mFileName;

	std::vector<Color> mPalette;

	Vec2 mCanvasSize;
	Vec2 mCursor;

	Vec2 mActionStart;

	FrameBuffer mCopyBuffer;
	FrameBuffer* mFb;
	std::vector<FrameBuffer> mFbs;
	bool mMergeLayers = false;
	Style mStyle;

	WChar mSelectedChar = ' ';

public:
	ColorDrawClient(Tab* tab, std::string file);

	void toggleMergeLayers(void);

	void openDirectory(void);

	void enterCommandMode(void);
	void exitCommandMode(void);

	void enterInsertMode(void);
	void enterTextInsertMode(void);
	void exitInsertMode(void);

	void enterSelectMode(void);
	void exitSelectMode(void);

	void resizeCanvas(int width, int height);

	void addColor(std::vector<std::string_view> params);

	std::pair<Vec2, Vec2> getSelectionArea(void);

	void save(void);
	void load(void);

	void selectLayer(void);
	void returnToTextEditor(void);
	void copy(void);
	void paste(void);
	void pasteTransparent(void);
	void swapCursor(void);
	void cursorMove(Vec2 d);
	void fillChar(void);
	void fillBox(int type = 0);
	void placeChar(void);
	void selectNewChar(void);
	void selectFg(void);
	void selectBg(void);
	void selectFormat(void);
	void selectStyle(void);
	void selectChar(void);
	void replaceFg(void);
	void replaceBg(void);
	void replaceFormat(void);
	void replaceStyle(void);
	void replaceChar(void);
	void replaceFillChar(void);
	void backspace(void);
	void doDelete(void);
	void textBackspace(void);
	void textDelete(void);
	void cursorMoveFirstCharacter(void);
	void cursorMoveLastCharacter(void);
	void tryClose(void);
	void toggleCaptailizationAtCursor(void);

	void renderStatusBar(void);

	void onAttach(void) override;
	Vec2 getCursor(void) override;
	void onResize(void) override;
	void onSelect(void) override;
	void onDeselect(void) override;
	void onPaste(WStringView data) override;
	void unhandledKey(Key key) override;
	void onKeyDown(Key key) override;
	void onLeftMouseButtonDown(Vec2 pos) override;
	void onRightMouseButtonDown(Vec2 pos) override;
	void onPrerender(void) override;
	void render(void) override;

private:
	std::vector<std::pair<std::string, Vec2>> generateJumpKeys(std::vector<Vec2> locations);
};

#endif

