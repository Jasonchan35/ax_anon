#if AX_USE_Win32Api
#include "axNativeUIEventUtil_Win32.h"

axNativeUIKeyCode axNativeUIEventUtil_Win32::convertKey(WPARAM vkey) {
	using KeyCode = axNativeUIKeyCode;
//	AX_LOG("vkey = {?:X}", vkey);
	
	using IntType = axEnum<axNativeUIKeyCode>::IntType;

	if (vkey >= '0' && vkey <= '9')
		return axNativeUIKeyCode::Digi0 + static_cast<IntType>(vkey - '0');

	if (vkey >= 'A' && vkey <= 'Z')
		return axNativeUIKeyCode::A + static_cast<IntType>(vkey - 'A');

	if (vkey >= VK_NUMPAD0 && vkey <= VK_NUMPAD9)
		return axNativeUIKeyCode::NumPad0 + static_cast<IntType>(vkey - VK_NUMPAD0);

	if (vkey >= VK_F1 && vkey <= VK_F24)
		return axNativeUIKeyCode::F1 + static_cast<IntType>(vkey - VK_F1);

	switch (vkey) {
		case VK_BACK:				return KeyCode::Backspace;
		case VK_TAB:				return KeyCode::Tab;
		case VK_CLEAR:				return KeyCode::Clear;

		case VK_RETURN:				return KeyCode::Enter;

		case VK_SHIFT:				return KeyCode::Shift;
		case VK_CONTROL:			return KeyCode::Ctrl;
		case VK_MENU:				return KeyCode::Alt;
		case VK_PAUSE:				return KeyCode::Pause;
		case VK_CAPITAL:			return KeyCode::CapsLock;

		case VK_KANA:				return KeyCode::IME_Kana;
//		case VK_HANGUL:				return KeyCode::IME_Hangul;
		case VK_JUNJA:				return KeyCode::IME_Junja;
		case VK_FINAL:				return KeyCode::IME_Final;
//		case VK_HANJA:				return KeyCode::IME_Hanja;
		case VK_KANJI:				return KeyCode::IME_Kanji;

		case VK_ESCAPE:				return KeyCode::Escape;

		case VK_CONVERT:			return KeyCode::IME_Convert;
		case VK_NONCONVERT:			return KeyCode::IME_NonConvert;
		case VK_ACCEPT:				return KeyCode::IME_Accept;
		case VK_MODECHANGE:			return KeyCode::IME_ModeChange;

		case VK_SPACE:				return KeyCode::Space;
		case VK_PRIOR:				return KeyCode::PageUp;
		case VK_NEXT:				return KeyCode::PageDown;
		case VK_END:				return KeyCode::End;
		case VK_HOME:				return KeyCode::Home;
		case VK_LEFT:				return KeyCode::LeftArrow;
		case VK_UP:					return KeyCode::UpArrow;
		case VK_RIGHT:				return KeyCode::RightArrow;
		case VK_DOWN:				return KeyCode::DownArrow;
		case VK_SELECT:				return KeyCode::Select;
		case VK_PRINT:				return KeyCode::Print;
		case VK_EXECUTE:			return KeyCode::Execute;
		case VK_SNAPSHOT:			return KeyCode::PrintScreen;
		case VK_INSERT:				return KeyCode::Insert;
		case VK_DELETE:				return KeyCode::Delete;
		case VK_HELP:				return KeyCode::Help;
		// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
		// 0x40 : unassigned
		// VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
		case VK_LWIN:				return KeyCode::LWin;
		case VK_RWIN:				return KeyCode::RWin;
		case VK_APPS:				return KeyCode::Apps;
		// 0x5E : reserved
		case VK_SLEEP:				return KeyCode::Sleep;

		case VK_MULTIPLY:			return KeyCode::NumPadMul;
		case VK_ADD:				return KeyCode::NumPadAdd;
		case VK_SEPARATOR:			return KeyCode::Separator;
		case VK_SUBTRACT:			return KeyCode::NumPadSub;
		case VK_DECIMAL:			return KeyCode::NumPadDecimal;
		case VK_DIVIDE:				return KeyCode::NumPadDiv;
		// VK_F1 - VK_F24 (0x70 - 0x87)
		// 0x88 - 0x8F : unassigned
		case VK_NUMLOCK:			return KeyCode::NumLock;
		case VK_SCROLL:				return KeyCode::ScrollLock;
		// NEC PC-9800 kbd definitions
		case VK_OEM_NEC_EQUAL:		return KeyCode::NumPadEqual;
		// Fujitsu/OASYS kbd definitions
		//#define VK_OEM_FJ_JISHO   0x92   // 'Dictionary' key
		//#define VK_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
		//#define VK_OEM_FJ_TOUROKU 0x94   // 'Register word' key
		//#define VK_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
		//#define VK_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key
		// 0x97 - 0x9F : unassigned
		/*
		 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
		 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
		 * No other API or message will distinguish left and right keys in this way.
		 */
		case VK_LSHIFT:				return KeyCode::LShift;
		case VK_RSHIFT:				return KeyCode::RShift;
		case VK_LCONTROL:			return KeyCode::LCtrl;
		case VK_RCONTROL:			return KeyCode::RCtrl;
		case VK_LMENU:				return KeyCode::LAlt;
		case VK_RMENU:				return KeyCode::RAlt;
		// 
	#if(_WIN32_WINNT >= 0x0500)
		case VK_BROWSER_BACK:		return KeyCode::BrowserBack;
		case VK_BROWSER_FORWARD:	return KeyCode::BrowserForward;
		case VK_BROWSER_REFRESH:	return KeyCode::BrowserRefresh;
		case VK_BROWSER_STOP:		return KeyCode::BrowserStop;
		case VK_BROWSER_SEARCH:		return KeyCode::BrowserBack;
		case VK_BROWSER_FAVORITES:	return KeyCode::BrowserFavorites;
		case VK_BROWSER_HOME:		return KeyCode::BrowserHome;
		//
		case VK_VOLUME_MUTE:		return KeyCode::VolumeMute;
		case VK_VOLUME_DOWN:		return KeyCode::VolumeDown;
		case VK_VOLUME_UP:			return KeyCode::VolumeUp;
		case VK_MEDIA_NEXT_TRACK:	return KeyCode::MediaNextTrack;
		case VK_MEDIA_PREV_TRACK:	return KeyCode::MediaPrevTrack;
		case VK_MEDIA_STOP:			return KeyCode::MediaStop;
		case VK_MEDIA_PLAY_PAUSE:	return KeyCode::MediaPlayPause;
		//
		case VK_LAUNCH_MAIL:		return KeyCode::LaunchMail;
		case VK_LAUNCH_MEDIA_SELECT:return KeyCode::LaunchMediaSelect;
		case VK_LAUNCH_APP1:		return KeyCode::LaunchMediaApp1;
		case VK_LAUNCH_APP2:		return KeyCode::LaunchMediaApp2;
	#endif /* _WIN32_WINNT >= 0x0500 */
		/*
		 * 0xB8 - 0xB9 : reserved
		 */
		case VK_OEM_1:				return KeyCode::SemiColon;	// ';:' key
		case VK_OEM_PLUS:			return KeyCode::Equals;		// '=+' key
		case VK_OEM_COMMA:			return KeyCode::Comma;		// ',<' key
		case VK_OEM_MINUS:			return KeyCode::Minus;		// '-_' key
		case VK_OEM_PERIOD:			return KeyCode::Period;		// '.>' key
		case VK_OEM_2:				return KeyCode::Slash;		// '/?' key
		case VK_OEM_3:				return KeyCode::BackQuote;	// '`~' key
		/*
		 * 0xC1 - 0xD7 : reserved
		 */
		/*
		 * 0xD8 - 0xDA : unassigned
		 */
		case VK_OEM_4:				return KeyCode::LBracket;	//  '[{' for US
		case VK_OEM_5:				return KeyCode::BackSlash;	//  '\|' for US
		case VK_OEM_6:				return KeyCode::RBracket;	//  ']}' for US
		case VK_OEM_7:				return KeyCode::Quotes;		//  ''"' for US
		//case VK_OEM_8:			return KeyCode::;
		/*
		 * 0xE0 : reserved
		 */
		/*
		 * Various extended or enhanced keyboards
		 */
		//case VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
		//case VK_OEM_102		 0xE2  //  "<>" or "\|" on RT 102-key kbd.
		//case VK_ICO_HELP       0xE3  //  Help key on ICO
		//case VK_ICO_00         0xE4  //  00 key on ICO

	#if(WINVER >= 0x0400)
		case VK_PROCESSKEY:			return axNativeUIKeyCode::ProcessKey;
	#endif /* WINVER >= 0x0400 */
	// #define VK_ICO_CLEAR      0xE6
	#if(_WIN32_WINNT >= 0x0500)
	// #define VK_PACKET         0xE7
	#endif /* _WIN32_WINNT >= 0x0500 */
		/*
		 * 0xE8 : unassigned
		 */
		/*
		 * Nokia/Ericsson definitions
		 */
		//#define VK_OEM_RESET      0xE9
		//#define VK_OEM_JUMP       0xEA
		//#define VK_OEM_PA1        0xEB
		//#define VK_OEM_PA2        0xEC
		//#define VK_OEM_PA3        0xED
		//#define VK_OEM_WSCTRL     0xEE
		//#define VK_OEM_CUSEL      0xEF
		//#define VK_OEM_ATTN       0xF0
		//#define VK_OEM_FINISH     0xF1
		//#define VK_OEM_COPY       0xF2
		//#define VK_OEM_AUTO       0xF3
		//#define VK_OEM_ENLW       0xF4
		//#define VK_OEM_BACKTAB    0xF5
		//
		//#define VK_ATTN           0xF6
		//#define VK_CRSEL          0xF7
		//#define VK_EXSEL          0xF8
		//#define VK_EREOF          0xF9
		//#define VK_PLAY           0xFA
		//#define VK_ZOOM           0xFB
		//#define VK_NONAME         0xFC
		//#define VK_PA1            0xFD
		//#define VK_OEM_CLEAR      0xFE
		/*
		 * 0xFF : reserved
		 */
		default:		return axNativeUIKeyCode::None;
	}
}

#endif