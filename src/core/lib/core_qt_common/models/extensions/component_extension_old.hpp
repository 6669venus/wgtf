#ifndef COMPONENT_EXTENSION_OLD_HPP
#define COMPONENT_EXTENSION_OLD_HPP

#include "i_model_extension.hpp"

class IQtFramework;

class ComponentExtensionOld : public IModelExtension
{
public:
	ComponentExtensionOld();
	virtual ~ComponentExtensionOld();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

private:
	IQtFramework * qtFramework_;
};

#endif // COMPONENT_EXTENSION_HPP