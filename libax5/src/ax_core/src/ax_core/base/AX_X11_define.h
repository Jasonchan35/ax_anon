#pragma once

#if AX_USE_X11 || AX_USE_Gtk

#if AX_TRY_USE_OPENGL
	#define ax_HAS_GLEW 1
	#define GLEW_STATIC 1
	#include <ax_core/glew/glew.h>

	#define GLEW_STATIC 1
	#include <ax_core/glew/glxew.h>
#endif

#define BOOL ax_X11_BOOL
using ax_X11_BOOL = unsigned char;

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/keysymdef.h>
#include <X11/cursorfont.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glxext.h>
//#include <GL/glx.h>

AX_STATIC_ASSERT(axTypeTraits::isSame<BOOL, ax_X11_BOOL>::value);
#undef BOOL

#if AX_SIZEOF_POINTER == 8
	using ax_X11_XID = axUInt64;
#elif AX_SIZEOF_POINTER == 4;
	using ax_X11_XID = axUInt32;
#else
	#error "Unknown size for ax_X11_Window"
#endif

// struct ax_X11_Display		{ struct V; V* v{nullptr}; };
// struct ax_X11_XVisualInfo	{ struct V; V* v{nullptr}; };
// struct ax_X11_GLXContext		{ struct V; V* v{nullptr}; };
// struct ax_X11_XIM			{ struct V; V* v{nullptr}; };
// struct ax_X11_XIC			{ struct V; V* v{nullptr}; };
// struct ax_X11_XContext		{ axUInt32   v{0}; };
// struct ax_X11_Window 		{ ax_X11_XID v{0}; };
// struct ax_X11_Drawable		{ ax_X11_XID v{0}; };
// struct ax_X11_Font			{ ax_X11_XID v{0}; };
// struct ax_X11_Pixmap			{ ax_X11_XID v{0}; };
// struct ax_X11_Cursor			{ ax_X11_XID v{0}; };
// struct ax_X11_Colormap		{ ax_X11_XID v{0}; };
// struct ax_X11_GContext		{ ax_X11_XID v{0}; };
// struct ax_X11_KeySym			{ ax_X11_XID v{0}; };
// struct ax_X11_Atom			{ ax_X11_XID v{0}; };

#define ax_X11_DefinePointerType(T) using ax_X11_##T = T;

ax_X11_DefinePointerType(Display		)
ax_X11_DefinePointerType(XVisualInfo	)
ax_X11_DefinePointerType(GLXContext		)
ax_X11_DefinePointerType(XIM			)
ax_X11_DefinePointerType(XIC			)

#define ax_X11_DefineType(T) using ax_X11_##T = T;

ax_X11_DefineType(XContext	)
ax_X11_DefineType(Window 	)
ax_X11_DefineType(Drawable	)
ax_X11_DefineType(Font		)
ax_X11_DefineType(Pixmap	)
ax_X11_DefineType(Cursor	)
ax_X11_DefineType(Colormap	)
ax_X11_DefineType(GContext	)
ax_X11_DefineType(KeySym	)
ax_X11_DefineType(Atom		)

ax_X11_DefineType(Bool		)
ax_X11_DefineType(Status	)

#define ax_X11_DefineID(T)   const ax_X11_XID ax_X11_##T = T;

ax_X11_DefineID(ParentRelative)

ax_X11_DefineID(CopyFromParent)

ax_X11_DefineID(PointerWindow)
ax_X11_DefineID(InputFocus)
ax_X11_DefineID(PointerRoot)
ax_X11_DefineID(AnyPropertyType)
ax_X11_DefineID(AnyKey)
ax_X11_DefineID(AnyButton)
ax_X11_DefineID(AllTemporary)
ax_X11_DefineID(CurrentTime)
ax_X11_DefineID(NoSymbol)


