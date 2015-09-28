#ifndef FILE_OBJECT_ITEM_HPP
#define FILE_OBJECT_ITEM_HPP

#include "../i_item.hpp"

class IAssetObjectModel;

class FileObjectItem : public IItem
{
public:
	FileObjectItem( IAssetObjectModel & model );
	FileObjectItem( const FileObjectItem& rhs );
	virtual ~FileObjectItem();

	FileObjectItem& operator=( const FileObjectItem& rhs );

	virtual int columnCount() const override;
	virtual const char* getDisplayText( int column ) const override;
	virtual ThumbnailData getThumbnail( int column ) const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // FILE_OBJECT_ITEM_HPP