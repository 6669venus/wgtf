#include "scoped_disable_command_recording.hpp"


#include <cassert>
#include <limits>


namespace {
	static __declspec( thread ) int s_entryCount_ = 0;
} // namespace


ScopedDisableCommandRecording::ScopedDisableCommandRecording()
{
	assert( s_entryCount_ < std::numeric_limits< int >::max() );
	++s_entryCount_;
}


ScopedDisableCommandRecording::~ScopedDisableCommandRecording()
{
	--s_entryCount_;
	assert( s_entryCount_ >= 0 );
}


bool ScopedDisableCommandRecording::isIgnored()
{
	return (s_entryCount_ > 0 );
}
