#pragma once
#ifndef _QT_COLLECTION_MODEL_HPP
#define _QT_COLLECTION_MODEL_HPP

#include "qt_item_model.hpp"


class CollectionModel;


/**
 *	Provides QML interface to use a CollectionModel as a list model.
 *	Can access items by key as well as by row index.
 */
class QtCollectionModel : public QtListModel
{
	Q_OBJECT

public:
	QtCollectionModel( CollectionModel & source );

	/**
	 *	Get CollectionModel that is being adapted to be used by Qt.
	 */
	const CollectionModel & source() const;

	/**
	 *	Get item from collection.
	 *	@param key may be index or map key.
	 *	@return found item or nullptr if:
	 *		Key type does not match the source collection's key type.
	 *		Key cannot be found in collection.
	 */
	Q_INVOKABLE QObject * item( const QVariant & key ) const;

	/**
	 *	Add a new item using the given key.
	 *	
	 *	@pre key must be an index type, cannot be a key into a map.
	 *	
	 *	@param key the position to insert the new item.
	 *		Inserts the item *before* the given key.
	 *		If the key is 0, the new item is added to the start.
	 *		If key is count(), the new item is added to the end.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool insertItem( const QVariant & key );

	/**
	 *	Remove an item using the given key.
	 *	
	 *	@pre key must be an index type, cannot be a key into a map.
	 *	
	 *	@param key the position to insert the new item.
	 *		Inserts the item *before* the given key.
	 *		If the key is 0, the new item is added to the start.
	 *		If key is count(), the new item is added to the end.
	 *	@return true on success.
	 */
	Q_INVOKABLE bool removeItem( const QVariant & key );
};


#endif // _QT_COLLECTION_MODEL_HPP

