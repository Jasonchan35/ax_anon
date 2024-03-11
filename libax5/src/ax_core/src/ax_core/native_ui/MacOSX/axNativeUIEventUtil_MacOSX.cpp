//
//  axNativeUIEvent_MacOSX.cpp
//  libax_core
//
//  Created by Jason on 2018-06-09.
//
//

#if AX_OS_MACOSX
#include "axNativeUIEventUtil_MacOSX.h"

axNativeUIKeyCode axNativeUIEventUtil_MacOSX::convertKey(NSEvent* inEvent) {
	using KeyCode = axNativeUIKeyCode;
	auto mf = inEvent.modifierFlags;
	
	auto* inStr = inEvent.characters;
	if (inStr != nil && inStr.length > 0) {
		uint16_t inKey = [inStr characterAtIndex:0];
		
		if (inKey >= NSF1FunctionKey && inKey <= NSF35FunctionKey) {
			return axNativeUIKeyCode::F1 + (inKey - NSF1FunctionKey);
		}
		
		switch (inKey) {
//			case NSEnterCharacter:			return KeyCode::;
//	case NSBackspaceCharacter:		return KeyCode::;
			case NSTabCharacter:			return KeyCode::Tab;
//			case NSNewlineCharacter:		return KeyCode::;
//			case NSFormFeedCharacter             = 0x000c,
			case NSCarriageReturnCharacter:	return KeyCode::Enter;
//			case NSBackTabCharacter              = 0x0019,
			case NSDeleteCharacter:	{
				if (mf & NSEventModifierFlagFunction)
					return KeyCode::Delete;
				return KeyCode::Backspace;
			}
//			case NSLineSeparatorCharacter        = 0x2028,
//			case NSParagraphSeparatorCharacter   = 0x2029
		//-------
			case NSUpArrowFunctionKey:		return KeyCode::UpArrow;
			case NSDownArrowFunctionKey:	return KeyCode::DownArrow;
			case NSLeftArrowFunctionKey:	return KeyCode::LeftArrow;
			case NSRightArrowFunctionKey:	return KeyCode::RightArrow;
		//-------
			case NSInsertFunctionKey:       return KeyCode::Insert;
			case NSDeleteFunctionKey:       return KeyCode::Delete;
			case NSHomeFunctionKey:         return KeyCode::Home;
			case NSBeginFunctionKey:        return KeyCode::Begin;
			case NSEndFunctionKey:          return KeyCode::End;
			case NSPageUpFunctionKey:       return KeyCode::PageUp;
			case NSPageDownFunctionKey:     return KeyCode::PageDown;
			case NSPrintScreenFunctionKey:  return KeyCode::PrintScreen;
			case NSScrollLockFunctionKey:   return KeyCode::ScrollLock;
			case NSPauseFunctionKey:        return KeyCode::Pause;
			case NSSysReqFunctionKey:       return KeyCode::SysReq;
			case NSBreakFunctionKey:        return KeyCode::Break;
			case NSResetFunctionKey:        return KeyCode::Reset;
			case NSStopFunctionKey:         return KeyCode::Stop;
			case NSMenuFunctionKey:         return KeyCode::Menu;
			case NSUserFunctionKey:         return KeyCode::User;
			case NSSystemFunctionKey:       return KeyCode::System;
			case NSPrintFunctionKey:        return KeyCode::Print;
		//	case NSClearLineFunctionKey:    return KeyCode::ClearLine;
		//	case NSClearDisplayFunctionKey: return KeyCode::ClearDisplay;
		//	case NSInsertLineFunctionKey:   return KeyCode::InsertLine;
		//	case NSDeleteLineFunctionKey:   return KeyCode::DeleteLine;
		//	case NSInsertCharFunctionKey:   return KeyCode::InsertChar;
		//	case NSDeleteCharFunctionKey:   return KeyCode::DeleteChar;
			case NSPrevFunctionKey:         return KeyCode::Prev;
			case NSNextFunctionKey:         return KeyCode::Next;
			case NSSelectFunctionKey:       return KeyCode::Select;
			case NSExecuteFunctionKey:      return KeyCode::Execute;
			case NSUndoFunctionKey:         return KeyCode::Undo;
			case NSRedoFunctionKey:         return KeyCode::Redo;
			case NSFindFunctionKey:         return KeyCode::Find;
			case NSHelpFunctionKey:         return KeyCode::Help;
			case NSModeSwitchFunctionKey:   return KeyCode::ModeSwitch;
			
		}
	}
	return axNativeUIKeyCode::None;
}

axNativeUIEventModifier axNativeUIEventUtil_MacOSX::convertModifierFlags(NSEventModifierFlags mf) {
	using Modifier = axNativeUIEventModifier;
	auto o = Modifier::None;
	if (mf & NSEventModifierFlagShift	) o |= Modifier::Shift;
	if (mf & NSEventModifierFlagControl	) o |= Modifier::Ctrl;
	if (mf & NSEventModifierFlagOption	) o |= Modifier::Atl;
	if (mf & NSEventModifierFlagCommand	) o |= Modifier::Cmd;
	if (mf & NSEventModifierFlagFunction) o |= Modifier::Fn;
	return o;
}

#endif
