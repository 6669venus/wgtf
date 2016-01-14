#pragma once
#ifndef _MAP_LIST_MODEL_HPP
#define _MAP_LIST_MODEL_HPP


#include "core_data_model/collection_model.hpp"
#include "core_dependency_system/di_ref.hpp"


class IDefinitionManager;


class PvpListModel : public CollectionModel
{
public:
	PvpListModel( IComponentContext & context, Collection & source );
	virtual ~PvpListModel();

	// IListModel
	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem* item ) const override;

	virtual bool empty() const override;
	virtual size_t size() const override;
	virtual int columnCount() const override;

private:
	DIRef< IDefinitionManager > definitionManager_;
};


#endif // _MAP_LIST_MODEL_HPP