ax_X11_DefineID(NoEventMask					) // 0L
ax_X11_DefineID(KeyPressMask				) // (1L<<0)  
ax_X11_DefineID(KeyReleaseMask				) // (1L<<1)  
ax_X11_DefineID(ButtonPressMask				) // (1L<<2)  
ax_X11_DefineID(ButtonReleaseMask			) // (1L<<3)  
ax_X11_DefineID(EnterWindowMask				) // (1L<<4)  
ax_X11_DefineID(LeaveWindowMask				) // (1L<<5)  
ax_X11_DefineID(PointerMotionMask			) // (1L<<6)  
ax_X11_DefineID(PointerMotionHintMask		) // (1L<<7)  
ax_X11_DefineID(Button1MotionMask			) // (1L<<8)  
ax_X11_DefineID(Button2MotionMask			) // (1L<<9)  
ax_X11_DefineID(Button3MotionMask			) // (1L<<10) 
ax_X11_DefineID(Button4MotionMask			) // (1L<<11) 
ax_X11_DefineID(Button5MotionMask			) // (1L<<12) 
ax_X11_DefineID(ButtonMotionMask			) // (1L<<13) 
ax_X11_DefineID(KeymapStateMask				) // (1L<<14)
ax_X11_DefineID(ExposureMask				) // (1L<<15) 
ax_X11_DefineID(VisibilityChangeMask		) // (1L<<16) 
ax_X11_DefineID(StructureNotifyMask			) // (1L<<17) 
ax_X11_DefineID(ResizeRedirectMask			) // (1L<<18) 
ax_X11_DefineID(SubstructureNotifyMask		) // (1L<<19) 
ax_X11_DefineID(SubstructureRedirectMask	) // (1L<<20) 
ax_X11_DefineID(FocusChangeMask				) // (1L<<21) 
ax_X11_DefineID(PropertyChangeMask			) // (1L<<22) 
ax_X11_DefineID(ColormapChangeMask			) // (1L<<23) 
ax_X11_DefineID(OwnerGrabButtonMask			) // (1L<<24) 


ax_X11_DefineID(KeyPress			) // 2
ax_X11_DefineID(KeyRelease			) // 3
ax_X11_DefineID(ButtonPress			) // 4
ax_X11_DefineID(ButtonRelease		) // 5
ax_X11_DefineID(MotionNotify		) // 6
ax_X11_DefineID(EnterNotify			) // 7
ax_X11_DefineID(LeaveNotify			) // 8
ax_X11_DefineID(FocusIn				) // 9
ax_X11_DefineID(FocusOut			) // 10
ax_X11_DefineID(KeymapNotify		) // 11
ax_X11_DefineID(Expose				) // 12
ax_X11_DefineID(GraphicsExpose		) // 13
ax_X11_DefineID(NoExpose			) // 14
ax_X11_DefineID(VisibilityNotify	) // 15
ax_X11_DefineID(CreateNotify		) // 16
ax_X11_DefineID(DestroyNotify		) // 17
ax_X11_DefineID(UnmapNotify			) // 18
ax_X11_DefineID(MapNotify			) // 19
ax_X11_DefineID(MapRequest			) // 20
ax_X11_DefineID(ReparentNotify		) // 21
ax_X11_DefineID(ConfigureNotify		) // 22
ax_X11_DefineID(ConfigureRequest	) // 23
ax_X11_DefineID(GravityNotify		) // 24
ax_X11_DefineID(ResizeRequest		) // 25
ax_X11_DefineID(CirculateNotify		) // 26
ax_X11_DefineID(CirculateRequest	) // 27
ax_X11_DefineID(PropertyNotify		) // 28
ax_X11_DefineID(SelectionClear		) // 29
ax_X11_DefineID(SelectionRequest	) // 30
ax_X11_DefineID(SelectionNotify		) // 31
ax_X11_DefineID(ColormapNotify		) // 32
ax_X11_DefineID(ClientMessage		) // 33
ax_X11_DefineID(MappingNotify		) // 34
ax_X11_DefineID(GenericEvent		) // 35
ax_X11_DefineID(LASTEvent			) // 36	// must be bigger than any event #


