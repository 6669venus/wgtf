#ifndef I_ITEM_ROLE_HPP
#define I_ITEM_ROLE_HPP

#include <Utility>

typedef std::pair< const char *, unsigned int > ItemRole;

// Add new role types here
#define ITEM_ROLES																\
	/* Tree Adapter */															\
	X( ChildModel )																\
	X( ColumnModel )															\
	X( HasChildren )															\
	X( Expanded )																\
	X( Selected )																\
	/* Component Extension */													\
	X( Component )																\
	X( ValueType )																\
	X( KeyType )																\
	X( IsEnum )																	\
	X( IsThumbnail )															\
	X( IsSlider )																\
	X( IsColor )																\
	/* Value Extension */														\
	X( Value )																	\
	X( Key )																	\
	X( MinValue )																\
	X( MaxValue )																\
	X( EnumModel )																\
	X( Definition )																\
	X( DefinitionModel )														\
	/* Thumbnail Extension */													\
	X( Thumbnail )																\

#define X( ROLE ) 																\
	class ROLE##Role															\
	{																			\
	public:																		\
		static const char * roleName_;											\
		static unsigned int roleId_;											\
		static ItemRole role_;													\
	};
	ITEM_ROLES
#undef X

#endif //I_ITEM_ROLE_HPP