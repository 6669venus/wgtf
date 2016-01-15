#include "pvp_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_variant/collection.hpp"


#include <cstdlib>
#include <cwchar>
#include <limits>
#include <string>


namespace
{
enum COLUMNS {
	//UNKNOWN_COLUMN = 0,
	MAP_COLUMN = 0,
	SCENARIO_COLUMN,
	MATCH_GROUP_COLUMN,
	LOGIC_COLUMN,
	TIER_COLUMN
};
const int TIER_COUNT = 10;
const int MIN_RATE = 0;
const int MAX_RATE = 10;
} // namespace

class PvpListModel::HeaderItem : public IItem
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
		//auto & collection = model_.getSource();
		if (roleId == ValueTypeRole::roleId_)
		{
			if (column == MAP_COLUMN)
			{
				return TypeId::getType< std::string >().getName();
			}
			else if (column == SCENARIO_COLUMN)
			{
				return TypeId::getType< std::string >().getName();
			}
			else if (column == MATCH_GROUP_COLUMN)
			{
				return TypeId::getType< std::string >().getName();
			}
			else if (column == LOGIC_COLUMN)
			{
				return TypeId::getType< std::string >().getName();
			}
			else if (column >= TIER_COLUMN)
			{
				return TypeId::getType< int >().getName();
			}
			else
			{
				return TypeId::getType< std::string >().getName();
			}
		}
		//else if (roleId == KeyTypeRole::roleId_)
		//{
		//	return collection.keyType().getName();
		//}
		
		//auto it = collection.begin();
		//for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
		//if (it == collection.end())
		//{
		//	return Variant();
		//}

		//if (roleId == IndexPathRole::roleId_)
		//{
		//	ResizingMemoryStream dataStream;
		//	TextStream s(dataStream);
		//	Variant value = it.value();
		//	s << value;
		//	return dataStream.takeBuffer();
		//}

		if (roleId == MinValueRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return 0;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == MaxValueRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return TIER_COUNT;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == StepSizeRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return 1;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == DecimalsRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return 0;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == IsReadOnlyRole::roleId_)
		{
			return true;
		}

		if (roleId == ValueRole::roleId_)
		{
			if (column == MAP_COLUMN)
			{
				return "Map";
			}
			else if (column == SCENARIO_COLUMN)
			{
				return "Scenario";
			}
			else if (column == MATCH_GROUP_COLUMN)
			{
				return "Match Group";
			}
			else if (column == LOGIC_COLUMN)
			{
				return "Logic";
			}
			else if (column >= TIER_COLUMN)
			{
				const int tier = column - TIER_COLUMN + 1;
				return tier;
			}
			else
			{
				return "Unknown";
			}
		}
		//else if (roleId == KeyRole::roleId_)
		//{
		//	return it.key();
		//}

		return Variant();
	}

	bool setData( int column, size_t roleId, const Variant & data ) override
	{
		return false;
	}

private:
	PvpListModel & model_;
	size_t index_;
};

