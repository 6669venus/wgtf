#include "cstdmf/debug_filter.hpp"

#include "third_party/CppUnitLite2/src/TestException.h"

#include "cstdmf/debug_message_callbacks.hpp"

BW_BEGIN_NAMESPACE

	
template < class T >
std::ostream& operator << ( std::ostream& os, const BW::vector< T >& vec )
{
	std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os, ", "));
	return os;
}

template < class K, class T >
std::ostream& operator << ( std::ostream& os, const BW::map< K, T >& m )
{
	for (BW::map<K, T>::const_iterator iter = m.begin(), end = m.end(); iter != end; ++iter)
	{
		os << iter->first << ": " << iter->second << std::endl;
	}
	return os;
}


// Provides the ability to test for MF_ASSERT's being triggered in the code
class ThrowTestExceptionOnAssert
{
private:
	class TestAssertThrower : public CriticalMessageCallback
	{
	public:
		virtual void handleCritical( const char * message )
		{
			throw ::TestException("", 0, message);
		}
	};

	TestAssertThrower thrower_;

public:
	ThrowTestExceptionOnAssert()
	{
		BW::DebugFilter::instance().addCriticalCallback(&thrower_);
	}

	~ThrowTestExceptionOnAssert()
	{
		BW::DebugFilter::instance().deleteCriticalCallback(&thrower_);
	}
};

#define ENABLE_ASSERT_EXCEPTIONS \
	//ThrowTestExceptionOnAssert _throwAss

BW_END_NAMESPACE
