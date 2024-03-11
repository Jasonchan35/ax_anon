#pragma once

#if AX_USE_X11 || AX_USE_Gtk

//============ defines in X.h ==================================================================

#undef ParentRelative      // 1L	// background pixmap in CreateWindow
								    // and ChangeWindowAttributes
									
#undef CopyFromParent      // 0L	// border pixmap in CreateWindow
									// and ChangeWindowAttributes
									// special VisualID and special window
									// class passed to CreateWindow

#undef PointerWindow       // 0L	// destination window in SendEvent
#undef InputFocus          // 1L	// destination window in SendEvent
#undef PointerRoot         // 1L	// focus window in SetInputFocus
#undef AnyPropertyType     // 0L	// special Atom, passed to GetProperty
#undef AnyKey		       // 0L	// special Key Code, passed to GrabKey
#undef AnyButton           // 0L	// special Button Code, passed to GrabButton
#undef AllTemporary        // 0L	// special Resource ID passed to KillClient
#undef CurrentTime         // 0L	// special Time
#undef NoSymbol	           // 0L	// special KeySym


#undef NoEventMask					// 0L
#undef KeyPressMask					// (1L<<0)  
#undef KeyReleaseMask				// (1L<<1)  
#undef ButtonPressMask				// (1L<<2)  
#undef ButtonReleaseMask			// (1L<<3)  
#undef EnterWindowMask				// (1L<<4)  
#undef LeaveWindowMask				// (1L<<5)  
#undef PointerMotionMask			// (1L<<6)  
#undef PointerMotionHintMask		// (1L<<7)  
#undef Button1MotionMask			// (1L<<8)  
#undef Button2MotionMask			// (1L<<9)  
#undef Button3MotionMask			// (1L<<10) 
#undef Button4MotionMask			// (1L<<11) 
#undef Button5MotionMask			// (1L<<12) 
#undef ButtonMotionMask				// (1L<<13) 
#undef KeymapStateMask				// (1L<<14)
#undef ExposureMask					// (1L<<15) 
#undef VisibilityChangeMask			// (1L<<16) 
#undef StructureNotifyMask			// (1L<<17) 
#undef ResizeRedirectMask			// (1L<<18) 
#undef SubstructureNotifyMask		// (1L<<19) 
#undef SubstructureRedirectMask		// (1L<<20) 
#undef FocusChangeMask				// (1L<<21) 
#undef PropertyChangeMask			// (1L<<22) 
#undef ColormapChangeMask			// (1L<<23) 
#undef OwnerGrabButtonMask			// (1L<<24) 


#undef KeyPress				// 2
#undef KeyRelease			// 3
#undef ButtonPress			// 4
#undef ButtonRelease		// 5
#undef MotionNotify			// 6
#undef EnterNotify			// 7
#undef LeaveNotify			// 8
#undef FocusIn				// 9
#undef FocusOut				// 10
#undef KeymapNotify			// 11
#undef Expose				// 12
#undef GraphicsExpose		// 13
#undef NoExpose				// 14
#undef VisibilityNotify		// 15
#undef CreateNotify			// 16
#undef DestroyNotify		// 17
#undef UnmapNotify			// 18
#undef MapNotify			// 19
#undef MapRequest			// 20
#undef ReparentNotify		// 21
#undef ConfigureNotify		// 22
#undef ConfigureRequest		// 23
#undef GravityNotify		// 24
#undef ResizeRequest		// 25
#undef CirculateNotify		// 26
#undef CirculateRequest		// 27
#undef PropertyNotify		// 28
#undef SelectionClear		// 29
#undef SelectionRequest		// 30
#undef SelectionNotify		// 31
#undef ColormapNotify		// 32
#undef ClientMessage		// 33
#undef MappingNotify		// 34
#undef GenericEvent			// 35
#undef LASTEvent			// 36	// must be bigger than any event #


#undef ShiftMask		// (1<<0)
#undef LockMask			// (1<<1)
#undef ControlMask		// (1<<2)
#undef Mod1Mask			// (1<<3)
#undef Mod2Mask			// (1<<4)
#undef Mod3Mask			// (1<<5)
#undef Mod4Mask			// (1<<6)
#undef Mod5Mask			// (1<<7)


#undef ShiftMapIndex	// 0
#undef LockMapIndex		// 1
#undef ControlMapIndex	// 2
#undef Mod1MapIndex		// 3
#undef Mod2MapIndex		// 4
#undef Mod3MapIndex		// 5
#undef Mod4MapIndex		// 6
#undef Mod5MapIndex		// 7


