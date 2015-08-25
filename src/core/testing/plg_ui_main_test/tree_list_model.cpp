#include "tree_list_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "pages/test_page.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
//#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include <codecvt>


namespace
{
	//==============================================================================
	class TestListItem : public VariantListItem
	{
	public:
		TestListItem( const Variant& value, IDefinitionManager & definitionManager )
			: VariantListItem( value )
			, displayName_( "Unknown" )
		{
			if (value.typeIs<ObjectHandle>())
			{
				ObjectHandle objHandle;
				bool isOk = value.tryCast( objHandle );
				assert( isOk );
				auto metaName =
					findFirstMetaData< MetaDisplayNameObj >( *objHandle.getDefinition( definitionManager ) );
				if (metaName != nullptr)
				{
					std::wstring_convert< Utf16to8Facet > conversion( 
						Utf16to8Facet::create() );

					displayName_ = conversion.to_bytes( metaName->getDisplayName() );
				}
				else
				{
					const auto classDef = objHandle.getDefinition( definitionManager );
					if (classDef != nullptr)
					{
						displayName_ = classDef->getName();
					}
				}
			}
		}
		TestListItem( Variant&& value, IDefinitionManager & definitionManager )
			: VariantListItem( std::forward<Variant&&>( value ) )
			, displayName_( "Unknown" )
		{
			const Variant & Value = this->value<const Variant &>();
			if (Value.typeIs<ObjectHandle>())
			{
				ObjectHandle objHandle;
				bool isOk = Value.tryCast( objHandle );
				assert( isOk );
				auto metaName =
					findFirstMetaData< MetaDisplayNameObj >( *objHandle.getDefinition( definitionManager ) );
				if (metaName != nullptr)
				{
					std::wstring_convert< Utf16to8Facet > conversion( 
						Utf16to8Facet::create() );

					displayName_ = conversion.to_bytes( metaName->getDisplayName() );
				}
				else
				{
					const auto classDef = objHandle.getDefinition( definitionManager );
					if (classDef != nullptr)
					{
						displayName_ = classDef->getName();
					}
				}
			}
		}
		// IItem
		const char * getDisplayText( int column ) const override
		{
			return displayName_.c_str();
		}

	private:
		//TODO: http://jira.bigworldtech.com/browse/NGT-434
		mutable std::string displayName_;
	};
}

TreeListModel::TreeListModel()
{
}



TreeListModel::~TreeListModel()
{
}

void TreeListModel::init( IDefinitionManager & defManager, IReflectionController & controller )
{
	pDefManager_ = &defManager;
	assert( pDefManager_ != nullptr );
	controller_ = &controller;
	assert( controller_ != nullptr );
	std::vector< ObjectHandle > objects;
	pDefManager_->getObjectManager()->getObjects(objects);
	std::unique_ptr<VariantList> listModel( new VariantList() );
	for (auto object : objects)
	{
		auto def = object.getDefinition( defManager );
		if (def == nullptr || def->isGeneric() || (object == this) )
		{
			continue;
		}
		std::unique_ptr<TestListItem> item( new TestListItem( object, defManager ) );
		listModel->emplace_back( item.release() );
	}
	listModel_ = std::move( listModel );
}


ObjectHandle TreeListModel::getTreeModel() const
{
	auto listModel = listModel_.getBase<VariantList>();
	assert( listModel != nullptr && !listModel->empty() );
	const Variant value = (*listModel)[0].value<const Variant &>();
	ObjectHandle objHandle;
	bool isOk = value.tryCast( objHandle );
	assert( isOk );

	auto model = std::unique_ptr< ITreeModel >(
		new ReflectedTreeModel( objHandle, *pDefManager_, controller_ ) );
	return std::move( model );
}

ObjectHandle TreeListModel::getListModel() const
{
	return listModel_;
}
