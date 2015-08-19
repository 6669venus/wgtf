#ifndef DATA_CHANGE_NOTIFIER_HPP
#define DATA_CHANGE_NOTIFIER_HPP


#include "core_data_model/i_value_change_notifier.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include <QtCore>


/**
 *	Qt interface for dataAboutToBeChanged() and dataChanged() events.
 */
class DataChangeNotifier
	: public QObject
{
	Q_OBJECT

	Q_PROPERTY( QVariant source
		READ getSource
		WRITE setSource
		NOTIFY sourceChanged )
	Q_PROPERTY( QVariant data
		READ getData
		WRITE setData
		NOTIFY dataChanged )
	Q_CLASSINFO( "DefaultProperty", "data" )

	DECLARE_QT_MEMORY_HANDLER

public:
	typedef IValueChangeNotifier SourceType;

	DataChangeNotifier();
	virtual ~DataChangeNotifier();

	void source( SourceType* selectionSource );
	const SourceType* source() const;

private:
	QVariant getSource() const;
	bool setSource( const QVariant& source );

	QVariant getData() const;
	bool setData( const QVariant& value );

	void onPreDataChanged( const SourceType* sender,
		const SourceType::PreDataChangedArgs& args );
	void onPostDataChanged( const SourceType* sender,
		const SourceType::PostDataChangedArgs& args );
	void onPreNofifierDestroyed(const SourceType* sender,
		const SourceType::PreNotifierDestroyedArgs& args);

signals:
	void sourceChanged();
	void dataAboutToBeChanged();
	void dataChanged();

private:
	SourceType* source_;
};


#endif // DATA_CHANGE_NOTIFIER_HPP

