#include "pvp_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_variant/collection.hpp"


namespace
{
	class HeaderItem : public IItem
	{
	public:
		HeaderItem( PvpListModel & model, size_t index )
			: model_( model )
			, index_( index )
		{

		}

		const char * getDisplayText( int column ) const override
		{
			return nullptr;
		}

		ThumbnailData getThumbnail( int column ) const override
		{
			return nullptr;
		}

		Variant getData( int column, size_t roleId ) const override
		{
			auto & collection = model_.getSource();
			if (roleId == ValueTypeRole::roleId_)
			{
				return collection.valueType().getName();
			}
			else if (roleId == KeyTypeRole::roleId_)
			{
				return collection.keyType().getName();
			}
			
			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return Variant();
			}

			if (roleId == IndexPathRole::roleId_)
			{
				ResizingMemoryStream dataStream;
				TextStream s(dataStream);
				Variant value = it.value();
				s << value;
				return dataStream.takeBuffer();
			}

			if (roleId == ValueRole::roleId_)
			{
				//return it.value();

				if (column == 1)
				{
					return "Map";
				}
				else if (column == 2)
				{
					return "Scenario";
				}
				else if (column == 3)
				{
					return "Match Group";
				}
				else if (column == 4)
				{
					return "Logic";
				}
				else
				{
					const int tier = column - 4;
					if (tier <= 9)
					{
						const char digit = '0' + tier;
						return std::string( "t" ) + digit;
					}
					else
					{
						const char digit = '0' + (tier - 10);
						return std::string( "t1" ) + digit;
					}
				}
			}
			else if (roleId == KeyRole::roleId_)
			{
				return it.key();
			}

			return Variant();
		}

		bool setData( int column, size_t roleId, const Variant & data ) override
		{
			assert( false && "Not implemented" );
			return false;
		}

