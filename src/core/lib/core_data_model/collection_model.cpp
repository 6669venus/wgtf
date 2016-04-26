#include "collection_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_data_model/i_item_role.hpp"

namespace
{
	class CollectionItem : public AbstractItem
	{
	public:
		CollectionItem(CollectionModel & model, size_t index)
			: model_(model)
			, index_(index)
		{

		}

		Variant getData(int row, int column, size_t roleId) const override
		{
			auto & collection = model_.getSource();
			if (roleId == ValueTypeRole::roleId_)
			{
				return collection.valueType().getName();
			}
			else if (roleId == KeyTypeRole::roleId_)
			{
				return collection.keyType().getName();
			}

			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return Variant();
			}

			if (roleId == IndexPathRole::roleId_)
			{
				ResizingMemoryStream dataStream;
				TextStream s(dataStream);
				Variant value = it.value();
				s << value;
				return dataStream.takeBuffer();
			}

			if (roleId == ValueRole::roleId_)
			{
				return it.value();
			}
			else if (roleId == KeyRole::roleId_)
			{
				return it.key();
			}

			return Variant();
		}

		bool setData(int row, int column, size_t roleId, const Variant & data) override
		{
			if (roleId != ValueRole::roleId_)
			{
				return false;
			}

			auto & collection = model_.getSource();
			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return false;
			}

			it.setValue(data);
			return true;
		}

	private:
		CollectionModel & model_;
		size_t index_;
	};
}


CollectionModel::CollectionModel()
{
}


CollectionModel::~CollectionModel()
{
}

Connection CollectionModel::connectPreItemDataChanged(AbstractListModel::DataCallback callback) 
{
	return preItemDataChanged_.connect(callback);
}
Connection CollectionModel::connectPostItemDataChanged(AbstractListModel::DataCallback callback) 
{
	return postItemDataChanged_.connect(callback);
}

Connection CollectionModel::connectPreRowsInserted(AbstractListModel::RangeCallback callback) 
{
	return preRowsInserted_.connect(callback);
}
Connection CollectionModel::connectPostRowsInserted(AbstractListModel::RangeCallback callback) 
{
	return postRowsInserted_.connect(callback);
}

Connection CollectionModel::connectPreRowsRemoved(AbstractListModel::RangeCallback callback) 
{
	return preRowsRemoved_.connect(callback);
}
Connection CollectionModel::connectPostRowsRemoved(AbstractListModel::RangeCallback callback) 
{
	return postRowsRemoved_.connect(callback);
}


void CollectionModel::setSource(Collection & collection)
{
	// TODO emit signal
	items_.clear();

	connectPreChange.disconnect();
	connectPostChanged.disconnect();
	connectPreInsert.disconnect();
	connectPostInserted.disconnect();
	connectPreErase.disconnect();
	connectPostErase.disconnect();

	collection_ = collection;
	
	connectPreChange = 
		collection_.connectPreChange((Collection::ElementPreChangeCallback)[=](const Collection::Iterator& pos, const Variant& newValue)
	{
		int row = -1;
		if (pos.key().tryCast<int>(row))
		{
			if (newValue.canCast<AbstractItem>())
			{
				size_t role = ValueTypeRole::roleId_;
				//int row, int column, size_t role, const Variant & value 
				preItemDataChanged_(row, 0, role, newValue);
			}
		}
	});
	
	connectPostChanged = 
		collection_.connectPostChanged((Collection::ElementPostChangedCallback)[=](const Collection::Iterator& pos, const Variant& oldValue)
	{
		int row = -1;
		if (pos.key().tryCast<int>(row))
		{
			size_t role = ValueTypeRole::roleId_;

			//int row, int column, size_t role, const Variant & value 
			postItemDataChanged_(row, 0, role, oldValue);
		}
	});

	connectPreInsert = 
		collection_.connectPreInsert(Collection::ElementRangeCallback([=](const Collection::Iterator& pos, size_t count)
	{
		int row = -1;
		if (pos.key().tryCast<int>(row))
		{
			//int row, int count
			preRowsInserted_(row, (int)count);
		}
	}));

	connectPostInserted =
		collection_.connectPostInserted((Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
	{
		int row = -1;
		if (pos.key().tryCast<int>(row))
		{
			//int row, int count
			postRowsInserted_(row, (int)count);
		}
	});

	connectPreErase = 
		collection_.connectPreErase((Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
	{
		int row = -1;
		if (pos.key().tryCast<int>(row))
		{
			//int row, int count
			preRowsRemoved_(row, (int)count);
		}
	});

	connectPostErase =
		collection_.connectPostErased((Collection::ElementRangeCallback)[=](const Collection::Iterator& pos, size_t count)
	{
		int row = -1;
		if (pos.key().tryCast<int>(row))
		{
			//int row, int count
			postRowsRemoved_(row, (int)count);
		}
	});
}


Collection & CollectionModel::getSource()
{
	return collection_;
}


AbstractItem * CollectionModel::item(int index) const
{
	if (items_.size() <= (size_t)index) 
	{
		items_.resize(index + 1);
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new CollectionItem(*const_cast< CollectionModel * >(this), index);
	items_[index] = std::unique_ptr< AbstractItem >(item);
	return item;
}


int CollectionModel::index(const AbstractItem* item) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert(it != items_.end());
	return index;
}


int CollectionModel::rowCount() const
{
	return (int)collection_.size();
}


int CollectionModel::columnCount() const
{
	return 1;
}
