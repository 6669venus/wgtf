#ifndef THUMBNAIL_EXTENSION_HPP
#define THUMBNAIL_EXTENSION_HPP

#include "i_model_extension.hpp"

class IQtFramework;

class ThumbnailExtension : public IModelExtension
{
public:
	ThumbnailExtension();
	virtual ~ThumbnailExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index,
		int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

private:
	IQtFramework * qtFramework_;
};

#endif // THUMBNAIL_EXTENSION_HPP