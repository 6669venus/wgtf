#include "i_item_role.hpp"

#define X( ROLE, ROLENAME )																			\
	const char * ROLE##Role::roleName_ = #ROLENAME;														\
	unsigned int ROLE##Role::roleId_ = ItemRole::compute( #ROLENAME );
	ITEM_ROLES
#undef X