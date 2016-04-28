#include "class_definition_model_new.hpp"

#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include <iterator>

namespace
{
	class ClassDefinitionItem : public AbstractListItem
	{
	public:
		ClassDefinitionItem( const IClassDefinition * definition ) 
			: definition_( definition )
		{}

		Variant getData( int column, size_t roleId ) const override
		{ 
			if (roleId == ValueRole::roleId_)
			{
				return ObjectHandle(
					const_cast< IClassDefinition * >( definition_ ) );
			}
			else if (roleId == ValueTypeRole::roleId_)
			{
				return TypeId::getType< ObjectHandle >().getName();
			}
			else if (roleId == KeyRole::roleId_)
			{
				return definition_->getName();
			}
			else if (roleId == KeyTypeRole::roleId_)
			{
				return TypeId::getType< const char * >().getName();
			}
			else if (roleId == IndexPathRole::roleId_)
			{
				return definition_->getName();
			}
			return Variant();
		}	
		
		bool setData( int column, size_t roleId, const Variant & data ) override
		{ 
			return false;
		}

	private:
		const IClassDefinition * definition_;
	};
}

ClassDefinitionModelNew::ClassDefinitionModelNew( const IClassDefinition * definition,
	const IDefinitionManager & definitionManager )
{
	std::vector< IClassDefinition * > definitions;
	definitionManager.getDefinitionsOfType(
		definition, definitions );

	for (auto it = definitions.begin(); it != definitions.end(); ++it)
	{
		items_.push_back( new ClassDefinitionItem( *it ) );
	}
}

ClassDefinitionModelNew::~ClassDefinitionModelNew()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

AbstractItem * ClassDefinitionModelNew::item( int row ) const /* override */
{
	assert( row >= 0 );
	const auto index = static_cast< std::vector< AbstractItem * >::size_type >( row );
	assert( index < items_.size() );
	return items_[ index ];
}

int ClassDefinitionModelNew::index( const AbstractItem * item ) const /* override */
{
	auto it = std::find( items_.begin(), items_.end(), item );
	assert( it != items_.end() );
	return static_cast< int >( std::distance( items_.begin(), it ) );
}


int ClassDefinitionModelNew::rowCount() const /* override */
{
	return static_cast< int >( items_.size() );
}


int ClassDefinitionModelNew::columnCount() const /* override */
{
	return 1;
}
