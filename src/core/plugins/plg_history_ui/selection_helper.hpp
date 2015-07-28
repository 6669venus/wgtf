#ifndef SELECTION_HELPER_HPP
#define SELECTION_HELPER_HPP

#include <QObject>
#include <QVariant>
#include "qt_common/qt_new_handler.hpp"

class HistorySelectionHandler;

class SelectionHelper
    : public QObject
{
    Q_OBJECT

	Q_PROPERTY( QVariant source
	READ getSource
	WRITE setSource
	NOTIFY sourceChanged )
	Q_INVOKABLE void select( const QVariant & data );

	DECLARE_QT_MEMORY_HANDLER
public:
	typedef HistorySelectionHandler SourceType;


    SelectionHelper( QObject * parent = nullptr );
    ~SelectionHelper();

	void source( SourceType* selectionSource );
	const SourceType* source() const;

private:
	QVariant getSource() const;
	bool setSource( const QVariant& source );

signals:
	void sourceChanged();

private:
	SourceType* source_;

};


#endif //SELECTION_HELPER_HPP
