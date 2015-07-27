//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_asset_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_ASSET_MODEL_H_
#define I_ASSET_MODEL_H_

#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "data_model/i_tree_model.hpp"
#include "data_model/i_item.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/object_handle.hpp"
#include "serialization/interfaces/i_file_system.hpp"

#include <memory>

//------------------------------------------------------------------------------
// IAssetObjectModel
//
// Defines a single asset object model
//------------------------------------------------------------------------------

class IAssetObjectModel
{
	DECLARE_REFLECTED
public:
	virtual ~IAssetObjectModel(){}
	virtual const char* getFileName() const { return nullptr; }
	virtual const char* getFullPath() const { return nullptr; }
	virtual const char* getThumbnail() const { return nullptr; }
	virtual uint64_t getSize() const { return 0; }
	virtual uint64_t getCreatedTime() const { return 0; }
	virtual uint64_t getModifiedTime() const { return 0; }
	virtual uint64_t getAccessedTime() const { return 0; }
	virtual bool isDirectory() const { return false; }
	virtual bool isReadOnly() const { return false; }
};

#endif // I_ASSET_MODEL_H_