#include "class_definition_model.hpp"

#include "data_model/i_item.hpp"
#include "data_model/i_item_role.hpp"
#include "reflection/interfaces/i_class_definition.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/utilities/reflection_utilities.hpp"

namespace
{
	class ClassDefinitionItem : public IItem
	{
	public:
		ClassDefinitionItem( const IClassDefinition * definition ) 
			: definition_( definition )
		{}

		int columnCount() const 
		{ 
			return 1; 
		}

		const char * getDisplayText( int column ) const 
		{ 
			return definition_->getName(); 
		}

		ThumbnailData getThumbnail( int column ) const
		{
			return nullptr;
		}

		Variant getData( int column, size_t roleId ) const 
		{ 
			if (roleId == ValueRole::roleId_)
			{
				return ObjectHandle( 
					const_cast< IClassDefinition * >( definition_ ) );
			}
			return Variant();
		}	
		
		bool setData( int column, size_t roleId, const Variant & data ) 
		{ 
			return false; 
		}

	private:
		const IClassDefinition * definition_;
	};
}

ClassDefinitionModel::ClassDefinitionModel( const IClassDefinition * definition )
{
	std::vector< IClassDefinition * > definitions;
	definition->getDefinitionManager()->getDefinitionsOfType(
		definition, definitions );

	for( auto it = definitions.begin(); it != definitions.end(); ++it )
	{
		items_.push_back( new ClassDefinitionItem( *it ) );
	}
}

ClassDefinitionModel::~ClassDefinitionModel()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

IItem * ClassDefinitionModel::item( size_t index ) const
{
	assert( index < items_.size() );
	return items_[ index ];
}

size_t ClassDefinitionModel::index( const IItem * item ) const
{
	auto it = std::find( items_.begin(), items_.end(), item );
	assert( it != items_.end() );
	return it - items_.begin();
}


bool ClassDefinitionModel::empty() const
{
	return items_.empty();
}


size_t ClassDefinitionModel::size() const
{
	return items_.size();
}
