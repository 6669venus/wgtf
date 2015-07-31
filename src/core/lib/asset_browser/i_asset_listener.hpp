#ifndef I_ASSET_LISTENER_HPP
#define I_ASSET_LISTENER_HPP

class IAssetListener
{
public:

	IAssetListener() {}
	virtual ~IAssetListener() {}

	virtual void useAsset( const char* assetPath ) = 0;
};

#endif // I_ASSET_LISTENER_HPP