#ifndef ALERT_MODELS_HPP
#define ALERT_MODELS_HPP

#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/object_handle.hpp"

class AlertObjectModel
{
public:

	AlertObjectModel();
	AlertObjectModel( const AlertObjectModel& rhs );
	AlertObjectModel( const char* message );

	virtual ~AlertObjectModel();

	void init( const char* message );

	const char* getMessage() const;

private:

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class AlertPageModel
{
public:

	AlertPageModel();
	AlertPageModel( const AlertPageModel& rhs );

	virtual ~AlertPageModel();

	void init( IContextManager& contextManager );

	void addAlert( const char* message );

	ObjectHandle removeAlert() const;

	const int & currentSelectedRowIndex() const;
	void currentSelectedRowIndex( const int & index );

private:

	DECLARE_REFLECTED

	ObjectHandle getAlerts() const;

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // ALERT_MODELS_HPP