#pragma once

#include "Scene/Lights/SCN_SpotLight.h"
#include "Scene/SCN_System.h"
#include "Scene/SCN_Object.h"
#include "Scene/SCN_Geometry.h"
#include "Scene/SCN_Component.h"
#include "Scene/SCN_Camera.h"
#include "Geometry/Combine/GEO_CopyToPoints.h"
#include "Geometry/Combine/GEO_Merge.h"
#include "Geometry/Core/GEO_Modifier.h"
#include "Geometry/Core/GEO_Node.h"
#include "Geometry/Deform/GEO_Bend.h"
#include "Geometry/Deform/GEO_Subdiv.h"
#include "Geometry/Shape/GEO_Cube.h"
#include "Geometry/Shape/GEO_Grid.h"
#include "Geometry/Shape/GEO_WaveFrontObj.h"
#include "Geometry/Transform/GEO_Transform.h"
#include "Scene/Lights/SCN_DirectionalLight.h"
#include "Scene/Lights/SCN_LightBase.h"
#include "Scene/Lights/SCN_PointLight.h"


template<class HANDLER> inline
static void generated_node_types(HANDLER& handler) {
	handler.template addType< Anon::SCN_SpotLight                      >();
	handler.template addType< Anon::SCN_System                         >();
	handler.template addType< Anon::SCN_Object                         >();
	handler.template addType< Anon::SCN_Geometry                       >();
	handler.template addType< Anon::SCN_Component                      >();
	handler.template addType< Anon::SCN_Camera                         >();
	handler.template addType< Anon::GEO_CopyToPoints                   >();
	handler.template addType< Anon::GEO_Merge                          >();
	handler.template addType< Anon::GEO_Modifier                       >();
	handler.template addType< Anon::GEO_Node                           >();
	handler.template addType< Anon::GEO_Bend                           >();
	handler.template addType< Anon::GEO_Subdiv                         >();
	handler.template addType< Anon::GEO_Cube                           >();
	handler.template addType< Anon::GEO_Grid                           >();
	handler.template addType< Anon::GEO_WaveFrontObj                   >();
	handler.template addType< Anon::GEO_Transform                      >();
	handler.template addType< Anon::SCN_DirectionalLight               >();
	handler.template addType< Anon::SCN_LightBase                      >();
	handler.template addType< Anon::SCN_PointLight                     >();
}