ax_X11_DefineID(ShiftMask	) // (1<<0)
ax_X11_DefineID(LockMask	) // (1<<1)
ax_X11_DefineID(ControlMask	) // (1<<2)
ax_X11_DefineID(Mod1Mask	) // (1<<3)
ax_X11_DefineID(Mod2Mask	) // (1<<4)
ax_X11_DefineID(Mod3Mask	) // (1<<5)
ax_X11_DefineID(Mod4Mask	) // (1<<6)
ax_X11_DefineID(Mod5Mask	) // (1<<7)

ax_X11_DefineID(ShiftMapIndex	) // 0
ax_X11_DefineID(LockMapIndex	) // 1
ax_X11_DefineID(ControlMapIndex	) // 2
ax_X11_DefineID(Mod1MapIndex	) // 3
ax_X11_DefineID(Mod2MapIndex	) // 4
ax_X11_DefineID(Mod3MapIndex	) // 5
ax_X11_DefineID(Mod4MapIndex	) // 6
ax_X11_DefineID(Mod5MapIndex	) // 7

ax_X11_DefineID(Button1Mask		) // (1<<8)
ax_X11_DefineID(Button2Mask		) // (1<<9)
ax_X11_DefineID(Button3Mask		) // (1<<10)
ax_X11_DefineID(Button4Mask		) // (1<<11)
ax_X11_DefineID(Button5Mask		) // (1<<12)
ax_X11_DefineID(AnyModifier		) // (1<<15)  /* used in GrabButton, GrabKey */

ax_X11_DefineID(Button1		) // 1
ax_X11_DefineID(Button2		) // 2
ax_X11_DefineID(Button3		) // 3
ax_X11_DefineID(Button4		) // 4
ax_X11_DefineID(Button5		) // 5

const unsigned int ax_X11_Button8 = 8; 
const unsigned int ax_X11_Button9 = 9;

ax_X11_DefineID(NotifyNormal		) // 0
ax_X11_DefineID(NotifyGrab			) // 1
ax_X11_DefineID(NotifyUngrab		) // 2
ax_X11_DefineID(NotifyWhileGrabbed	) // 3
ax_X11_DefineID(NotifyHint			) // 1	// for MotionNotify events

ax_X11_DefineID(NotifyAncestor			) // 0
ax_X11_DefineID(NotifyVirtual			) // 1
ax_X11_DefineID(NotifyInferior			) // 2
ax_X11_DefineID(NotifyNonlinear			) // 3
ax_X11_DefineID(NotifyNonlinearVirtual	) // 4
ax_X11_DefineID(NotifyPointer			) // 5
ax_X11_DefineID(NotifyPointerRoot		) // 6
ax_X11_DefineID(NotifyDetailNone		) // 7

ax_X11_DefineID(VisibilityUnobscured		) // 0
ax_X11_DefineID(VisibilityPartiallyObscured	) // 1
ax_X11_DefineID(VisibilityFullyObscured		) // 2

ax_X11_DefineID(PlaceOnTop		) // 0
ax_X11_DefineID(PlaceOnBottom	) // 1

ax_X11_DefineID(FamilyInternet		) // 0	/* IPv4 */
ax_X11_DefineID(FamilyDECnet		) // 1
ax_X11_DefineID(FamilyChaos			) // 2
ax_X11_DefineID(FamilyInternet6		) // 6	/* IPv6 */

ax_X11_DefineID(FamilyServerInterpreted) // 5

ax_X11_DefineID(PropertyNewValue	) // 0
ax_X11_DefineID(PropertyDelete		) // 1

ax_X11_DefineID(ColormapUninstalled	) // 0
ax_X11_DefineID(ColormapInstalled	) // 1

ax_X11_DefineID(GrabModeSync	) // 0
ax_X11_DefineID(GrabModeAsync	) // 1

ax_X11_DefineID(GrabSuccess		) // 0
ax_X11_DefineID(AlreadyGrabbed	) // 1
ax_X11_DefineID(GrabInvalidTime	) // 2
ax_X11_DefineID(GrabNotViewable	) // 3
ax_X11_DefineID(GrabFrozen		) // 4

