#ifndef BW_COPYABLE_HPP
#define BW_COPYABLE_HPP
#include "core_copy_paste/i_copyable_object.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_variant/variant.hpp"
#include <QObject>
#include <QVariant>

class BWCopyable
	: public QObject
	, public ICopyableObject
{
	Q_OBJECT

	Q_PROPERTY( QVariant data MEMBER data_  )

public:
	BWCopyable( QObject * parent = NULL );
	~BWCopyable();

	Q_INVOKABLE void setValue( const QVariant & data );
	// This hint is designed for 
	// searching most suitable controls which the value could paste to. 
	Q_INVOKABLE void setValueHint( const QString & hint );
	DECLARE_QT_MEMORY_HANDLER

	const char * getDataHint() const override;
	const Variant & getData() override;
	bool setData( const Variant & value ) override;


signals:
	void dataCopied();
	void dataPasted();

private:
	std::string  valueHint_;
	QVariant data_;
	Variant value_;
};

#endif //BW_COPYABLE_HPP