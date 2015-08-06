#include "i_item_role.hpp"
#include "wg_types/hash_utilities.hpp"

#define X( ROLE )																					\
	const char * ROLE##Role::roleName_ = #ROLE;														\
	unsigned int ROLE##Role::roleId_ = static_cast<unsigned int>(HashUtilities::compute( #ROLE ));	\
	ItemRole ROLE##Role::role_ = std::make_pair( ROLE##Role::roleName_, ROLE##Role::roleId_ );
	ITEM_ROLES
#undef X