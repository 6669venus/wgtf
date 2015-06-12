#ifndef FOLDER_CONTENT_OBJECT_MODEL_HPP
#define FOLDER_CONTENT_OBJECT_MODEL_HPP

#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "data_model/i_tree_model.hpp"
#include "data_model/i_item.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/object_handle.hpp"

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
	FolderContentObjectModel( const char* fileName, const char* thumbnail );

	virtual ~FolderContentObjectModel();

	void init( const char* fileName, const char* thumbnail );

	const char* getFileName() const;
	const char* getThumbnail() const;

private:

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // FOLDER_CONTENT_OBJECT_MODEL_HPP