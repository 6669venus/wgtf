#pragma once
#ifndef _MATCH_GROUP_LIST_MODEL_HPP
#define _MATCH_GROUP_LIST_MODEL_HPP


#include "core_data_model/collection_model.hpp"
#include "core_dependency_system/di_ref.hpp"


class IDefinitionManager;
class ObjectHandle;


/**
 *	Lookup all matching scenarios from source and provide a list.
 */
class MatchGroupListModel : public CollectionModel
{
public:
	/**
	 *	@param context
	 *	@param source the collection of Space objects used to make this model.
	 *	@param matchGroup filter for matching scenarios.
	 */
	MatchGroupListModel( IComponentContext & context,
		Collection & source,
		const wchar_t * matchGroup );
	virtual ~MatchGroupListModel();

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

	/**
	 *	Walk through the Python data structures to get to the Scenario entry
	 *	for each row in the IListModel.
	 *	@param index row to be converted to a Scenario.
	 *	@param outPosition output number of elements walked.
	 *	@param outSpaceHandle output the handle to the top-level Space object.
	 *	@param outScenarioHandle output the Scenario corresponding to the index.
	 */
	void walkToScenario( size_t index,
		size_t & outPosition,
		ObjectHandle & outSpaceHandle,
		ObjectHandle & outScenarioHandle ) const;

	DIRef< IDefinitionManager > definitionManager_;
	std::wstring matchGroup_;
};


#endif // _MATCH_GROUP_LIST_MODEL_HPP

