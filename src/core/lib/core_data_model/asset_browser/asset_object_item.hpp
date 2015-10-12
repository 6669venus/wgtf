#ifndef ASSET_OBJECT_ITEM_HPP
#define ASSET_OBJECT_ITEM_HPP

#include "core_data_model/i_item.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

class AssetObjectItem : public IItem
{
public:
	AssetObjectItem();
	AssetObjectItem( const AssetObjectItem & rhs );
	AssetObjectItem( const FileInfo & fileInfo );
	virtual ~AssetObjectItem();

	AssetObjectItem& operator=( const AssetObjectItem & rhs );

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

	// Custom Functions for Asset Data
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

#endif // ASSET_OBJECT_ITEM_HPP