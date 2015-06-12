#ifndef SELECTION_EXTENSION_HPP
#define SELECTION_EXTENSION_HPP

#include "i_model_extension.hpp"
#include <memory>

class SelectionExtension : public IModelExtension
{
	Q_OBJECT

	Q_PROPERTY( QVariant selectedIndex
				READ getSelectedIndex
				WRITE setSelectedIndex
				NOTIFY selectionChanged )

public:
	SelectionExtension();
	virtual ~SelectionExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

	void onDataAboutToBeChanged( const QModelIndex& index,
		int role,
		const QVariant& value );
	void onDataChanged( const QModelIndex& index,
		int role,
		const QVariant& value );

signals:
	void selectionChanged();

private:
	QVariant getSelectedIndex() const;
	void setSelectedIndex( const QVariant& index );

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // SELECTION_EXTENSION_HPP