ax_X11_DefineID(AsyncPointer	) // 0
ax_X11_DefineID(SyncPointer		) // 1
ax_X11_DefineID(ReplayPointer	) // 2
ax_X11_DefineID(AsyncKeyboard	) // 3
ax_X11_DefineID(SyncKeyboard	) // 4
ax_X11_DefineID(ReplayKeyboard	) // 5
ax_X11_DefineID(AsyncBoth		) // 6
ax_X11_DefineID(SyncBoth		) // 7

ax_X11_DefineID(RevertToNone		) // (int)None
ax_X11_DefineID(RevertToPointerRoot	) // (int)PointerRoot
ax_X11_DefineID(RevertToParent		) // 2

ax_X11_DefineID(Success			) // 0	/* everything's okay */
ax_X11_DefineID(BadRequest		) // 1	/* bad request code */
ax_X11_DefineID(BadValue	 	) // 2	/* int parameter out of range */
ax_X11_DefineID(BadWindow		) // 3	/* parameter not a Window */
ax_X11_DefineID(BadPixmap		) // 4	/* parameter not a Pixmap */
ax_X11_DefineID(BadAtom			) // 5	/* parameter not an Atom */
ax_X11_DefineID(BadCursor		) // 6	/* parameter not a Cursor */
ax_X11_DefineID(BadFont			) // 7	/* parameter not a Font */
ax_X11_DefineID(BadMatch		) // 8	/* parameter mismatch */
ax_X11_DefineID(BadDrawable		) // 9	/* parameter not a Pixmap or Window */
ax_X11_DefineID(BadAccess		) // 10	/* depending on context:

ax_X11_DefineID(BadAlloc			) // 11	/* insufficient resources */
ax_X11_DefineID(BadColor			) // 12	/* no such colormap */
ax_X11_DefineID(BadGC				) // 13	/* parameter not a GC */
ax_X11_DefineID(BadIDChoice			) // 14	/* choice not in range or already used */
ax_X11_DefineID(BadName				) // 15	/* font or color name doesn't exist */
ax_X11_DefineID(BadLength			) // 16	/* Request length incorrect */
ax_X11_DefineID(BadImplementation	) // 17	/* server is defective */

ax_X11_DefineID(FirstExtensionError	) // 128
ax_X11_DefineID(LastExtensionError	) // 255

ax_X11_DefineID(CWBackPixmap		) // (1L<<0)
ax_X11_DefineID(CWBackPixel			) // (1L<<1)
ax_X11_DefineID(CWBorderPixmap		) // (1L<<2)
ax_X11_DefineID(CWBorderPixel		) // (1L<<3)
ax_X11_DefineID(CWBitGravity		) // (1L<<4)
ax_X11_DefineID(CWWinGravity		) // (1L<<5)
ax_X11_DefineID(CWBackingStore 		) // (1L<<6)
ax_X11_DefineID(CWBackingPlanes	    ) // (1L<<7)
ax_X11_DefineID(CWBackingPixel	    ) // (1L<<8)
ax_X11_DefineID(CWOverrideRedirect	) // (1L<<9)
ax_X11_DefineID(CWSaveUnder			) // (1L<<10)
ax_X11_DefineID(CWEventMask			) // (1L<<11)
ax_X11_DefineID(CWDontPropagate		) // (1L<<12)
ax_X11_DefineID(CWColormap			) // (1L<<13)
ax_X11_DefineID(CWCursor			) // (1L<<14)

ax_X11_DefineID(CWX				) // (1<<0)
ax_X11_DefineID(CWY				) // (1<<1)
ax_X11_DefineID(CWWidth			) // (1<<2)
ax_X11_DefineID(CWHeight		) // (1<<3)
ax_X11_DefineID(CWBorderWidth	) // (1<<4)
ax_X11_DefineID(CWSibling		) // (1<<5)
ax_X11_DefineID(CWStackMode		) // (1<<6)

/* Bit Gravity */
ax_X11_DefineID(ForgetGravity		) // 0
ax_X11_DefineID(NorthWestGravity	) // 1
ax_X11_DefineID(NorthGravity		) // 2
ax_X11_DefineID(NorthEastGravity	) // 3
ax_X11_DefineID(WestGravity			) // 4
ax_X11_DefineID(CenterGravity		) // 5
ax_X11_DefineID(EastGravity			) // 6
ax_X11_DefineID(SouthWestGravity	) // 7
ax_X11_DefineID(SouthGravity		) // 8
ax_X11_DefineID(SouthEastGravity	) // 9
ax_X11_DefineID(StaticGravity		) // 10

