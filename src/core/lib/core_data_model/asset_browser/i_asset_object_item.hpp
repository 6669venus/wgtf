#ifndef I_ASSET_OBJECT_ITEM_HPP
#define I_ASSET_OBJECT_ITEM_HPP

#include "core_data_model/i_item.hpp"

class IAssetPresentationProvider;

/**
 IAssetObjectItem
 Represents an IItem used by the Asset Browser tree and list models. Contains special functionality for retrieving
 properties of an asset for display. May be overwritten by developers to provide custom functionality for 
 retrieving thumbnails, status icons, and additional data on assets unique to one's studio.
 */
class IAssetObjectItem : public IItem
{
public:
	IAssetObjectItem() {}
	virtual ~IAssetObjectItem() {}
	
	// IItem Methods
	virtual int columnCount() const override = 0;
	virtual const char* getDisplayText( int column ) const override = 0;
	virtual ThumbnailData getThumbnail( int column ) const override = 0;
	virtual Variant getData( int column, size_t roleId ) const override = 0;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override = 0;
	
	// IAssetObjectItem Methods for Tree Model Usage
	virtual const IItem* getParent() const = 0;
	virtual IItem* operator[]( size_t index ) const = 0;
	virtual size_t indexOf( const IItem* item ) const = 0;
	virtual bool empty() const = 0;
	virtual size_t size() const = 0;

	// Retrieves the binary block for a status overlay in the asset browser list
	virtual ThumbnailData getStatus() const = 0;
};

#endif // I_ASSET_OBJECT_ITEM_HPP