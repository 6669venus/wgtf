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
	X( ParentIndex )															\
	X( Selected )																\
	X( IndexPath )																\
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
	/* TODO: Temporary roles on Value Extension for AB file info */				\
	X( TypeIcon )																\
	X( Size )																	\
	X( CreatedTime )															\
	X( ModifiedTime )															\
	X( AccessedTime )															\
	X( IsDirectory )															\
	X( IsReadOnly )																\
	X( IsCompressed )															\
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