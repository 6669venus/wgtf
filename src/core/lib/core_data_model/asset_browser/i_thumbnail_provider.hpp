#ifndef I_THUMBNAIL_PROVIDER_HPP
#define I_THUMBNAIL_PROVIDER_HPP

#include "core_data_model/i_item.hpp"

class IThumbnailProvider
{
public:
	IThumbnailProvider() {}
	virtual ~IThumbnailProvider() {}

	virtual ThumbnailData fetch() = 0;
};

#endif // I_THUMBNAIL_PROVIDER_HPP