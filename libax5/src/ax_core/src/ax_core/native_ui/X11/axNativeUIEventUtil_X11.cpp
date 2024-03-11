#if AX_USE_X11
#include "axNativeUIEventUtil_X11.h"

axNativeUIKeyCode ax_X11_Util::convertKey(KeySym vkey) {
//	AX_LOG("vkey = {?:X}", vkey);
	using KeyCode = axNativeUIKeyCode;

	if (vkey >= XK_F1 && vkey <= XK_F35) {
		return static_cast<KeyCode>(vkey - XK_F1) + KeyCode::F1;
	}

	switch (vkey) {
		case XK_BackSpace:		return KeyCode::Backspace;	//  0xff08  /* Back space, back char */
		case XK_Tab:            return KeyCode::Tab;  		// 0xff09
	//	case XK_Linefeed:       return KeyCode::  			// 0xff0a  /* Linefeed, LF */
		case XK_Clear:          return KeyCode::Clear;		// 0xff0b
		case XK_Return:         return KeyCode::Enter;		// 0xff0d  /* Return, enter */
		case XK_Pause:          return KeyCode::Pause;		// 0xff13  /* Pause, hold */
		case XK_Scroll_Lock:    return KeyCode::ScrollLock;	// 0xff14
	//	case XK_Sys_Req:        return KeyCode::  			// 0xff15
		case XK_Escape:         return KeyCode::Escape;		// 0xff1b
		case XK_Delete:         return KeyCode::Delete;		// 0xffff  /* Delete, rubout */

	/* International & multi-key character composition */

		// #define XK_Multi_key                     0xff20  /* Multi-key character compose */
		// #define XK_Codeinput                     0xff37
		// #define XK_SingleCandidate               0xff3c
		// #define XK_MultipleCandidate             0xff3d
		// #define XK_PreviousCandidate             0xff3e

		/* Japanese keyboard support */
		// #define XK_Kanji                         0xff21  /* Kanji, Kanji convert */
		// #define XK_Muhenkan                      0xff22  /* Cancel Conversion */
		// #define XK_Henkan_Mode                   0xff23  /* Start/Stop Conversion */
		// #define XK_Henkan                        0xff23  /* Alias for Henkan_Mode */
		// #define XK_Romaji                        0xff24  /* to Romaji */
		// #define XK_Hiragana                      0xff25  /* to Hiragana */
		// #define XK_Katakana                      0xff26  /* to Katakana */
		// #define XK_Hiragana_Katakana             0xff27  /* Hiragana/Katakana toggle */
		// #define XK_Zenkaku                       0xff28  /* to Zenkaku */
		// #define XK_Hankaku                       0xff29  /* to Hankaku */
		// #define XK_Zenkaku_Hankaku               0xff2a  /* Zenkaku/Hankaku toggle */
		// #define XK_Touroku                       0xff2b  /* Add to Dictionary */
		// #define XK_Massyo                        0xff2c  /* Delete from Dictionary */
		// #define XK_Kana_Lock                     0xff2d  /* Kana Lock */
		// #define XK_Kana_Shift                    0xff2e  /* Kana Shift */
		// #define XK_Eisu_Shift                    0xff2f  /* Alphanumeric Shift */
		// #define XK_Eisu_toggle                   0xff30  /* Alphanumeric toggle */
		// #define XK_Kanji_Bangou                  0xff37  /* Codeinput */
		// #define XK_Zen_Koho                      0xff3d  /* Multiple/All Candidate(s) */
		// #define XK_Mae_Koho                      0xff3e  /* Previous Candidate */

	/* 0xff31 thru 0xff3f are under XK_KOREAN */

	/* Cursor control & motion */
		case XK_Home:                       return KeyCode::Home;		//  0xff50
		case XK_Left:                       return KeyCode::LeftArrow;	//  0xff51  /* Move left, left arrow */
		case XK_Up:                         return KeyCode::UpArrow;	//  0xff52  /* Move up, up arrow */
		case XK_Right:                      return KeyCode::RightArrow;	//  0xff53  /* Move right, right arrow */
		case XK_Down:                       return KeyCode::DownArrow;	//  0xff54  /* Move down, down arrow */
//		case XK_Prior:                      return KeyCode::Prev;		//  0xff55  /* Prior, previous */
		case XK_Page_Up:                    return KeyCode::PageUp;		//  0xff55
//		case XK_Next:                       return KeyCode::Next;		//  0xff56  /* Next */
		case XK_Page_Down:                  return KeyCode::PageDown;	//  0xff56
		case XK_End:                        return KeyCode::End;		//  0xff57  /* EOL */
		case XK_Begin:                      return KeyCode::Begin;		//  0xff58  /* BOL */


		/* Misc functions */

		case XK_Select:                     return KeyCode::Select; 		//   0xff60  /* Select, mark */
		case XK_Print:                      return KeyCode::Print; 			//   0xff61
		case XK_Execute:                    return KeyCode::Execute;		//   0xff62  /* Execute, run, do */
		case XK_Insert:                     return KeyCode::Insert; 		//   0xff63  /* Insert, insert here */
		case XK_Undo:                       return KeyCode::Undo; 			//   0xff65
		case XK_Redo:                       return KeyCode::Redo; 			//   0xff66  /* Redo, again */
		case XK_Menu:                       return KeyCode::Menu; 			//   0xff67
		case XK_Find:                       return KeyCode::Find; 			//   0xff68  /* Find, search */
		case XK_Cancel:                     return KeyCode::Cancel;			//   0xff69  /* Cancel, stop, abort, exit */
		case XK_Help:                       return KeyCode::Help; 			//   0xff6a  /* Help */
		case XK_Break:                      return KeyCode::Break; 			//   0xff6b
		case XK_Mode_switch:                return KeyCode::ModeSwitch; 	//   0xff7e  /* Character set switch */
//		case XK_script_switch:              return KeyCode::ScriptSwitch;	//   0xff7e  /* Alias for mode_switch */
		case XK_Num_Lock:                   return KeyCode::NumLock;		//   0xff7f

		/* Keypad functions, keypad numbers cleverly chosen to map to ASCII */

//		case XK_KP_Space:                   return KeyCode::; //   0xff80  /* Space */
//		case XK_KP_Tab:                     return KeyCode::; //   0xff89
		case XK_KP_Enter:                   return KeyCode::NumPadEnter; //   0xff8d  /* Enter */
		// case XK_KP_F1:                      return KeyCode::; //   0xff91  /* PF1, KP_A, ... */
		// case XK_KP_F2:                      return KeyCode::; //   0xff92
		// case XK_KP_F3:                      return KeyCode::; //   0xff93
		// case XK_KP_F4:                      return KeyCode::; //   0xff94
		case XK_KP_Home:                    return KeyCode::NumPadHome; 	//   0xff95
		case XK_KP_Left:                    return KeyCode::NumPadLeft; 	//   0xff96
		case XK_KP_Up:                      return KeyCode::NumPadUp;		//   0xff97
		case XK_KP_Right:                   return KeyCode::NumPadRight;	//   0xff98
		case XK_KP_Down:                    return KeyCode::NumPadDown;		//   0xff99
//		case XK_KP_Prior:                   return KeyCode::NumPadPrev;		//   0xff9a
		case XK_KP_Page_Up:                 return KeyCode::NumPadPageUp; 	//   0xff9a
//		case XK_KP_Next:                    return KeyCode::NumPadNext; 	//   0xff9b
		case XK_KP_Page_Down:               return KeyCode::NumPadPageDown; //   0xff9b
		case XK_KP_End:                     return KeyCode::NumPadEnd;		//   0xff9c
		case XK_KP_Begin:                   return KeyCode::NumPadBegin;	//   0xff9d
		case XK_KP_Insert:                  return KeyCode::NumPadInsert;	//   0xff9e
		case XK_KP_Delete:                  return KeyCode::NumPadDelete;	//   0xff9f
		case XK_KP_Equal:                   return KeyCode::NumPadEqual;	//   0xffbd  /* Equals */
		case XK_KP_Multiply:                return KeyCode::NumPadMul;		//   0xffaa
		case XK_KP_Add:                     return KeyCode::NumPadAdd;		//   0xffab
		case XK_KP_Separator:               return KeyCode::Separator;		//   0xffac  /* Separator, often comma */
		case XK_KP_Subtract:                return KeyCode::NumPadSub;		//   0xffad
		case XK_KP_Decimal:                 return KeyCode::NumPadDecimal;	//   0xffae
		case XK_KP_Divide:                  return KeyCode::NumPadDiv;		//   0xffaf

		case XK_KP_0:                       return KeyCode::NumPad0; //  0xffb0
		case XK_KP_1:                       return KeyCode::NumPad1; //  0xffb1
		case XK_KP_2:                       return KeyCode::NumPad2; //  0xffb2
		case XK_KP_3:                       return KeyCode::NumPad3; //  0xffb3
		case XK_KP_4:                       return KeyCode::NumPad4; //  0xffb4
		case XK_KP_5:                       return KeyCode::NumPad5; //  0xffb5
		case XK_KP_6:                       return KeyCode::NumPad6; //  0xffb6
		case XK_KP_7:                       return KeyCode::NumPad7; //  0xffb7
		case XK_KP_8:                       return KeyCode::NumPad8; //  0xffb8
		case XK_KP_9:                       return KeyCode::NumPad9; //  0xffb9
		/*
		* Auxiliary functions; note the duplicate definitions for left and right
		* function keys;  Sun keyboards and a few other manufacturers have such
		* function key groups on the left and/or right sides of the keyboard.
		* We've not found a keyboard with more than 35 function keys total.
		*/
		/* Modifiers */

		// #define XK_Shift_L                       0xffe1  /* Left shift */
		// #define XK_Shift_R                       0xffe2  /* Right shift */
		// #define XK_Control_L                     0xffe3  /* Left control */
		// #define XK_Control_R                     0xffe4  /* Right control */
		// #define XK_Caps_Lock                     0xffe5  /* Caps lock */
		// #define XK_Shift_Lock                    0xffe6  /* Shift lock */

		// #define XK_Meta_L                        0xffe7  /* Left meta */
		// #define XK_Meta_R                        0xffe8  /* Right meta */
		// #define XK_Alt_L                         0xffe9  /* Left alt */
		// #define XK_Alt_R                         0xffea  /* Right alt */
		// #define XK_Super_L                       0xffeb  /* Left super */
		// #define XK_Super_R                       0xffec  /* Right super */
		// #define XK_Hyper_L                       0xffed  /* Left hyper */
		// #define XK_Hyper_R                       0xffee  /* Right hyper */
	}
	return KeyCode::None;
}

