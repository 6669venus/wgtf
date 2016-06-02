
#include "type_registration.h"

#include "action.h"
#include "graph_editor.h"
#include "graph_node.h"
#include "connector.h"
#include "connection_slot.h"
#include "connection_item.h"

#include <core_reflection/type_class_definition.hpp>

namespace wgt
{
void RegisterGrapEditorTypes(IDefinitionManager& mng)
{
    mng.registerDefinition(new TypeClassDefinition<Action>());
    mng.registerDefinition(new TypeClassDefinition<Connector>());
    mng.registerDefinition(new TypeClassDefinition<ConnectionSlot>());
    mng.registerDefinition(new TypeClassDefinition<GraphEditor>());
    mng.registerDefinition(new TypeClassDefinition<GraphNode>());

    qmlRegisterType<ConnectionItem>("DAVA", 1, 0, "ConnectionItem");
    qmlRegisterType<InteractiveConnectionItem>("DAVA", 1, 0, "InteractiveConnectionItem");
}
} // end namespace wgt
