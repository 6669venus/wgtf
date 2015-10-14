#ifndef BASE_ASSET_OBJECT_ITEM_HPP
#define BASE_ASSET_OBJECT_ITEM_HPP

#include "i_asset_object_item.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

class BaseAssetObjectItem : public IAssetObjectItem
{
public:
	BaseAssetObjectItem( const FileInfo & fileInfo, const IItem * parent, IFileSystem * fileSystem );
	BaseAssetObjectItem( const BaseAssetObjectItem & rhs );
	virtual ~BaseAssetObjectItem();

	BaseAssetObjectItem& operator=( const BaseAssetObjectItem & rhs );

	// File Information
	// TODO: Remove dependency on the file system on low level models in the asset browser.
	// JIRA: http://jira.bigworldtech.com/browse/NGT-1248
	void init( const FileInfo& fileInfo );
	const FileInfo& getFileInfo() const;

	// IItem Implementation
	virtual int columnCount() const override;
	virtual const char* getDisplayText( int column ) const override;
	virtual ThumbnailData getThumbnail( int column ) const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override;

	// IBaseAssetObjectItem Implementation
	virtual const IItem* getParent() const override;
	virtual IItem* operator[]( size_t index ) const override;
	virtual size_t indexOf( const IItem* item ) const override;
	virtual bool empty() const override;
	virtual size_t size() const override;

	// Custom Functions for Basic Asset Data Using FileInfo
	virtual const char* getFileName() const;
	virtual const char* getFullPath() const;
	virtual uint64_t getSize() const;
	virtual uint64_t getCreatedTime() const;
	virtual uint64_t getModifiedTime() const;
	virtual uint64_t getAccessedTime() const;
	virtual bool isDirectory() const;
	virtual bool isReadOnly() const;
	virtual bool isCompressed() const;

private:

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // BASE_ASSET_OBJECT_ITEM_HPP