/* Window gravity + bit gravity above */

ax_X11_DefineID(UnmapGravity	) // 0

/* Used in CreateWindow for backing-store hint */
ax_X11_DefineID(NotUseful       ) // 0
ax_X11_DefineID(WhenMapped      ) // 1
ax_X11_DefineID(Always          ) // 2

/* Used in GetWindowAttributes reply */

ax_X11_DefineID(IsUnmapped		) // 0
ax_X11_DefineID(IsUnviewable	) // 1
ax_X11_DefineID(IsViewable		) // 2

/* Used in ChangeSaveSet */
ax_X11_DefineID(SetModeInsert   ) // 0
ax_X11_DefineID(SetModeDelete   ) // 1

/* Used in ChangeCloseDownMode */
ax_X11_DefineID(DestroyAll         ) // 0
ax_X11_DefineID(RetainPermanent    ) // 1
ax_X11_DefineID(RetainTemporary    ) // 2

/* Window stacking method (in configureWindow) */
ax_X11_DefineID(Above            ) // 0
ax_X11_DefineID(Below            ) // 1
ax_X11_DefineID(TopIf            ) // 2
ax_X11_DefineID(BottomIf         ) // 3
ax_X11_DefineID(Opposite         ) // 4

/* Circulation direction */
ax_X11_DefineID(RaiseLowest      ) // 0
ax_X11_DefineID(LowerHighest     ) // 1

/* Property modes */
ax_X11_DefineID(PropModeReplace   ) // 0
ax_X11_DefineID(PropModePrepend   ) // 1
ax_X11_DefineID(PropModeAppend    ) // 2

/*****************************************************************
 * GRAPHICS DEFINITIONS
 *****************************************************************/

/* graphics functions, as in GC.alu */
ax_X11_DefineID(GXclear			) // 0x0		/* 0 */
ax_X11_DefineID(GXand			) // 0x1		/* src AND dst */
ax_X11_DefineID(GXandReverse	) // 0x2		/* src AND NOT dst */
ax_X11_DefineID(GXcopy			) // 0x3		/* src */
ax_X11_DefineID(GXandInverted	) // 0x4		/* NOT src AND dst */
ax_X11_DefineID(GXnoop			) // 0x5		/* dst */
ax_X11_DefineID(GXxor			) // 0x6		/* src XOR dst */
ax_X11_DefineID(GXor			) // 0x7		/* src OR dst */
ax_X11_DefineID(GXnor			) // 0x8		/* NOT src AND NOT dst */
ax_X11_DefineID(GXequiv			) // 0x9		/* NOT src XOR dst */
ax_X11_DefineID(GXinvert		) // 0xa		/* NOT dst */
ax_X11_DefineID(GXorReverse		) // 0xb		/* src OR NOT dst */
ax_X11_DefineID(GXcopyInverted	) // 0xc		/* NOT src */
ax_X11_DefineID(GXorInverted	) // 0xd		/* NOT src OR dst */
ax_X11_DefineID(GXnand			) // 0xe		/* NOT src OR NOT dst */
ax_X11_DefineID(GXset			) // 0xf		/* 1 */

/* LineStyle */
ax_X11_DefineID(LineSolid		) // 0
ax_X11_DefineID(LineOnOffDash	) // 1
ax_X11_DefineID(LineDoubleDash	) // 2

/* capStyle */
ax_X11_DefineID(CapNotLast		) // 0
ax_X11_DefineID(CapButt			) // 1
ax_X11_DefineID(CapRound		) // 2
ax_X11_DefineID(CapProjecting	) // 3

/* joinStyle */
ax_X11_DefineID(JoinMiter	) // 0
ax_X11_DefineID(JoinRound	) // 1
ax_X11_DefineID(JoinBevel	) // 2

