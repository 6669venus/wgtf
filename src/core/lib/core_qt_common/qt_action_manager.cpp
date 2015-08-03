#include "qt_action_manager.hpp"
#include "ui_framework/i_action.hpp"
#include "wg_types/string_ref.hpp"

#include <QFile>
#include <QIODevice>
#include <QXmlSimpleReader>

#include <algorithm>

struct QtActionData
{
	std::string text_;
	std::string icon_;
	std::string windowId_;
	std::string path_;
	std::string shortcut_;
};

class QtAction : public IAction
{
public:
	QtAction( const char * id,
		std::function<void()> & func, 
		std::function<bool()> & enableFunc )
		: text_( id )
		, func_( func )
		, enableFunc_( enableFunc )
	{

	}

	QtAction( const char * text,
		const char * icon,
		const char * windowId, 
		const char * path,
		const char * shortcut,
		std::function<void()> & func, 
		std::function<bool()> & enableFunc )
			: text_( text )
			, icon_( icon )
			, windowId_( windowId )
			, path_( path )
			, shortcut_( shortcut )
			, func_( func )
			, enableFunc_( enableFunc )
	{

	}

	const char * text() const override
	{
		return text_.c_str();
	}

	const char * icon() const override
	{
		return icon_.c_str();
	}

	const char * windowId() const override
	{
		return windowId_.c_str();
	}

	const char * path() const override
	{
		return path_.c_str();
	}

	const char * shortcut() const override
	{
		return shortcut_.c_str();
	}

	bool enabled() const override
	{
		return enableFunc_();
	}
	
	void execute() override
	{
		func_();
	}

private:
	std::string text_;
	std::string icon_;
	std::string windowId_;
	std::string path_;
	std::string shortcut_;
	std::function<void()> func_;
	std::function<bool()> enableFunc_;
};

class QtActionContentHandler : public QXmlDefaultHandler
{
public:
	QtActionContentHandler( QtActionManager & actionManager ) 
		: actionManager_( actionManager ) {}
	~QtActionContentHandler() {}

	bool startElement( const QString & namespaceURI, const QString & localName,
		const QString & qName, const QXmlAttributes & atts )
	{
		std::string id = localName.toUtf8();
		auto actionData = std::unique_ptr< QtActionData >( new QtActionData );
		auto index = atts.index( "text" );
		if (index >= 0)
		{
			actionData->text_ = atts.value( index ).toUtf8();
		}
		else
		{
			actionData->text_ = id;
		}
		index = atts.index( "icon" );
		if (index >= 0)
		{
			actionData->icon_ = atts.value( index ).toUtf8();
		}
		index = atts.index( "window" );
		if (index >= 0)
		{
			actionData->windowId_ = atts.value( index ).toUtf8();
		}
		index = atts.index( "path" );
		if (index >= 0)
		{
			actionData->path_ = atts.value( index ).toUtf8();
		}
		index = atts.index( "shortcut" );
		if (index >= 0)
		{
			actionData->shortcut_ = atts.value( index ).toUtf8();
		}

		actionManager_.registerActionData( id.c_str(), actionData );
		return true;
	}

private:
	QtActionManager & actionManager_;
};

QtActionManager::QtActionManager()
{

}

QtActionManager::~QtActionManager()
{

}

std::unique_ptr< IAction > QtActionManager::createAction( 
	const char * id,
	std::function<void()> func,
	std::function<bool()> enableFunc )
{
	auto it = actionData_.find( id );
	if (it != actionData_.end())
	{
		auto & actionData = *it->second;
		return std::unique_ptr< IAction >( new QtAction(
			actionData.text_.c_str(),
			actionData.icon_.c_str(),
			actionData.windowId_.c_str(),
			actionData.path_.c_str(),
			actionData.shortcut_.c_str(),
			func,
			enableFunc ) );
	}

	return std::unique_ptr< IAction >( new QtAction(
		id,	func, enableFunc ) );
}

void QtActionManager::loadActionData( QIODevice & source )
{
	QXmlSimpleReader actions;
	QtActionContentHandler handler( *this );
	actions.setContentHandler( &handler );
	actions.parse( QXmlInputSource( &source ) );
}

bool QtActionManager::registerActionData( const char * id, 
	std::unique_ptr< QtActionData > & actionData )
{
	auto it = actionData_.find( id );
	if (it != actionData_.end())
	{
		return false;
	}

	actionData_[ id ] = std::unique_ptr< QtActionData >( actionData.release() );
	return true;
}