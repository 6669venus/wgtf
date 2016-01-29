#ifndef BW_TEXTFIELD_HPP
#define BW_TEXTFIELD_HPP

#include "../qt_new_handler.hpp"

#include <QQuickItem>

class BWTextField
	: public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(
		QVariant text
		READ getText
		WRITE setText
		NOTIFY textChanged REVISION 1 )
public:
	BWTextField( QQuickItem * parent = NULL );
	~BWTextField();

	Q_INVOKABLE void setText( QVariant displayText );

	Q_INVOKABLE QVariantMap upPressed(
		QVariant textFieldWrapper, QVariant cursorPositionWrapper );
	Q_INVOKABLE QVariantMap downPressed(
		QVariant textField, QVariant cursorPosition );

	DECLARE_QT_MEMORY_HANDLER

signals:
	Q_REVISION( 1 ) void textChanged();

protected:
	void componentComplete();

private:
	QString text_;

	QVariant getText();

	void childrenRectChangedInternal();
	void textChangedInternal();
};

#endif //BW_TEXTFIELD_HPP