bool ax_X11_Util::checkError(const axSourceLoc& loc, int errorCode) {
	if (errorCode == 0) return true;

	switch (errorCode) {
	#define AX_MACRO_OP(V) case V:	AX_LOG("error : "#V"\n  {?}", loc); break;
		AX_MACRO_OP(ax_X11_BadRequest);
		AX_MACRO_OP(ax_X11_BadValue);
		AX_MACRO_OP(ax_X11_BadWindow);
		AX_MACRO_OP(ax_X11_BadPixmap);
		AX_MACRO_OP(ax_X11_BadAtom);
		AX_MACRO_OP(ax_X11_BadCursor);
		AX_MACRO_OP(ax_X11_BadFont);
		AX_MACRO_OP(ax_X11_BadMatch);
		AX_MACRO_OP(ax_X11_BadDrawable);
		AX_MACRO_OP(ax_X11_BadAccess);

		AX_MACRO_OP(ax_X11_BadAlloc);
		AX_MACRO_OP(ax_X11_BadColor);
		AX_MACRO_OP(ax_X11_BadGC);
		AX_MACRO_OP(ax_X11_BadIDChoice);
		AX_MACRO_OP(ax_X11_BadName);
		AX_MACRO_OP(ax_X11_BadLength);
		AX_MACRO_OP(ax_X11_BadImplementation);
	#undef AX_MACRO_OP
	}
	return false;
}

#endif