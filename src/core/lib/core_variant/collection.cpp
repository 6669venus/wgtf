#include "collection.hpp"
#include <cassert>

Collection::ConstIterator& Collection::ConstIterator::operator++()
{
	detach();
	impl_->inc();
	return *this;
}


bool Collection::ConstIterator::operator==(const Collection::ConstIterator& that) const
{
	if(impl_ && that.impl_)
	{
		return impl_->equals(*that.impl_);
	}
	else
	{
		return impl_.get() == that.impl_.get();
	}
}


void Collection::ConstIterator::detach()
{
	if(impl_ && !impl_.unique())
	{
		impl_ = impl_->clone();
	}
}


//------------------------------------------------------------------------------
bool Collection::isValid() const
{
	return impl_.get() != nullptr;
}


//------------------------------------------------------------------------------
const TypeId& Collection::keyType() const
{
	return impl_->keyType();
}



const TypeId& Collection::valueType() const
{
	return impl_->valueType();
}


bool Collection::isSame( const void* container ) const
{
	return
		impl_ &&
		impl_->containerData() == container;
}


bool Collection::empty() const
{
	if (impl_)
	{
		return impl_->empty();
	}
	else
	{
		return true;
	}
}


size_t Collection::size() const
{
	if(impl_)
	{
		return impl_->size();
	}
	else
	{
		return 0;
	}
}


Collection::Iterator Collection::begin()
{
	if(impl_)
	{
		return impl_->begin();
	}
	else
	{
		return Iterator();
	}
}


Collection::ConstIterator Collection::begin() const
{
	if(impl_)
	{
		return impl_->begin();
	}
	else
	{
		return Iterator();
	}
}


Collection::Iterator Collection::end()
{
	if(impl_)
	{
		return impl_->end();
	}
	else
	{
		return Iterator();
	}
}


Collection::ConstIterator Collection::end() const
{
	if(impl_)
	{
		return impl_->end();
	}
	else
	{
		return Iterator();
	}
}


Collection::Iterator Collection::find(const Variant& key)
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_EXISTING).first;
	}
	else
	{
		return Iterator();
	}
}


Collection::ConstIterator Collection::find(const Variant& key) const
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_EXISTING).first;
	}
	else
	{
		return ConstIterator();
	}
}


Collection::Iterator Collection::insert(const Variant& key)
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_NEW).first;
	}
	else
	{
		return Iterator();
	}
}


std::pair<Collection::Iterator, bool> Collection::get(const Variant& key)
{
	if(impl_)
	{
		return impl_->get(key, CollectionImplBase::GET_AUTO);
	}
	else
	{
		return std::pair<Iterator, bool>(Iterator(), false);
	}
}


Collection::Iterator Collection::erase(const Iterator& pos)
{
	if(impl_)
	{
		return impl_->erase(pos.impl());
	}
	else
	{
		return Iterator();
	}
}


size_t Collection::erase(const Variant& key)
{
	if(impl_)
	{
		return impl_->erase(key);
	}
	else
	{
		return 0;
	}
}


Collection::Iterator Collection::erase(const Iterator& first, const Iterator& last)
{
	if(impl_)
	{
		return impl_->erase(first.impl(), last.impl());
	}
	else
	{
		return Iterator();
	}
}


Collection::ValueRef Collection::operator[](const Variant& key)
{
	assert(impl_);

	return impl_->get(key, CollectionImplBase::GET_AUTO).first;
}


const Variant Collection::operator[](const Variant& key) const
{
	assert(impl_);

	return impl_->get(key, CollectionImplBase::GET_EXISTING).first->value();
}


bool Collection::operator==(const Collection& that) const
{
	if (impl_ == that.impl_)
	{
		return true;
	}

	if (impl_ == nullptr || that.impl_ == nullptr)
	{
		return false;
	}

	auto size = that.impl_->size();
	if (impl_->size() != size)
	{
		return false;
	}

	auto thisIt = impl_->begin();
	auto thatIt = that.impl_->begin();
	for (size_t i = 0; i < size; ++i)
	{
		if (thisIt->key() != thatIt->key() ||
			thisIt->value() != thatIt->value())
		{
			return false;
		}

		thisIt->inc();
		thatIt->inc();
	}

	return true;
}


