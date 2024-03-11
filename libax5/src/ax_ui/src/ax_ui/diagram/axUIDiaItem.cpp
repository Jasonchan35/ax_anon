#include "axUIDiaItem.h"
#include <ax_ui/diagram/axUIDiaGroup.h>
#include <ax_ui/diagram/axUIDiagram.h>

axUIDiaItem::axUIDiaItem(axUIView* parent) : Base(parent) {
	_diagram = findParentOfType<axUIDiagram>();
}
