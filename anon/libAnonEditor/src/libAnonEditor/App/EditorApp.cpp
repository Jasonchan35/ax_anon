#include "EditorApp.h"
#include "EditorMainWindow.h"
#include "../Panels/AssetsPanel/AssetsPanel.h"
#include "../Panels/InspectorPanel/InspectorPanel.h"
#include "../Panels/SceneOutlinerPanel/SceneOutlinerPanel.h"
#include "../Panels/SceneViewPanel/SceneViewPanel.h"
#include "../Panels/ConsolePanel/ConsolePanel.h"
#include "../Panels/DiagramPanel/DiagramPanel.h"

namespace Anon{ namespace Editor {

EditorApp::EditorApp() {

	//AX_DUMP_VAR(ax_sizeof(axIString));
	//AX_DUMP_VAR(ax_sizeof(axString));
	//AX_DUMP_VAR(ax_sizeof(axTempString));
	//AX_DUMP_VAR(ax_sizeof(axRenderVertex_PosUvColor));
	//AX_DUMP_VAR(ax_sizeof(axTypedBase));
	//AX_DUMP_VAR(ax_sizeof(axLinkable));
	//AX_DUMP_VAR(ax_sizeof(axRefCounted));
	//AX_DUMP_VAR(ax_sizeof(axObject));
	//using MapNode = axMapNode<AnonObjectId, AnonObject>;
	//AX_DUMP_VAR(ax_sizeof(MapNode));
	//AX_DUMP_VAR(ax_sizeof(AnonObject));
	//AX_DUMP_VAR(ax_sizeof(axLink<axUIDiaNodeBase>));
	//AX_DUMP_VAR(ax_sizeof(DiaNode));
	//AX_DUMP_VAR(ax_sizeof(DiaConnectionNode));
	//AX_DUMP_VAR(ax_sizeof(DiaConnection));
	//AX_DUMP_VAR(ax_sizeof(GEO_GeoData));
	//AX_DUMP_VAR(ax_sizeof(GEO_AttrArrayBase));
	//AX_DUMP_VAR(ax_sizeof(GEO_PointComponent));
	//AX_DUMP_VAR(ax_sizeof(GEO_VertexComponent));
	//AX_DUMP_VAR(ax_sizeof(GEO_PrimitiveComponent));
	//AX_DUMP_VAR(ax_sizeof(axArray<GEO_Primitive>));
	//AX_DUMP_VAR(ax_sizeof(axNameId));
	//AX_DUMP_VAR(ax_sizeof(GEO_Node));
	//AX_DUMP_VAR(ax_sizeof(SCN_Object));
	//AX_DUMP_VAR(ax_sizeof(SCN_Component));

//	axProfiler::s_instance()->enableDebugPort();

#if AX_OS_WINDOWS
//	setRenderApi(axRenderApi::DX11, false);
//	setRenderApi(axRenderApi::DX12, false);
//	setRenderApi(axRenderApi::OpenGL, false);
//	setRenderApi(axRenderApi::Vulkan, false);

	if (::GetAsyncKeyState(VK_CONTROL) && ::GetAsyncKeyState(VK_SHIFT)) {
		setRenderApi(axRenderApi::Vulkan, false);
	} else if (::GetAsyncKeyState(VK_CONTROL)) {
		setRenderApi(axRenderApi::DX12, false);
	} else if (::GetAsyncKeyState(VK_SHIFT)) {
		setRenderApi(axRenderApi::OpenGL, false);
	}


#elif AX_OS_LINUX
	setRenderApi(axRenderApi::OpenGL, false);
//	setRenderApi(axRenderApi::Vulkan, false);

#elif AX_OS_OSX
	setRenderApi(axRenderApi::Metal,  false);
//	setRenderApi(axRenderApi::Vulkan, false);
//	setRenderApi(axRenderApi::OpenGL, false);

#endif
}

EditorApp::~EditorApp() {
}

void EditorApp::onCreate() {
	Base::onCreate();

	renderer()->setVSync(false);
	modules.create();

	auto dir = axPath::dirname(currentExecuteFilename());
	
	#if AX_OS_OSX
		axString tmp;
		tmp.format("{?}/../../../", dir);
		axPath::setCurrentDir(tmp);
	#else
		axPath::setCurrentDir(dir);
	#endif

	axString resourcesDir;
#if AX_OS_WINDOWS
	resourcesDir = "../../..";
#elif AX_OS_IOS
	resourcesDir = axPath::appResourcesDir();
#else
	resourcesDir = "../..";
#endif

	_assetsDir.append(resourcesDir, "/assets");
	axRenderer::s_instance()->builtInAssets()->load(_assetsDir);

	axPath::setCurrentDir(_assetsDir);

	//===========

	defaultAssets()->load(this, "./");

	//-------
	auto* anon =  AnonContext::s_instance();
	{
		AnonContext::CreateDesc desc;
		desc.inEditor = true;
		anon->evRequestCustomAppEvent.bind(this, [this](){ requestCustomAppEvent(); });
		anon->create(desc);
	}


	_mainWin.ref(ax_new EditorMainWindow);
//	_mainWin->setSize(1600, 800);
	_mainWin->setSize(1920, 1080);

	// -- left --
	auto* leftZone   = _mainWin->dockZone();
	ax_new SceneViewPanel(leftZone);

#if 1 // other panels
	auto* rightZone  = leftZone->split(400, false);
	auto* middleZone = leftZone->split(250, false);

	ax_new ConsolePanel(leftZone->split(120, true));
	auto* diagramPanel = ax_new DiagramPanel(leftZone->split(400, true));
	AX_UNUSED(diagramPanel);

	// -- middle --
	auto* outliner = ax_new SceneOutlinerPanel(middleZone);
	AX_UNUSED(outliner);

	auto* assetsPanel = ax_new AssetsPanel(middleZone->split(500, true));

	// -- right --
	ax_new InspectorPanel(rightZone);



	_assetDatabase.ref(ax_new AssetDatabase());
	_assetDatabase->create("./");

	AX_UNUSED(assetsPanel);
//	assetsPanel->reload();

	{
		auto* scn = SCN_System::s_instance();
		scn->loadSceneFile("scenes/test002.AnonScene");
		if (auto* cube = scn->findObject("Group1/Geometry")) {
			if (auto* geo = cube->findComponent<SCN_Geometry>()) {
				auto* dia = geo->diagram();
				openDiagramPanel(dia);
			}
		}
	}

	#if 0
	{
		auto* testing = ax_new SCN_Object(nullptr);
		testing->setName("Testing");

		auto* model = ax_new SCN_Object(nullptr);
		{
			model->setName("Model");
			auto geoAsset = axSPtr_new(GeoAsset);
			//auto* shape = ax_new SCN_Shape(model);
			//shape->setAsset(geoAsset);

			auto* dia = geoAsset->diagram();
			#if 1
			{
				// create nodes
				{
					auto* cube  = ax_new GEO_Cube(dia);
					auto* tran0 = ax_new GEO_Transform(dia);
					auto* tran1 = ax_new GEO_Transform(dia);
					auto* tran2 = ax_new GEO_Transform(dia);
					cube->output.connect(tran0->input);

					tran0->output.connect(tran1->input);
					tran1->output.connect(tran2->input);
				}

				for (axInt i = 0; i < 1; i++) {
					auto* group = ax_new DiaGroup(dia);

					auto* tran0 = ax_new GEO_Transform(group);
					auto* tran1 = ax_new GEO_Transform(group);
					tran0->output.connect(tran1->input);
				}

				dia->saveToJsonFile("_test.dia.json");
				diagramPanel->setDiagram(dia);
			}
			#endif
		}

		if (1) {
			auto* sub = ax_new SCN_Object(model);
			sub->setName("Sub");		

			for (axInt i = 0; i < 4; i++) {
				auto* test = ax_new SCN_Object(sub);
				axTempString tmp;
				tmp.format("Test {?}", i);
				test->setName(tmp);
			}
		}
	}

	#endif
#endif

//	rootView()->updateLayout();
//	rootView()->dump();
	_mainWin->setVisible(true);
}
	
void EditorApp::onUIKeyEvent(UIKeyEvent& ev) {
	if (ev.key == UIKeyEvent::KeyCode::F1) {
	}
}

void EditorApp::loadScene(axStrView filename) {
	SCN_System::s_instance()->loadSceneFile(filename);
}

void EditorApp::openDiagramPanel(Diagram* dia) {
	evOpenDiagramPanel.emit(dia);
}

void EditorApp::addShortcutKey(axUIShortcutKey key, axObject* obj, axFunc<void()> func) {
	auto& e = _shortcutTable._dict.getOrAdd(key);
	e.ev.bind(obj, func);
}

void EditorApp::onHandleCustomAppEvent() {
	Base::onHandleCustomAppEvent();
	AnonContext::s_instance()->onHandleCustomAppEvent();
}

bool EditorApp::onKeyboardShortcut(UIKeyEvent& ev) {
	if (ev.isDown()) {
		auto s = ev.shortcutKey();
		auto* e = _shortcutTable._dict.find(s);
		if (e) {
			e->ev.emit();
			return true;
		}
	}

	return false;
}

void EditorApp::Modules::create() {
	editorUI.create();
	scene.create();
	geo.create();
}

}} //namespace