class PvpListModel::CollectionItem : public IItem
{
public:
	CollectionItem( const DIRef< IDefinitionManager > & definitionManager,
		PvpListModel & model,
		size_t index )
		: definitionManager_( definitionManager )
		, model_( model )
		, index_( index )
	{

		size_t position = 0;
		model_.walkToScenario( index_, position, collectionHandle_, scenarioHandle_ );
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
			if (column == MAP_COLUMN)
			{
				return TypeId::getType< std::wstring >().getName();
			}
			else if (column == SCENARIO_COLUMN)
			{
				return TypeId::getType< std::wstring >().getName();
			}
			else if (column == MATCH_GROUP_COLUMN)
			{
				return TypeId::getType< std::wstring >().getName();
			}
			else if (column == LOGIC_COLUMN)
			{
				return TypeId::getType< std::wstring >().getName();
			}
			else if (column >= TIER_COLUMN)
			{
				return TypeId::getType< int >().getName();
			}
			else
			{
				return TypeId::getType< std::wstring >().getName();
			}
		}
		if (roleId == MinValueRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return MIN_RATE;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == MaxValueRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return MAX_RATE;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == StepSizeRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return 1;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == DecimalsRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return 0;
			}
			else
			{
				return Variant();
			}
		}
		if (roleId == IsReadOnlyRole::roleId_)
		{
			if (column >= TIER_COLUMN)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		//else if (roleId == KeyTypeRole::roleId_)
		//{
		//	return collection.keyType().getName();
		//}
		
		if (!collectionHandle_.isValid())
		{
			return Variant();
		}
		if (!scenarioHandle_.isValid())
		{
			return Variant();
		}

		//if (roleId == IndexPathRole::roleId_)
		//{
		//	ResizingMemoryStream dataStream;
		//	TextStream s(dataStream);
		//	Variant value = it.value();
		//	s << value;
		//	return dataStream.takeBuffer();
		//}

		if (roleId == ValueRole::roleId_)
		{
			auto pDefinitionManager = definitionManager_.get();
			assert( pDefinitionManager != nullptr );
			auto & definitionManager = (*pDefinitionManager);

			if (!collectionHandle_.isValid())
			{
				return Variant();
			}

			if (column == MAP_COLUMN)
			{
				auto definition = collectionHandle_.getDefinition( definitionManager );
				if (definition == nullptr)
				{
					return false;
				}
				auto pProperty = definition->findProperty( "mapName" );
				if (pProperty == nullptr)
				{
					return Variant();
				}
				return pProperty->get( collectionHandle_, definitionManager );
			}
			else if (column == SCENARIO_COLUMN)
			{
				auto scenarioDefinition =
					scenarioHandle_.getDefinition( definitionManager );
				if (scenarioDefinition == nullptr)
				{
					return false;
				}
				auto pScenarioScenarioProperty = scenarioDefinition->findProperty( "scenario" );
				if (pScenarioScenarioProperty == nullptr)
				{
					return L"Unknown";
				}

				return pScenarioScenarioProperty->get( scenarioHandle_, definitionManager );
			}
			else if (column == MATCH_GROUP_COLUMN)
			{
				auto scenarioDefinition =
					scenarioHandle_.getDefinition( definitionManager );
				if (scenarioDefinition == nullptr)
				{
					return false;
				}
				auto pMatchGroupProperty = scenarioDefinition->findProperty( "matchGroup" );
				if (pMatchGroupProperty == nullptr)
				{
					return L"Unknown";
				}

				return pMatchGroupProperty->get( scenarioHandle_, definitionManager );
			}
			else if (column == LOGIC_COLUMN)
			{
				auto scenarioDefinition =
					scenarioHandle_.getDefinition( definitionManager );
				if (scenarioDefinition == nullptr)
				{
					return false;
				}
				auto pLogicProperty = scenarioDefinition->findProperty( "logic" );
				if (pLogicProperty == nullptr)
				{
					return L"Unknown";
				}

				return pLogicProperty->get( scenarioHandle_, definitionManager );
			}
			else if (column >= TIER_COLUMN)
			{
				// Subtract map name columns etc.
				const int tier = column - TIER_COLUMN + 1;
				if (tier > TIER_COUNT)
				{
					NGT_ERROR_MSG( "Tier %d is not displayable by the UI\n", tier );
				}

				auto scenarioDefinition =
					scenarioHandle_.getDefinition( definitionManager );
				if (scenarioDefinition == nullptr)
				{
					return false;
				}

				auto pLevelsProperty = scenarioDefinition->findProperty( "levels" );
				if (pLevelsProperty == nullptr)
				{
					return -1;
				}

				auto levelsVariant = pLevelsProperty->get( scenarioHandle_, definitionManager );

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

				for (auto levelIt = levelsCollection.cbegin();
					levelIt != levelsCollection.cend();
					++levelIt)
				{
					auto levelVariant = (*levelIt);

					ObjectHandle levelHandle;
					const bool isLevel =
						levelVariant.tryCast< ObjectHandle >( levelHandle );
					if (!isLevel)
					{
						return -1;
					}

					auto levelDefinition =
						levelHandle.getDefinition( definitionManager );
					if (levelDefinition == nullptr)
					{
						return false;
					}

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

					// In the format "1", "2", "3" etc.
					int singleLevel;
					const bool isSingleLevel =
						listVariant.tryCast< int >( singleLevel );
					if (isSingleLevel)
					{
						if (singleLevel != tier)
						{
							continue;
						}

						auto rateVariant = pRateProperty->get(
							levelHandle, definitionManager );

						int rate = 0;
						const bool isRate =
							rateVariant.tryCast< int >( rate );
						if (!isRate)
						{
							return -1;
						}
						return rate;
					}

					// In the format "1-2", "7-10" etc.
					std::wstring levelList;
					const bool isList =
						listVariant.tryCast< std::wstring >( levelList );
					if (!isList)
					{
						return -1;
					}

					// Check if string is long enough
					if (levelList.empty())
					{
						return -1;
					}

					const wchar_t * pFirstNumber = &levelList[0];
					wchar_t * pAfterLastChar = nullptr;
					const int base = 10;
					const auto startTier = wcstol( pFirstNumber , &pAfterLastChar, base );

					// Skip the '-'
					wchar_t * pSecondNumber = pAfterLastChar + 1;

					// Check if string is long enough
					if (static_cast< size_t >( pSecondNumber - pFirstNumber ) >= levelList.size())
					{
						return -1;
					}
					const int endTier = wcstol( pSecondNumber, nullptr, base );
					
					// Failed to read numbers
					if ((startTier <= 0) || (endTier <= 0))
					{
						return -1;
					}
					if (startTier >= endTier)
					{
						return false;
					}
					if ((startTier > TIER_COUNT) || (endTier > TIER_COUNT))
					{
						NGT_WARNING_MSG( "Not all tiers displayable by UI %d-%d\n",
							startTier,
							endTier );
					}
					
					// If tier is outside of level, does not match this entry
					if ((tier < startTier) || (tier > endTier))
					{
						continue;
					}

					// Otherwise, get rate for tier
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

				return 0;
			}
			else
			{
				return L"Unknown";
			}
		}
		//else if (roleId == KeyRole::roleId_)
		//{
		//	return it.key();
		//}

		return Variant();
	}

	bool setData( int column, size_t roleId, const Variant & data ) override
	{
		if (roleId != ValueRole::roleId_)
		{
			return false;
		}

		if (!collectionHandle_.isValid())
		{
			return false;
		}
		if (!scenarioHandle_.isValid())
		{
			return false;
		}

		if (column == MAP_COLUMN)
		{
			return false;
		}
		else if (column == SCENARIO_COLUMN)
		{
			return false;
		}
		else if (column == MATCH_GROUP_COLUMN)
		{
			return false;
		}
		else if (column == LOGIC_COLUMN)
		{
			return false;
		}
		else if (column >= TIER_COLUMN)
		{
			int rate;
			const bool isRate = data.tryCast< int >( rate );
			if (!isRate)
			{
				NGT_ERROR_MSG( "Data for column %d is not an int\n", column );
				return false;
			}

			if ((rate < MIN_RATE) || (rate > MAX_RATE))
			{
				NGT_ERROR_MSG( "Rate %d is outside the range %d-%d\n",
					rate,
					MIN_RATE,
					MAX_RATE );
				return false;
			}

			// Subtract map name columns etc.
			const int tier = column - TIER_COLUMN + 1;
			if (tier > TIER_COUNT)
			{
				NGT_ERROR_MSG( "Tier %d is not displayable by the UI\n", tier );
			}

			auto pDefinitionManager = definitionManager_.get();
			assert( pDefinitionManager != nullptr );
			auto & definitionManager = (*pDefinitionManager);

			auto scenarioDefinition =
				scenarioHandle_.getDefinition( definitionManager );
			if (scenarioDefinition == nullptr)
			{
				return false;
			}

			auto pLevelsProperty = scenarioDefinition->findProperty( "levels" );
			if (pLevelsProperty == nullptr)
			{
				return false;
			}

			auto levelsVariant = pLevelsProperty->get( scenarioHandle_,
				definitionManager );

			Collection levelsCollection;
			const bool isLevels =
				levelsVariant.tryCast< Collection >( levelsCollection );
			if (!isLevels)
			{
				return false;
			}

			// Search for existing level entry
			for (auto levelIt = levelsCollection.begin();
				levelIt != levelsCollection.end();
				++levelIt)
			{
				auto levelVariant = (*levelIt);

				ObjectHandle levelHandle;
				const bool isLevel =
					levelVariant.tryCast< ObjectHandle >( levelHandle );
				if (!isLevel)
				{
					return false;
				}

				auto levelDefinition =
					levelHandle.getDefinition( definitionManager );
				if (levelDefinition == nullptr)
				{
					return false;
				}

				auto pListProperty = levelDefinition->findProperty( "list" );
				if (pListProperty == nullptr)
				{
					return false;
				}

				auto listVariant = pListProperty->get( levelHandle, definitionManager );

				// In the format "1", "2", "3" etc.
				int singleLevel;
				const bool isSingleLevel =
					listVariant.tryCast< int >( singleLevel );
				if (isSingleLevel)
				{
					if (singleLevel != tier)
					{
						continue;
					}

					auto pRateProperty = levelDefinition->findProperty( "rate" );
					if (pRateProperty == nullptr)
					{
						return false;
					}

					return pRateProperty->set( levelHandle, data, definitionManager );
				}

				// In the format "1-2", "7-10" etc.
				std::wstring levelList;
				const bool isList =
					listVariant.tryCast< std::wstring >( levelList );
				if (!isList)
				{
					return false;
				}

				// Check if string is long enough
				if (levelList.empty())
				{
					return false;
				}

				const wchar_t * pFirstNumber = &levelList[0];
				wchar_t * pAfterLastChar = nullptr;
				const int base = 10;
				const auto startTier = wcstol( pFirstNumber , &pAfterLastChar, base );

				// Skip the '-'
				wchar_t * pSecondNumber = pAfterLastChar + 1;

				// Check if string is long enough
				if (static_cast< size_t >( pSecondNumber - pFirstNumber ) >= levelList.size())
				{
					return false;
				}
				const int endTier = wcstol( pSecondNumber, nullptr, base );
				
				// Failed to read numbers
				if ((startTier <= 0) || (endTier <= 0))
				{
					return false;
				}
				if (startTier >= endTier)
				{
					return false;
				}
				if ((startTier > TIER_COUNT) || (endTier > TIER_COUNT))
				{
					NGT_WARNING_MSG( "Not all tiers displayable by UI %d-%d\n",
						startTier,
						endTier );
				}
				
				// If tier is outside of this range, go to next entry
				if ((tier < startTier) || (tier > endTier))
				{
					continue;
				}

				auto pRateProperty = levelDefinition->findProperty( "rate" );
				if (pRateProperty == nullptr)
				{
					return false;
				}
				auto rateVariant = pRateProperty->get( levelHandle, definitionManager );

				int oldRate = 0;
				const bool isOldRate =
					rateVariant.tryCast< int >( oldRate );
				if (!isOldRate)
				{
					return false;
				}

				// Tier matches the range.
				// But the user may have only edited one entry in the range
				// Delete the old level entry and add 2 new entries

				// Note: invalidates iterator, must exit for loop
				levelsCollection.erase( levelIt );

				if (tier == startTier)
				{
					bool result = true;
					if (rate != 0)
					{
						result &= this->addLevel(
							(*levelDefinition),
							levelsCollection,
							startTier,
							startTier,
							rate );
					}
					result &= this->addLevel(
						(*levelDefinition),
						levelsCollection,
						startTier + 1,
						endTier,
						oldRate );
					return result;
				}
				else if (tier == endTier)
				{
					bool result = true;
					result &= this->addLevel(
						(*levelDefinition),
						levelsCollection,
						startTier,
						endTier - 1,
						oldRate );
					if (rate != 0)
					{
						result &= this->addLevel(
							(*levelDefinition),
							levelsCollection,
							endTier,
							endTier,
							rate );
					}
					return result;
				}

				bool result = true;
				result &= this->addLevel(
					(*levelDefinition),
					levelsCollection,
					startTier,
					tier - 1,
					oldRate );
				if (rate != 0)
				{
					result &= this->addLevel(
						(*levelDefinition),
						levelsCollection,
						tier,
						tier,
						rate );
				}
				result &= this->addLevel(
					(*levelDefinition),
					levelsCollection,
					tier + 1,
					endTier,
					oldRate );
				return result;
			}

			// Existing entry was not found
			//if (rate != 0)
			//{
			// TODO create without definition
			//	return this->addLevel( ObjectHandle( nullptr ), // TODO
			//		levelsCollection,
			//		tier,
			//		tier,
			//		rate );
			//}

			// Setting rate to 0, no existing entry to set, do nothing
			return true;
		}
		return false;
	}

	// TODO need to copy a Level, cannot create a new one
	bool addLevel( const IClassDefinition & existingLevelDefinition,
		Collection & levelsCollection,
		int startTier,
		int endTier,
		int rate )
	{
		auto pDefinitionManager = definitionManager_.get();
		assert( pDefinitionManager != nullptr );
		auto & definitionManager = (*pDefinitionManager);

		ObjectHandle newLevelHandle = existingLevelDefinition.create();
		if (!newLevelHandle.isValid())
		{
			return false;
		}
		levelsCollection.insert( newLevelHandle );
		const auto newLevelDefinition = newLevelHandle.getDefinition( definitionManager );

		// TODO NGT-1319 cannot add new properties
		auto pListProperty = newLevelDefinition->findProperty( "list" );
		if (pListProperty == nullptr)
		{
			return false;
		}
		auto pRateProperty = newLevelDefinition->findProperty( "rate" );
		if (pRateProperty == nullptr)
		{
			return false;
		}

		// Convert tiers to string
		if (startTier == endTier)
		{
			// max int digits
			// TODO wchar_t
			char tierName[ (sizeof( int ) * 8 + 1) ];
			const int base = 10;
			itoa( startTier, &tierName[0], 10 );
			pListProperty->set( newLevelHandle, tierName, definitionManager );
			pRateProperty->set( newLevelHandle, rate, definitionManager );
			return true;
		}

		// max int digits
		// TODO wchar_t
		const size_t maxLength = (sizeof( int ) * 8 + 1) + 1 + (sizeof( int ) * 8 + 1);
		char tierName[ maxLength ];
		const int base = 10;
		itoa( startTier, &tierName[0], 10 );
		const auto firstIntLen = strlen( tierName );
		tierName[ firstIntLen ] = '-';
		itoa( endTier, &tierName[ firstIntLen + 1 ], 10 );

		pListProperty->set( newLevelHandle, tierName, definitionManager );
		pRateProperty->set( newLevelHandle, rate, definitionManager );
		return true;
	}

private:
	const DIRef< IDefinitionManager > & definitionManager_;
	PvpListModel & model_;
	size_t index_;
	ObjectHandle collectionHandle_;
	ObjectHandle scenarioHandle_;
};


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
		index );
	items_[index] = std::unique_ptr< IItem >( item );
	return item;
}


