#ifndef FILE_SYSTEM_MODEL_HPP
#define FILE_SYSTEM_MODEL_HPP

#include "core_data_model/i_tree_model.hpp"

#include <memory>

class IFileSystem;

class FileSystemModel : public ITreeModel
{
public:
	FileSystemModel( IFileSystem & fileSystem, const char * rootDirectory );
	~FileSystemModel();

	virtual IItem * item(size_t index, const IItem * parent) const;

	virtual ItemIndex index(const IItem * item) const;

	virtual bool empty(const IItem * item) const;

	virtual size_t size(const IItem * item) const;

	virtual int columnCount() const;

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};

#endif//FILE_SYSTEM_MODEL_HPP