/* fillStyle */
ax_X11_DefineID(FillSolid			) // 0
ax_X11_DefineID(FillTiled			) // 1
ax_X11_DefineID(FillStippled		) // 2
ax_X11_DefineID(FillOpaqueStippled	) // 3

/* fillRule */
ax_X11_DefineID(EvenOddRule		) // 0
ax_X11_DefineID(WindingRule		) // 1

/* subwindow mode */
ax_X11_DefineID(ClipByChildren		) // 0
ax_X11_DefineID(IncludeInferiors	) // 1

/* SetClipRectangles ordering */
ax_X11_DefineID(Unsorted	) // 0
ax_X11_DefineID(YSorted		) // 1
ax_X11_DefineID(YXSorted	) // 2
ax_X11_DefineID(YXBanded	) // 3

/* CoordinateMode for drawing routines */
ax_X11_DefineID(CoordModeOrigin		) // 0	/* relative to the origin */
ax_X11_DefineID(CoordModePrevious	) // 1	/* relative to previous point */

/* Polygon shapes */
ax_X11_DefineID(Complex		) // 0	/* paths may intersect */
ax_X11_DefineID(Nonconvex	) // 1	/* no paths intersect, but not convex */
ax_X11_DefineID(Convex		) // 2	/* wholly convex */

/* Arc modes for PolyFillArc */
ax_X11_DefineID(ArcChord		) // 0	/* join endpoints of arc */
ax_X11_DefineID(ArcPieSlice		) // 1	/* join endpoints to center of arc */

/* GC components: masks used in CreateGC, CopyGC, ChangeGC, OR'ed into
   GC.stateChanges */

ax_X11_DefineID(GCFunction        		) // (1L<<0)
ax_X11_DefineID(GCPlaneMask       		) // (1L<<1)
ax_X11_DefineID(GCForeground      		) // (1L<<2)
ax_X11_DefineID(GCBackground      		) // (1L<<3)
ax_X11_DefineID(GCLineWidth       		) // (1L<<4)
ax_X11_DefineID(GCLineStyle       		) // (1L<<5)
ax_X11_DefineID(GCCapStyle        		) // (1L<<6)
ax_X11_DefineID(GCJoinStyle				) // (1L<<7)
ax_X11_DefineID(GCFillStyle				) // (1L<<8)
ax_X11_DefineID(GCFillRule				) // (1L<<9) 
ax_X11_DefineID(GCTile					) // (1L<<10)
ax_X11_DefineID(GCStipple				) // (1L<<11)
ax_X11_DefineID(GCTileStipXOrigin		) // (1L<<12)
ax_X11_DefineID(GCTileStipYOrigin		) // (1L<<13)
ax_X11_DefineID(GCFont 					) // (1L<<14)
ax_X11_DefineID(GCSubwindowMode			) // (1L<<15)
ax_X11_DefineID(GCGraphicsExposures		) // (1L<<16)
ax_X11_DefineID(GCClipXOrigin			) // (1L<<17)
ax_X11_DefineID(GCClipYOrigin			) // (1L<<18)
ax_X11_DefineID(GCClipMask				) // (1L<<19)
ax_X11_DefineID(GCDashOffset			) // (1L<<20)
ax_X11_DefineID(GCDashList				) // (1L<<21)
ax_X11_DefineID(GCArcMode				) // (1L<<22)
ax_X11_DefineID(GCLastBit				) // 22

/*****************************************************************
 * FONTS 
 *****************************************************************/

/* used in QueryFont -- draw direction */
ax_X11_DefineID(FontLeftToRight		) // 0
ax_X11_DefineID(FontRightToLeft		) // 1
ax_X11_DefineID(FontChange			) // 255

/*****************************************************************
 *  IMAGING 
 *****************************************************************/

/* ImageFormat -- PutImage, GetImage */
ax_X11_DefineID(XYBitmap	) // 0	/* depth 1, XYFormat */
ax_X11_DefineID(XYPixmap	) // 1	/* depth == drawable depth */
ax_X11_DefineID(ZPixmap		) // 2	/* depth == drawable depth */

/*****************************************************************
 *  COLOR MAP STUFF 
 *****************************************************************/

