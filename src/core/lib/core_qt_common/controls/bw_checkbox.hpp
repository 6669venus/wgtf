#ifndef BW_CHECKBOX_HPP
#define BW_CHECKBOX_HPP

#include "../qt_new_handler.hpp"
#include <QtQuick/QQuickItem>

class BWCheckBox
	: public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(
		QVariant checked
		READ getChecked
		WRITE setChecked
		NOTIFY checkedChanged REVISION 1 )

public:
	BWCheckBox( QQuickItem * parent = NULL );
	~BWCheckBox();

	Q_INVOKABLE void setChecked( QVariant checked );

	DECLARE_QT_MEMORY_HANDLER

signals:
	Q_REVISION( 1 ) void checkedChanged();

protected:
	void componentComplete();

private:
	bool checked_;

	QVariant getChecked();

	void childrenRectChangedInternal();
	void checkedChangedInternal();

	std::vector< QMetaObject::Connection > connections_;
};

#endif //BW_CHECKBOX_HPP