#undef Button1Mask		// (1<<8)
#undef Button2Mask		// (1<<9)
#undef Button3Mask		// (1<<10)
#undef Button4Mask		// (1<<11)
#undef Button5Mask		// (1<<12)
#undef AnyModifier		// (1<<15)  /* used in GrabButton, GrabKey */


#undef Button1			// 1
#undef Button2			// 2
#undef Button3			// 3
#undef Button4			// 4
#undef Button5			// 5


#undef NotifyNormal			// 0
#undef NotifyGrab			// 1
#undef NotifyUngrab			// 2
#undef NotifyWhileGrabbed	// 3
#undef NotifyHint			// 1	// for MotionNotify events


#undef NotifyAncestor			// 0
#undef NotifyVirtual			// 1
#undef NotifyInferior			// 2
#undef NotifyNonlinear			// 3
#undef NotifyNonlinearVirtual	// 4
#undef NotifyPointer			// 5
#undef NotifyPointerRoot		// 6
#undef NotifyDetailNone			// 7


#undef VisibilityUnobscured				// 0
#undef VisibilityPartiallyObscured		// 1
#undef VisibilityFullyObscured			// 2

#undef PlaceOnTop		// 0
#undef PlaceOnBottom	// 1

#undef FamilyInternet		// 0	/* IPv4 */
#undef FamilyDECnet			// 1
#undef FamilyChaos			// 2
#undef FamilyInternet6		// 6	/* IPv6 */

#undef FamilyServerInterpreted // 5

#undef PropertyNewValue		// 0
#undef PropertyDelete		// 1

#undef ColormapUninstalled	// 0
#undef ColormapInstalled	// 1

#undef GrabModeSync			// 0
#undef GrabModeAsync		// 1

#undef GrabSuccess			// 0
#undef AlreadyGrabbed		// 1
#undef GrabInvalidTime		// 2
#undef GrabNotViewable		// 3
#undef GrabFrozen			// 4

#undef AsyncPointer		// 0
#undef SyncPointer		// 1
#undef ReplayPointer	// 2
#undef AsyncKeyboard	// 3
#undef SyncKeyboard		// 4
#undef ReplayKeyboard	// 5
#undef AsyncBoth		// 6
#undef SyncBoth			// 7

#undef RevertToNone			// (int)None
#undef RevertToPointerRoot	// (int)PointerRoot
#undef RevertToParent		// 2


#undef Success		// 0	/* everything's okay */
#undef BadRequest	// 1	/* bad request code */
#undef BadValue	 	// 2	/* int parameter out of range */
#undef BadWindow	// 3	/* parameter not a Window */
#undef BadPixmap	// 4	/* parameter not a Pixmap */
#undef BadAtom		// 5	/* parameter not an Atom */
#undef BadCursor	// 6	/* parameter not a Cursor */
#undef BadFont		// 7	/* parameter not a Font */
#undef BadMatch		// 8	/* parameter mismatch */
#undef BadDrawable	// 9	/* parameter not a Pixmap or Window */
#undef BadAccess	// 10	/* depending on context:

#undef BadAlloc				// 11	/* insufficient resources */
#undef BadColor				// 12	/* no such colormap */
#undef BadGC				// 13	/* parameter not a GC */
#undef BadIDChoice			// 14	/* choice not in range or already used */
#undef BadName				// 15	/* font or color name doesn't exist */
#undef BadLength			// 16	/* Request length incorrect */
#undef BadImplementation	// 17	/* server is defective */

#undef FirstExtensionError	// 128
#undef LastExtensionError	// 255

#undef CWBackPixmap			// (1L<<0)
#undef CWBackPixel			// (1L<<1)
#undef CWBorderPixmap		// (1L<<2)
#undef CWBorderPixel		// (1L<<3)
#undef CWBitGravity			// (1L<<4)
#undef CWWinGravity			// (1L<<5)
#undef CWBackingStore 		// (1L<<6)
#undef CWBackingPlanes	    // (1L<<7)
#undef CWBackingPixel	    // (1L<<8)
#undef CWOverrideRedirect	// (1L<<9)
#undef CWSaveUnder			// (1L<<10)
#undef CWEventMask			// (1L<<11)
#undef CWDontPropagate		// (1L<<12)
#undef CWColormap			// (1L<<13)
#undef CWCursor				// (1L<<14)

#undef CWX				// (1<<0)
#undef CWY				// (1<<1)
#undef CWWidth			// (1<<2)
#undef CWHeight			// (1<<3)
#undef CWBorderWidth	// (1<<4)
#undef CWSibling		// (1<<5)
#undef CWStackMode		// (1<<6)

