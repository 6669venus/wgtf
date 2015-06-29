#ifndef BW_COMBOBOX_HPP
#define BW_COMBOBOX_HPP

#include "../qt_new_handler.hpp"

#include <memory>
#include <QQuickItem>

class WGListModel;

class BWComboBox
	: public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(
		QVariant comboModel
		READ getComboModel
		WRITE setComboModel
		NOTIFY comboModelChanged REVISION 1 )
	Q_PROPERTY(
		QVariant chosenItem
		READ getChosenItem
		WRITE setChosenItem
		NOTIFY chosenItemChanged REVISION 1 )

	Q_PROPERTY(
		QString chosenDisplayString
		READ getChosenDisplayString
		NOTIFY chosenDisplayItemChanged REVISION 1 )
public:
	BWComboBox( QQuickItem * parent = NULL );
	~BWComboBox();

	DECLARE_QT_MEMORY_HANDLER

	Q_INVOKABLE void setChosenItem( QVariant chosenItem );

signals:
	Q_REVISION( 1 ) void comboModelChanged();
	Q_REVISION( 1 ) void chosenItemChanged();
	Q_REVISION( 1 ) void chosenDisplayItemChanged();

protected:
	void componentComplete();

private:
	std::unique_ptr< WGListModel > comboModel_;
	QVariant chosenItem_;

	QVariant getComboModel();
	void setComboModel( QVariant comboModel );

	QVariant getChosenItem();

	QString getChosenDisplayString();

	void chosenDisplayItemChangedInternal();
	void childrenRectChangedInternal();
};

#endif //BW_COMBOBOX_HPP