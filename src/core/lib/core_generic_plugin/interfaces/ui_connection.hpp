#ifndef UI_CONNECTION_HPP
#define UI_CONNECTION_HPP

#include <memory>

class UIConnection
{
public:
	UIConnection()
		: pImpl_( nullptr )
	{
	}

	UIConnection( UIConnection * pImpl )
		: pImpl_( pImpl )
	{
	}

	UIConnection( UIConnection && other ) : 
		pImpl_( std::move(other.pImpl_) )
	{
	}

	UIConnection & operator= ( UIConnection && other )
	{
		pImpl_ = std::move(other.pImpl_);
		return *this;
	}

	virtual ~UIConnection() {}

	UIConnection * getImpl() { return pImpl_.get(); }

private:
	UIConnection( const UIConnection & );
	UIConnection& operator= ( const UIConnection& );

	std::unique_ptr< UIConnection > pImpl_;
};

#endif //UI_CONNECTION_HPP