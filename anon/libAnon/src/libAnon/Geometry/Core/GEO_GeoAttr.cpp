#include "GEO_GeoAttr.h"

namespace Anon {

// https://en.wikipedia.org/wiki/Doubly_connected_edge_list
// The doubly connected edge list (DCEL), also known as half-edge data structure
//
//                           ^--------------->                           
//                           |               |                           
//             Edge 0        |    Point 0    v        Edge 1             
// ---------------------------------> <----------------------------------
//                           ^       ^       |                           
//                           |       |       v                           
//                           |<------|<-------                           
//                                   |                                   
//  vertex 0           vertex 3      |       vertex 4            vertex 5
//   <----------------------^        |        <------------------------^ 
//   |                      |        |        |                        | 
//   |                      |<----------------|                        | 
//   |                      |        |        |                        | 
//   |     Primitive 0      |  edge 2|------->|       Primitive 1      | 
//   |                      |        |        |                        | 
//   |                      |---------------->|                        | 
//   |                      |        |        |                        | 
//   v---------------------->        |        v------------------------> 
//  vertex 1           vertex 2      |       vertex 7            vertex 6
//                                   |                                   
//                                   |                                   
//                                   |                                   
//                                Point 1                                


//===========================================================
//  GEO_Component - shared attributes
//
//  GeoData0.pointAttrs         GeoData1.pointAttrs             GeoData2.pointAttrs
// +------------------+          +-------------------+           +-------------------+      
// | (owner) Position |--------->| Position          |---------->| Position          |----->
// |                  |          |                   |           |                   |      
// |    (owner) Color |--------->| Color             |     ^---> | Color             |----->
// +------------------+          |                   |     |     |                   |      
//                         ^---->| Normal            |---------> | Normal            |----->
// +------------------+    |     +-------------------+     |     +-------------------+      
// |   (owner) Normal |---->                               |                                
// +------------------+          +-------------------+     |                                
//                               |     (owner) Color |----->                                
//                               +-------------------+                                      



void GEO_Edge::onFormat(axFormat& f) const {
	f.format("GEO_Edge(point={?},{?} edge={?},{?})", point0, point1, nextEdge0, nextEdge1);
}

void GEO_Component::clear() {
	_attrs.resize(0);
	_size = 0;
}

void GEO_Component::resize(axInt n) {
	if (_size == n) return;
	_size = n;
	for (auto& a : _attrs) {
		if (a->owner() != this) {
			auto newCopy = a->baseClone(this);
			a.ref(newCopy.ptr());
		}
		a->resize(this, _size);
	}
}

void GEO_Component::onGetObjectInfo(axIString& info) {
	if (_size == 0) return;
	info.appendFormat("{?} [{?}]:\n", _type, _size);
	for (auto& a : _attrs) {
		info.appendFormat("  {?} ({?})\n", a->name(), a->type()->name());
	}
}

void GEO_Component::_addAttr(GEO_AttrArrayBase* newAttr) {
	if (!newAttr) return;

	for (auto& a : _attrs) {
		if (a->name() == newAttr->name()) {
			a.ref(newAttr); // replace
			return;
		}
	}
	_attrs.emplaceBack(newAttr);
}

GEO_AttrArrayBase* GEO_Component::_findAttrBase(axNameId name) & {
	for (auto& a : _attrs) {
		if (!a) continue;
		if (a->name() == name)
			return a.ptr();
	}
	return nullptr;
}

} // namespace