	private:
		PvpListModel & model_;
		size_t index_;
	};

	class CollectionItem : public IItem
	{
	public:
		CollectionItem( const DIRef< IDefinitionManager > & definitionManager,
			PvpListModel & model,
			size_t index )
			: definitionManager_( definitionManager )
			, model_( model )
			, index_( index )
		{

		}

		const char * getDisplayText( int column ) const override
		{
			return nullptr;
		}

		ThumbnailData getThumbnail( int column ) const override
		{
			return nullptr;
		}

		Variant getData( int column, size_t roleId ) const override
		{
			auto & collection = model_.getSource();
			if (roleId == ValueTypeRole::roleId_)
			{
				return collection.valueType().getName();
			}
			else if (roleId == KeyTypeRole::roleId_)
			{
				return collection.keyType().getName();
			}
			
			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return Variant();
			}

			if (roleId == IndexPathRole::roleId_)
			{
				ResizingMemoryStream dataStream;
				TextStream s(dataStream);
				Variant value = it.value();
				s << value;
				return dataStream.takeBuffer();
			}

			if (roleId == ValueRole::roleId_)
			{
				auto pDefinitionManager = definitionManager_.get();
				assert( pDefinitionManager != nullptr );
				auto & definitionManager = (*pDefinitionManager);

				auto & variantValue = it.value();
				if (!variantValue.typeIs< Variant::traits< ObjectHandle >::storage_type >())
				{
					return Variant();
				}
				ObjectHandle instance = variantValue.value< ObjectHandle >();
				if (!instance.isValid())
				{
					return Variant();
				}
				auto definition = instance.getDefinition( definitionManager );

				if (column == 1)
				{
					auto pProperty = definition->findProperty( "mapName" );
					if (pProperty == nullptr)
					{
						return Variant();
					}
					return pProperty->get( instance, definitionManager );
					//return "(33) spaces/00_CO_ocean";
				}
				if (column >= 2)
				{
					auto pScenariosProperty = definition->findProperty( "scenarios" );
					if (pScenariosProperty == nullptr)
					{
						return Variant();
					}
					auto scenariosVariant = pScenariosProperty->get( instance, definitionManager );
					Collection scenariosCollection;
					const bool isScenarios =
						scenariosVariant.tryCast< Collection >( scenariosCollection );
					if (!isScenarios)
					{
						return Variant();
					}
					if (scenariosCollection.empty())
					{
						return Variant();
					}

					auto scenarioVariant = *(scenariosCollection.begin());

					ObjectHandle scenarioHandle;
					const bool isScenario =
						scenarioVariant.tryCast< ObjectHandle >( scenarioHandle );
					if (!isScenario)
					{
						return Variant();
					}

					if (column == 2)
					{
						auto scenarioDefinition =
							scenarioHandle.getDefinition( definitionManager );
						auto pScenarioScenarioProperty = scenarioDefinition->findProperty( "scenario" );
						if (pScenarioScenarioProperty == nullptr)
						{
							return "Unknown";
						}

						return pScenarioScenarioProperty->get( scenarioHandle, definitionManager );
					}
					else if (column == 3)
					{
						auto scenarioDefinition =
							scenarioHandle.getDefinition( definitionManager );
						auto pMatchGroupProperty = scenarioDefinition->findProperty( "matchGroup" );
						if (pMatchGroupProperty == nullptr)
						{
							return "Unknown";
						}

						return pMatchGroupProperty->get( scenarioHandle, definitionManager );
					}
					else if (column == 4)
					{
						auto scenarioDefinition =
							scenarioHandle.getDefinition( definitionManager );
						auto pLogicProperty = scenarioDefinition->findProperty( "logic" );
						if (pLogicProperty == nullptr)
						{
							return "Unknown";
						}

						return pLogicProperty->get( scenarioHandle, definitionManager );
					}
					else
					{
						auto scenarioDefinition =
							scenarioHandle.getDefinition( definitionManager );
						auto pLevelsProperty = scenarioDefinition->findProperty( "levels" );
						if (pLevelsProperty == nullptr)
						{
							return -1;
						}

						auto levelsVariant = pLevelsProperty->get( scenarioHandle, definitionManager );

						Collection levelsCollection;
						const bool isLevels =
							levelsVariant.tryCast< Collection >( levelsCollection );
						if (!isLevels)
						{
							return -1;
						}
						if (levelsCollection.empty())
						{
							return -1;
						}

						auto levelVariant = *(levelsCollection.begin());

						ObjectHandle levelHandle;
						const bool isLevel =
							levelVariant.tryCast< ObjectHandle >( levelHandle );
						if (!isLevel)
						{
							return -1;
						}

						auto levelDefinition =
							levelHandle.getDefinition( definitionManager );

						auto pListProperty = levelDefinition->findProperty( "list" );
						if (pListProperty == nullptr)
						{
							return -1;
						}
						auto pRateProperty = levelDefinition->findProperty( "rate" );
						if (pRateProperty == nullptr)
						{
							return -1;
						}

						auto listVariant = pListProperty->get( levelHandle, definitionManager );
						std::wstring levelList;
						const bool isList =
							listVariant.tryCast< std::wstring >( levelList );
						if (!isList)
						{
							return -1;
						}

						// TODO match numbers better
						if (levelList.size() != 3)
						{
							return -1;
						}
						const int start = levelList[0] - '0';
						const int end = levelList[2] - '0';
						
						const int tier = column - 4;
						if ((tier < start) || (tier > end))
						{
							return 0;
						}

						auto rateVariant = pRateProperty->get( levelHandle, definitionManager );

						int rate = 0;
						const bool isRate =
							rateVariant.tryCast< int >( rate );
						if (!isRate)
						{
							return -1;
						}
						return rate;
					}
				}
				else
				{
					return 0;
				}
			}
			else if (roleId == KeyRole::roleId_)
			{
				return it.key();
			}

			return Variant();
		}

		bool setData( int column, size_t roleId, const Variant & data ) override
		{
			//if (roleId != ValueRole::roleId_)
			//{
			//	return false;
			//}

			//auto & collection = model_.getSource();
			//auto it = collection.begin();
			//for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			//if (it == collection.end())
			//{
			//	return false;
			//}

			//it.setValue( data );
			//return true;
			assert( false && "Not implemented" );
			return false;
		}

	private:
		const DIRef< IDefinitionManager > & definitionManager_;
		PvpListModel & model_;
		size_t index_;
	};
} // namespace


PvpListModel::PvpListModel( IComponentContext & context, Collection & source )
	: CollectionModel()
	, definitionManager_( context )
{
	this->setSource( source );
}


PvpListModel::~PvpListModel()
{
}


IItem * PvpListModel::item( size_t index ) const
{
	if (items_.size() <= index)
	{
		items_.resize( index + 1 );
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	if (index == 0)
	{
		item = new HeaderItem( *const_cast< PvpListModel * >( this ), index );
		items_[index] = std::unique_ptr< IItem >( item );
		return item;
	}

	item = new CollectionItem( definitionManager_,
		*const_cast< PvpListModel * >( this ),
		index - 1 );
	items_[index] = std::unique_ptr< IItem >( item );
	return item;
}


size_t PvpListModel::index( const IItem* item ) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert( it != items_.end() );
	return index;
}


bool PvpListModel::empty() const
{
	return collection_.empty();
}


size_t PvpListModel::size() const
{
	return collection_.size() + 1;
}


int PvpListModel::columnCount() const
{
	return 15;
}
