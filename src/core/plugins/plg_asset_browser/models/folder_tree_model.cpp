#include "folder_tree_model.hpp"
#include "folder_tree_item.hpp"

struct StringList
{
	StringList()
	{
		position = 0;
		listOfData = "Animations animations_01 animations_02 animations_03 animation_04 animations_05 ";
		listOfData += "Models models_01 models_02 models_03 models_04 models_05 ";
		listOfData += "Objects objects_01 objects_02 objects_03 objects_04 objects_05 ";
		listOfData += "Skins skins_01 skins_02 skins_03 skins_04 skins_05 ";
		listOfData += "Terrain terrain_01 terrain_02 terrain_03 terrain_04 terrain_05 ";
	}

	std::string next()
	{
		size_t nextPosition = listOfData.find( ' ', position );
		size_t count = nextPosition == std::string::npos ?
			std::string::npos : nextPosition - position;
		std::string temp = listOfData.substr( position, count );
		position = nextPosition == std::string::npos ? 0 : nextPosition + 1;
		return temp;
	}

	std::string listOfData;
	size_t position;
};

struct FolderTreeModel::Implementation
{
	Implementation( FolderTreeModel& main );
	~Implementation();

	std::vector<FolderTreeItem*> getSection( const FolderTreeItem* parent );
	char* copyString( const std::string& s ) const;
	void generateData( const FolderTreeItem* parent, size_t level );

	FolderTreeModel& main_;
	std::unordered_map<const FolderTreeItem*, std::vector<FolderTreeItem*>> data_;
	StringList dataSource_;

	static const size_t NUMBER_OF_GROUPS = 5;
	static const size_t NUMBER_OF_LEVELS = 1;
};

FolderTreeModel::Implementation::Implementation( FolderTreeModel& main )
	: main_( main )
{
	generateData( nullptr, 0 );
}

FolderTreeModel::Implementation::~Implementation()
{
	for (auto itr = data_.begin(); itr != data_.end(); ++itr)
	{
		auto items = itr->second;
		size_t max = items.size();

		for (size_t i = 0; i < max; ++i)
		{
			delete items[i];
		}
	}

	data_.clear();
}

std::vector<FolderTreeItem*> FolderTreeModel::Implementation::getSection(
	const FolderTreeItem* parent )
{
	auto itr = data_.lower_bound( parent );
	assert( itr != data_.end() );
	return itr->second;
}

char* FolderTreeModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}

void FolderTreeModel::Implementation::generateData(
	const FolderTreeItem* parent, size_t level )
{
	for (size_t i = 0; i < NUMBER_OF_GROUPS; ++i)
	{
		std::string dataString = dataSource_.next();
		FolderTreeItem* item = new FolderTreeItem(
			copyString( dataString ), parent );
		data_[parent].push_back( item );

		if (level < NUMBER_OF_LEVELS)
		{
			generateData( item, level + 1 );
		}

		data_[item];
	}
}


FolderTreeModel::FolderTreeModel()
	: impl_( new Implementation( *this ) )
{
}

FolderTreeModel::FolderTreeModel( const FolderTreeModel& rhs )
	: impl_( new Implementation( *this ) )
{
}

FolderTreeModel::~FolderTreeModel()
{
}

FolderTreeModel& FolderTreeModel::operator=( const FolderTreeModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this ) );
	}

	return *this;
}

IItem* FolderTreeModel::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const FolderTreeItem*>( parent );
	return impl_->getSection( temp )[index];
}

ITreeModel::ItemIndex FolderTreeModel::index( const IItem* item ) const
{
	auto temp = static_cast<const FolderTreeItem*>( item );
	temp = static_cast<const FolderTreeItem*>( temp->getParent() );
	ItemIndex index( 0, temp );

	auto items = impl_->getSection( temp );
	auto itr = std::find( items.begin(), items.end(), item );
	assert( itr != items.end() );

	index.first = itr - items.begin();
	return index;
}


bool FolderTreeModel::empty( const IItem* parent ) const
{
	const auto temp = static_cast< const FolderTreeItem* >( parent );
	return impl_->getSection( temp ).empty();
}


size_t FolderTreeModel::size( const IItem* parent ) const
{
	auto temp = static_cast<const FolderTreeItem*>( parent );
	return impl_->getSection( temp ).size();
}