/* For CreateColormap */
ax_X11_DefineID(AllocNone	) // 0	/* create map with no entries */
ax_X11_DefineID(AllocAll	) // 1	/* allocate entire map writeable */


/* Flags used in StoreNamedColor, StoreColors */
ax_X11_DefineID(DoRed		) // (1<<0)
ax_X11_DefineID(DoGreen		) // (1<<1)
ax_X11_DefineID(DoBlue		) // (1<<2)

/*****************************************************************
 * CURSOR STUFF
 *****************************************************************/

/* QueryBestSize Class */
ax_X11_DefineID(CursorShape		) // 0	/* largest size that can be displayed */
ax_X11_DefineID(TileShape		) // 1	/* size tiled fastest */
ax_X11_DefineID(StippleShape	) // 2	/* size stippled fastest */

/***************************************************************** 
 * KEYBOARD/POINTER STUFF
 *****************************************************************/
ax_X11_DefineID(AutoRepeatModeOff		) // 0
ax_X11_DefineID(AutoRepeatModeOn		) // 1
ax_X11_DefineID(AutoRepeatModeDefault	) // 2

ax_X11_DefineID(LedModeOff		) // 0
ax_X11_DefineID(LedModeOn		) // 1

/* masks for ChangeKeyboardControl */
ax_X11_DefineID(KBKeyClickPercent	) // (1L<<0)
ax_X11_DefineID(KBBellPercent		) // (1L<<1)
ax_X11_DefineID(KBBellPitch			) // (1L<<2)
ax_X11_DefineID(KBBellDuration		) // (1L<<3)
ax_X11_DefineID(KBLed				) // (1L<<4)
ax_X11_DefineID(KBLedMode			) // (1L<<5)
ax_X11_DefineID(KBKey				) // (1L<<6)
ax_X11_DefineID(KBAutoRepeatMode	) // (1L<<7)

ax_X11_DefineID(MappingSuccess     	) // 0
ax_X11_DefineID(MappingBusy        	) // 1
ax_X11_DefineID(MappingFailed		) // 2

ax_X11_DefineID(MappingModifier		) // 0
ax_X11_DefineID(MappingKeyboard		) // 1
ax_X11_DefineID(MappingPointer		) // 2

/*****************************************************************
 * SCREEN SAVER STUFF 
 *****************************************************************/
ax_X11_DefineID(DontPreferBlanking	) // 0
ax_X11_DefineID(PreferBlanking		) // 1
ax_X11_DefineID(DefaultBlanking		) // 2

ax_X11_DefineID(DisableScreenSaver		) // 0
ax_X11_DefineID(DisableScreenInterval	) // 0

ax_X11_DefineID(DontAllowExposures	) // 0
ax_X11_DefineID(AllowExposures		) // 1
ax_X11_DefineID(DefaultExposures	) // 2

/* for ForceScreenSaver */
ax_X11_DefineID(ScreenSaverReset 	) // 0
ax_X11_DefineID(ScreenSaverActive 	) // 1

/*****************************************************************
 * HOSTS AND CONNECTIONS
 *****************************************************************/

/* for ChangeHosts */
ax_X11_DefineID(HostInsert		) // 0
ax_X11_DefineID(HostDelete		) // 1

/* for ChangeAccessControl */
ax_X11_DefineID(EnableAccess		) // 1
ax_X11_DefineID(DisableAccess		) // 0

/* Display classes  used in opening the connection 
 * Note that the statically allocated ones are even numbered and the
 * dynamically changeable ones are odd numbered */
ax_X11_DefineID(StaticGray		) // 0
ax_X11_DefineID(GrayScale		) // 1
ax_X11_DefineID(StaticColor		) // 2
ax_X11_DefineID(PseudoColor		) // 3
ax_X11_DefineID(TrueColor		) // 4
ax_X11_DefineID(DirectColor		) // 5

/* Byte order  used in imageByteOrder and bitmapBitOrder */
ax_X11_DefineID(LSBFirst		) // 0
ax_X11_DefineID(MSBFirst		) // 1

#endif