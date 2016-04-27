#pragma once
#ifndef _CLASS_DEFINITION_MODEL_NEW_HPP
#define _CLASS_DEFINITION_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"
#include <vector>

class IClassDefinition;
class IDefinitionManager;

class ClassDefinitionModelNew : public AbstractListModel
{
public:
	ClassDefinitionModelNew( const IClassDefinition * definition, const IDefinitionManager & definitionManager );
	virtual ~ClassDefinitionModelNew();

	AbstractItem * item( int index ) const override;
	int index( const AbstractItem * item ) const override;

	int rowCount() const override;
	int columnCount() const override;

private:
	std::vector< AbstractItem * > items_;
};

#endif // _CLASS_DEFINITION_MODEL_NEW_HPP