#pragma once
#ifndef _MAP_LIST_MODEL_HPP
#define _MAP_LIST_MODEL_HPP


#include "core_data_model/collection_model.hpp"
#include "core_dependency_system/di_ref.hpp"


class IDefinitionManager;
class ObjectHandle;


/**
 *	Lookup all pvp scenarios from source and provide a list.
 */
class PvpListModel : public CollectionModel
{
public:
	PvpListModel( IComponentContext & context, Collection & source );
	virtual ~PvpListModel();

	// IListModel
	virtual IItem * item( size_t index ) const override;

	virtual bool empty() const override;

	/**
	 *	Calculate the total number of pvp entries contained within source.
	 *	@return the number of pvp scenarios.
	 */
	virtual size_t size() const override;

	/**
	 *	@return the number of tiers + info columns.
	 */
	virtual int columnCount() const override;

private:
	class CollectionItem;
	class HeaderItem;
	void walkToScenario( size_t index,
		size_t & outPosition,
		ObjectHandle & outCollectionHandle,
		ObjectHandle & outScenarioHandle ) const;
	DIRef< IDefinitionManager > definitionManager_;
};


#endif // _MAP_LIST_MODEL_HPP

