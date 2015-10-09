//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_asset_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_ASSET_MODEL_H_
#define I_ASSET_MODEL_H_

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_serialization/interfaces/i_file_system.hpp"

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
	virtual bool isCompressed() const { return false; }
};

#endif // I_ASSET_MODEL_H_