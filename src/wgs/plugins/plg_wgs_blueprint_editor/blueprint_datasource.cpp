#include "blueprint_datasource.hpp"
#include "models/blueprint_model.hpp"

BlueprintDataSource::BlueprintDataSource()
	: blueprintPageId_("")
	, blueprintPage_(nullptr)
{
	loadedObj_.clear();
}

BlueprintDataSource::~BlueprintDataSource()
{
}

void BlueprintDataSource::init(IContextManager &contextManager)
{
	auto defManager = contextManager.queryInterface<IDefinitionManager>();
	if (defManager == NULL)
	{
		return;
	}

	if (blueprintPage_ == nullptr)
	{
		blueprintPage_ = defManager->create<BlueprintPage>();
		blueprintPage_->init();
		RefObjectId id;
		bool ok = blueprintPage_.getId(id);
		assert(ok);
		blueprintPageId_ = id.toString();
	}
}

void BlueprintDataSource::fini(IContextManager &contextManager)
{
}

const ObjectHandleT<BlueprintPage> &BlueprintDataSource::getBlueprintPage() const
{
	return blueprintPage_;
}

void BlueprintDataSource::onObjectRegistered(const ObjectHandle &pObj)
{
	RefObjectId id;
	bool ok = pObj.getId(id);
	assert(ok);
	if (id == blueprintPageId_)
	{
		blueprintPage_ = pObj;
	}
	else
	{
		loadedObj_.insert(std::make_pair(id, pObj));
	}
}

void BlueprintDataSource::onObjectDeregistered(const ObjectHandle & obj)
{
	RefObjectId id;
	bool ok = obj.getId(id);
	assert(ok);
	auto findIt = loadedObj_.find(id);
	if (findIt != loadedObj_.end())
	{
		loadedObj_.erase(findIt);
	}
}
