#ifndef LOGGING_HPP
#define LOGGING_HPP

// NGT-276 TODO: Replace with a logging plugin instead

#define NGT_MSG( format, ... ) \
NGT::logMessage( format, ## __VA_ARGS__ )

#define NGT_TRACE_MSG( format, ... ) \
NGT::logMessage( "%s, %d: ", __FILE__, __LINE__ ); \
NGT::logMessage( format, ## __VA_ARGS__ )

#define NGT_DEBUG_MSG( format, ... ) \
NGT::logMessage( "%s, %d: ", __FILE__, __LINE__ ); \
NGT::logMessage( format, ## __VA_ARGS__ )


#define NGT_WARNING_MSG( format, ... ) \
NGT::logMessage( "%s, %d: ", __FILE__, __LINE__ ); \
NGT::logMessage( format, ## __VA_ARGS__ )


#define NGT_ERROR_MSG( format, ... ) \
NGT::logMessage( "%s, %d: ", __FILE__, __LINE__ ); \
NGT::logMessage( format, ## __VA_ARGS__ )


namespace NGT
{

int logMessage( const char* format, ... );

} // namespace NGT


#endif // LOGGING_HPP
