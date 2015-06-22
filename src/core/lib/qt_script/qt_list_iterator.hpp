#ifndef QT_LIST_ITERATOR_HPP
#define QT_LIST_ITERATOR_HPP

#include <QObject>
#include <QVariant>

class IListModel;
class IItem;

class QtListIterator : public QObject
{
	Q_OBJECT

	Q_PROPERTY(
		QVariant current
		READ getCurrent
		WRITE setCurrent )

public:
	QtListIterator( IListModel & listModel );
	virtual ~QtListIterator();

	Q_INVOKABLE void reset();
	Q_INVOKABLE bool moveNext();

private:
	QVariant getCurrent() const;
	void setCurrent( QVariant & value );

private:
	IListModel & listModel_;
	size_t size_;
	size_t currentIndex_;
	IItem * currentItem_;
};

#endif