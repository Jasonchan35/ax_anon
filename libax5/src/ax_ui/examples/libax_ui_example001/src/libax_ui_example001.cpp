#include <ax/ax_ui.h>


class MyPanel : public axUIDockPanel {
	AX_RTTI_CLASS(MyPanel, axUIDockPanel);
public:
	MyPanel(axUIDockZone* zone) 
		: Base(zone) 
		, _testButton(this, "Test Button")
	{
		_testButton.layout.setAlign(Align::s_center());


		test_vertex< axRenderVertex_Unknown >();
		test_vertex< axRenderVertex_PosColor>();
		test_vertex< axRenderVertex_PosUvColor>();
		test_vertex< axRenderVertex_PosUvColorNormal>();
		test_vertex< axRenderVertexBase_PosUvColor<axFloat32x3, axUInt8, 1, axFloat32x2, 2> >();

		AX_LOG("test");

	}

	template<class Vertex>
	void test_vertex() {
		auto desc = Vertex::s_desc();
		AX_DUMP_VAR(desc);
		AX_LOG("{?:x}", ax_enum_int(desc->vertexType));
		AX_LOG("\n");
	}

	virtual void onRender(axRenderRequest& req) override {
		req.ui.drawRect(AX_LOC, Rect2(10, 10, 320, 200), Color(.5f, .5f, .8f));
	}
	
	virtual void onMouseEvent(axUIMouseEvent& ev) override {
		Base::onMouseEvent(ev);
		if (ev.type == axUIMouseEventType::Down) {
			setNeedToLayout();
			setNeedToRender();
		}
	}

	axUIButton _testButton;
};

class MainWindow : public axUIDockWindow {
	AX_RTTI_CLASS(MainWindow, axUIDockWindow);
public:
	CreateDesc& _createDesc() {
		static CreateDesc desc;
		desc.isMainWindow = true;
		desc.visible = false;

		// desc.resizable = false;
		// desc.minButton = false;
		// desc.maxButton = false;
		// desc.closeButton = false;
		return desc;
	}

	MainWindow();
};

class axUI_Example001 : public axUIDockApp {	
	AX_RTTI_CLASS(axUI_Example001, axUIDockApp)
public:
	ax_DOWNCAST_GET_INSTANCE()

	axUI_Example001() {
//		setRenderApi(axRenderApi::Vulkan, false);
		setRenderApi(axRenderApi::OpenGL, false);
	}

	virtual void onCreate() override {
		Base::onCreate();

		renderer()->setVSync(false);

		auto dir = axPath::dirname(currentExecuteFilename());
	
		#if AX_OS_OSX
			axString tmp;
			tmp.format("{?}/../../../", dir);
			axPath::setCurrentDir(tmp);
		#else
			axPath::setCurrentDir(dir);
		#endif

		axString resourcesDir;
	#if AX_OS_IOS
		resourcesDir = axPath::appResourcesDir();
	#else
		resourcesDir = "../../../..";
	#endif

		_assetsDir.append(resourcesDir, "/assets");
		axRenderer::s_instance()->builtInAssets()->load(_assetsDir);

		axPath::setCurrentDir(_assetsDir);

	//===========

	defaultAssets()->load(this, "./");


		_mainWin = ax_new MainWindow;
		_mainWin->setSize(800, 600);
		_mainWin->updateLayout();
		_mainWin->setVisible(true);
	}

	axLink<MainWindow> _mainWin;
	axString _assetsDir;
};

//=================================================

MainWindow::MainWindow() 
	: Base(axUIApp::s_instance()->rootView(), _createDesc())
{
	axTempString title("axUI_Example001");

	auto* app = axUIApp::s_instance();
	title.appendFormat(" - {?}, VSync: {?}", app->renderer()->api(), app->renderer()->vsync());
	setWindowTitle(title);
	setWindowActive(true);
	
	ax_new MyPanel(dockZone());
}

axApp_main(axUI_Example001)