/* Bit Gravity */
#undef ForgetGravity		// 0
#undef NorthWestGravity		// 1
#undef NorthGravity			// 2
#undef NorthEastGravity		// 3
#undef WestGravity			// 4
#undef CenterGravity		// 5
#undef EastGravity			// 6
#undef SouthWestGravity		// 7
#undef SouthGravity			// 8
#undef SouthEastGravity		// 9
#undef StaticGravity		// 10

/* Window gravity + bit gravity above */

#undef UnmapGravity		// 0

/* Used in CreateWindow for backing-store hint */
#undef NotUseful         // 0
#undef WhenMapped        // 1
#undef Always            // 2

/* Used in GetWindowAttributes reply */

#undef IsUnmapped		// 0
#undef IsUnviewable		// 1
#undef IsViewable		// 2

/* Used in ChangeSaveSet */
#undef SetModeInsert           // 0
#undef SetModeDelete           // 1

/* Used in ChangeCloseDownMode */
#undef DestroyAll              // 0
#undef RetainPermanent         // 1
#undef RetainTemporary         // 2

/* Window stacking method (in configureWindow) */
#undef Above                   // 0
#undef Below                   // 1
#undef TopIf                   // 2
#undef BottomIf                // 3
#undef Opposite                // 4

/* Circulation direction */
#undef RaiseLowest             // 0
#undef LowerHighest            // 1

/* Property modes */
#undef PropModeReplace         // 0
#undef PropModePrepend         // 1
#undef PropModeAppend          // 2

/*****************************************************************
 * GRAPHICS DEFINITIONS
 *****************************************************************/

/* graphics functions, as in GC.alu */
#undef GXclear			// 0x0		/* 0 */
#undef GXand			// 0x1		/* src AND dst */
#undef GXandReverse		// 0x2		/* src AND NOT dst */
#undef GXcopy			// 0x3		/* src */
#undef GXandInverted	// 0x4		/* NOT src AND dst */
#undef GXnoop			// 0x5		/* dst */
#undef GXxor			// 0x6		/* src XOR dst */
#undef GXor				// 0x7		/* src OR dst */
#undef GXnor			// 0x8		/* NOT src AND NOT dst */
#undef GXequiv			// 0x9		/* NOT src XOR dst */
#undef GXinvert			// 0xa		/* NOT dst */
#undef GXorReverse		// 0xb		/* src OR NOT dst */
#undef GXcopyInverted	// 0xc		/* NOT src */
#undef GXorInverted		// 0xd		/* NOT src OR dst */
#undef GXnand			// 0xe		/* NOT src OR NOT dst */
#undef GXset			// 0xf		/* 1 */

/* LineStyle */
#undef LineSolid		// 0
#undef LineOnOffDash	// 1
#undef LineDoubleDash	// 2

/* capStyle */
#undef CapNotLast		// 0
#undef CapButt			// 1
#undef CapRound			// 2
#undef CapProjecting	// 3

/* joinStyle */
#undef JoinMiter		// 0
#undef JoinRound		// 1
#undef JoinBevel		// 2

/* fillStyle */
#undef FillSolid			// 0
#undef FillTiled			// 1
#undef FillStippled			// 2
#undef FillOpaqueStippled	// 3

/* fillRule */
#undef EvenOddRule		// 0
#undef WindingRule		// 1

/* subwindow mode */
#undef ClipByChildren		// 0
#undef IncludeInferiors		// 1

/* SetClipRectangles ordering */
#undef Unsorted		// 0
#undef YSorted		// 1
#undef YXSorted		// 2
#undef YXBanded		// 3

/* CoordinateMode for drawing routines */
#undef CoordModeOrigin		// 0	/* relative to the origin */
#undef CoordModePrevious 	// 1	/* relative to previous point */

/* Polygon shapes */
#undef Complex			// 0	/* paths may intersect */
#undef Nonconvex		// 1	/* no paths intersect, but not convex */
#undef Convex			// 2	/* wholly convex */

/* Arc modes for PolyFillArc */
#undef ArcChord			// 0	/* join endpoints of arc */
#undef ArcPieSlice		// 1	/* join endpoints to center of arc */

/* GC components: masks used in CreateGC, CopyGC, ChangeGC, OR'ed into
   GC.stateChanges */

