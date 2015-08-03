#ifndef FOLDER_CONTENT_OBJECT_MODEL_HPP
#define FOLDER_CONTENT_OBJECT_MODEL_HPP

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

#include <memory>

//------------------------------------------------------------------------------
// FolderContentObjectModel
//
// Defines a single object contained by an asset folder
//------------------------------------------------------------------------------

class FolderContentObjectModel
{
public:

	FolderContentObjectModel();
	FolderContentObjectModel( const FolderContentObjectModel& rhs );
	FolderContentObjectModel( const FileInfo& fileInfo );

	virtual ~FolderContentObjectModel();

	void init( const FileInfo& fileInfo );

	const FileInfo& getFileInfo() const;
	const char* getFileName() const;
	const char* getFullPath() const;
	const char* getThumbnail() const;
	uint64_t getSize() const;
	uint64_t getCreatedTime() const;
	uint64_t getModifiedTime() const;
	uint64_t getAccessedTime() const;
	bool isDirectory() const;
	bool isReadOnly() const;

private:

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // FOLDER_CONTENT_OBJECT_MODEL_HPP