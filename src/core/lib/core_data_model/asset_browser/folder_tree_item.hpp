#ifndef FOLDER_TREE_ITEM_HPP
#define FOLDER_TREE_ITEM_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

#include <memory>

//------------------------------------------------------------------------------
// FolderTreeItem
//
// Represents a single item in the asset browser tree view
//------------------------------------------------------------------------------

class FolderTreeItem: public IItem
{
public:
	FolderTreeItem( const FileInfo& fileInfo, const IItem* parent );
	FolderTreeItem( const FolderTreeItem& rhs );
	virtual ~FolderTreeItem();

	FolderTreeItem& operator=( const FolderTreeItem& rhs );

	const IItem* getParent() const;
	const FileInfo& getFileInfo() const;
	virtual int columnCount() const override;
	virtual const char* getDisplayText( int column ) const override;
	virtual ThumbnailData getThumbnail( int column ) const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // FOLDER_TREE_ITEM_HPP