#ifndef __VIDD_EVENT_HPP__
#define __VIDD_EVENT_HPP__

#include <vidd/keys.hpp>

#include <variant>

struct KeyEvent {
	Key key;

	KeyEvent(Key key)
	: key(key) {};
};

enum class MouseEventType {
	Move,
	LeftButtonDown,
	LeftButtonDoubleDown,
	LeftButtonTripleDown,
	LeftButtonDrag,
	LeftButtonDoubleDrag,
	LeftButtonTripleDrag,
	RightButtonDown,
	RightButtonDrag,
	ButtonUp,
	ScrollDown,
	ScrollUp,
};

struct MouseEvent {
	MouseEventType type;
	Vec2 pos;

	MouseEvent(MouseEventType type, Vec2 pos)
	: type(type), pos(pos) {};
};

enum class PasteEventType {
	Start,
	End
};

struct PasteEvent {
	PasteEventType type;

	PasteEvent(PasteEventType type)
	: type(type) {};
};

enum class EventType {
	Key,
	Mouse,
	Paste,
	ForceQuit,
	None,
};

struct NoneEvent {
};

struct Event {
	EventType type;
	std::variant<
		KeyEvent,
		MouseEvent,
		PasteEvent,
		NoneEvent
	> event;

	Event(void)
	: type(EventType::None), event(NoneEvent()) {};
	Event(EventType type, std::variant<KeyEvent, MouseEvent, PasteEvent, NoneEvent> event)
	: type(type), event(event) {};
};

#endif