bool PvpListModel::empty() const
{
	// First row is a header
	return false;
}


size_t PvpListModel::size() const
{
	const size_t impossibleIndex = std::numeric_limits< size_t >::max();
	size_t size = 0;
	ObjectHandle collectionHandle;
	ObjectHandle scenarioHandle;
	this->walkToScenario( impossibleIndex,
		size,
		collectionHandle,
		scenarioHandle );

	// +1 - first row is a header
	return size + 1;
}


int PvpListModel::columnCount() const
{
	return (TIER_COLUMN + TIER_COUNT);
}


// Index including header
void PvpListModel::walkToScenario( size_t index,
	size_t & outPosition,
	ObjectHandle & outCollectionHandle,
	ObjectHandle & outScenarioHandle ) const
{
	outPosition = 0;

	if (collection_.empty())
	{
		return;
	}

	auto pDefinitionManager = definitionManager_.get();
	assert( pDefinitionManager != nullptr );
	auto & definitionManager = (*pDefinitionManager);

	for (auto & collectionIt = collection_.cbegin();
		collectionIt != collection_.cend();
		++collectionIt)
	{
		auto & variantValue = collectionIt.value();
		if (!variantValue.typeIs< Variant::traits< ObjectHandle >::storage_type >())
		{
			continue;
		}
		ObjectHandle collectionHandle = variantValue.value< ObjectHandle >();
		if (!collectionHandle.isValid())
		{
			continue;
		}
		auto definition = collectionHandle.getDefinition( definitionManager );

		auto pScenariosProperty = definition->findProperty( "scenarios" );
		if (pScenariosProperty == nullptr)
		{
			continue;
		}
		auto scenariosVariant = pScenariosProperty->get( collectionHandle, definitionManager );
		Collection scenariosCollection;
		const bool isScenarios =
			scenariosVariant.tryCast< Collection >( scenariosCollection );
		if (!isScenarios)
		{
			continue;
		}
		if (scenariosCollection.empty())
		{
			continue;
		}

		for (auto scenarioIt = scenariosCollection.cbegin();
			scenarioIt != scenariosCollection.cend();
			++scenarioIt)
		{
			auto scenarioVariant = (*scenarioIt);

			ObjectHandle scenarioHandle;
			const bool isScenario =
				scenarioVariant.tryCast< ObjectHandle >( scenarioHandle );
			if (!isScenario)
			{
				continue;
			}

			// Check the match group is "pvp"
			auto scenarioDefinition =
				scenarioHandle.getDefinition( definitionManager );
			if (scenarioDefinition == nullptr)
			{
				return;
			}
			auto pMatchGroupProperty = scenarioDefinition->findProperty(
				"matchGroup" );
			if (pMatchGroupProperty == nullptr)
			{
				continue;
			}

			const auto matchGroupVariant = pMatchGroupProperty->get(
				scenarioHandle, definitionManager );
			std::wstring matchGroup;
			const bool isMatchGroup =
				matchGroupVariant.tryCast< std::wstring >( matchGroup );
			if (!isMatchGroup)
			{
				continue;
			}
			if (matchGroup != L"pvp")
			{
				continue;
			}

			// Add scenario to total
			outPosition += 1;
			if (index == outPosition)
			{
				outCollectionHandle = collectionHandle;
				outScenarioHandle = scenarioHandle;
				return;
			}
		}
	}

	return;
}
