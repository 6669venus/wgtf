#ifndef VALUE_EXTENSION_HPP
#define VALUE_EXTENSION_HPP

#include "i_model_extension.hpp"

class ValueExtension : public IModelExtension
{
public:
	ValueExtension();
	virtual ~ValueExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

	void onDataAboutToBeChanged( const QModelIndex &index,
		int role,
		const QVariant &value ) override;
	void onDataChanged( const QModelIndex &index,
		int role,
		const QVariant &value ) override;
};

#endif // COMPONENT_EXTENSION_HPP