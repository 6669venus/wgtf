#include "i_item_role.hpp"

#define X( ROLE )																					\
	const char * ROLE##Role::roleName_ = #ROLE;														\
	unsigned int ROLE##Role::roleId_ = ItemRole::compute( #ROLE );
	ITEM_ROLES
#undef X