#undef GCFunction        		// (1L<<0)
#undef GCPlaneMask       		// (1L<<1)
#undef GCForeground      		// (1L<<2)
#undef GCBackground      		// (1L<<3)
#undef GCLineWidth       		// (1L<<4)
#undef GCLineStyle       		// (1L<<5)
#undef GCCapStyle        		// (1L<<6)
#undef GCJoinStyle				// (1L<<7)
#undef GCFillStyle				// (1L<<8)
#undef GCFillRule				// (1L<<9) 
#undef GCTile					// (1L<<10)
#undef GCStipple				// (1L<<11)
#undef GCTileStipXOrigin		// (1L<<12)
#undef GCTileStipYOrigin		// (1L<<13)
#undef GCFont 					// (1L<<14)
#undef GCSubwindowMode			// (1L<<15)
#undef GCGraphicsExposures		// (1L<<16)
#undef GCClipXOrigin			// (1L<<17)
#undef GCClipYOrigin			// (1L<<18)
#undef GCClipMask				// (1L<<19)
#undef GCDashOffset				// (1L<<20)
#undef GCDashList				// (1L<<21)
#undef GCArcMode				// (1L<<22)
#undef GCLastBit				// 22
/*****************************************************************
 * FONTS 
 *****************************************************************/

/* used in QueryFont -- draw direction */
#undef FontLeftToRight		// 0
#undef FontRightToLeft		// 1
#undef FontChange			// 255

/*****************************************************************
 *  IMAGING 
 *****************************************************************/

/* ImageFormat -- PutImage, GetImage */
#undef XYBitmap		// 0	/* depth 1, XYFormat */
#undef XYPixmap		// 1	/* depth == drawable depth */
#undef ZPixmap		// 2	/* depth == drawable depth */

/*****************************************************************
 *  COLOR MAP STUFF 
 *****************************************************************/

/* For CreateColormap */
#undef AllocNone		// 0	/* create map with no entries */
#undef AllocAll			// 1	/* allocate entire map writeable */


/* Flags used in StoreNamedColor, StoreColors */
#undef DoRed			// (1<<0)
#undef DoGreen			// (1<<1)
#undef DoBlue			// (1<<2)

/*****************************************************************
 * CURSOR STUFF
 *****************************************************************/

/* QueryBestSize Class */
#undef CursorShape		// 0	/* largest size that can be displayed */
#undef TileShape		// 1	/* size tiled fastest */
#undef StippleShape		// 2	/* size stippled fastest */

/***************************************************************** 
 * KEYBOARD/POINTER STUFF
 *****************************************************************/
#undef AutoRepeatModeOff		// 0
#undef AutoRepeatModeOn			// 1
#undef AutoRepeatModeDefault	// 2

#undef LedModeOff		// 0
#undef LedModeOn		// 1

/* masks for ChangeKeyboardControl */
#undef KBKeyClickPercent	// (1L<<0)
#undef KBBellPercent		// (1L<<1)
#undef KBBellPitch			// (1L<<2)
#undef KBBellDuration		// (1L<<3)
#undef KBLed				// (1L<<4)
#undef KBLedMode			// (1L<<5)
#undef KBKey				// (1L<<6)
#undef KBAutoRepeatMode		// (1L<<7)

#undef MappingSuccess     	// 0
#undef MappingBusy        	// 1
#undef MappingFailed		// 2

#undef MappingModifier		// 0
#undef MappingKeyboard		// 1
#undef MappingPointer		// 2

/*****************************************************************
 * SCREEN SAVER STUFF 
 *****************************************************************/
#undef DontPreferBlanking	// 0
#undef PreferBlanking		// 1
#undef DefaultBlanking		// 2

#undef DisableScreenSaver		// 0
#undef DisableScreenInterval	// 0

#undef DontAllowExposures	// 0
#undef AllowExposures		// 1
#undef DefaultExposures		// 2

/* for ForceScreenSaver */
#undef ScreenSaverReset 	// 0
#undef ScreenSaverActive	// 1

/*****************************************************************
 * HOSTS AND CONNECTIONS
 *****************************************************************/

/* for ChangeHosts */
#undef HostInsert		// 0
#undef HostDelete		// 1

/* for ChangeAccessControl */
#undef EnableAccess		// 1
#undef DisableAccess	// 0

/* Display classes  used in opening the connection 
 * Note that the statically allocated ones are even numbered and the
 * dynamically changeable ones are odd numbered */
#undef StaticGray		// 0
#undef GrayScale		// 1
#undef StaticColor		// 2
#undef PseudoColor		// 3
#undef TrueColor		// 4
#undef DirectColor		// 5


/* Byte order  used in imageByteOrder and bitmapBitOrder */
#undef LSBFirst		// 0
#undef MSBFirst		// 1

// =========== defines in Xlib.h ============================================
const Bool ax_X11_True  = True;
const Bool ax_X11_False = False;

#undef Bool   // int
#undef Status // int
#undef True   // 1
#undef False  // 0

#undef QueuedAlready      // 0
#undef QueuedAfterReading // 1
#undef QueuedAfterFlush   // 2

//===========================================================================


#undef NoValue			// 0x0000

#define ax_X11_None 0L
#ifdef None
	#undef None
#endif

#endif