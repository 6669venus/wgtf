#ifndef I_ASSET_PRESENTATION_PROVIDER_HPP
#define I_ASSET_PRESENTATION_PROVIDER_HPP

#include "core_data_model/i_item.hpp"

class IAssetObjectItem;

class IAssetPresentationProvider
{
public:
	IAssetPresentationProvider() {}
	virtual ~IAssetPresentationProvider() {}

	virtual ThumbnailData getThumbnail( const IAssetObjectItem * asset ) = 0;
	virtual const char* getTypeIconResourceString( const IAssetObjectItem * asset ) = 0;
};

#endif // I_ASSET_PRESENTATION_PROVIDER_HPP