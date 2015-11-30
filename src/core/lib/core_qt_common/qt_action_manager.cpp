#include "qt_action_manager.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_action.hpp"
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
	static const char pathDelimiter = ';';
	QtAction( const char * id,
		std::function<void( IAction* )> & func, 
		std::function<bool( const IAction* )> & enableFunc,
		std::function<bool( const IAction* )> & checkedFunc )
		: text_( id )
		, func_( func )
		, enableFunc_( enableFunc )
		, checkedFunc_( checkedFunc )
		, checkable_( checkedFunc ? true : false )
	{

	}

	QtAction( const char * text,
		const char * icon,
		const char * windowId, 
		const char * path,
		const char * shortcut,
		std::function<void( IAction* )> & func, 
		std::function<bool( const IAction* )> & enableFunc,
		std::function<bool( const IAction* )> & checkedFunc )
		: text_( text )
		, icon_( icon )
		, windowId_( windowId )
		, paths_(StringUtils::split(path, pathDelimiter))
		, shortcut_( shortcut )
		, func_( func )
		, enableFunc_( enableFunc )
		, checkedFunc_( checkedFunc )
		, checkable_( checkedFunc ? true : false )
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

	const std::vector<std::string>& paths() const override
	{
		return paths_;
	}

	const char * shortcut() const override
	{
		return shortcut_.c_str();
	}

	bool enabled() const override
	{
		return enableFunc_( this );
	}

	bool checked() const override
	{
		return checkedFunc_( this );
	}

	bool isCheckable() const override
	{
		return checkable_;
	}
	
	void execute() override
	{
		func_( this );
	}

	virtual void setData( const Variant& data ) override
	{
		data_ = data;
	}

	Variant& getData() override
	{
		return data_;
	}

	const Variant& getData() const override
	{
		return data_;
	}

private:
	std::string text_;
	std::string icon_;
	std::string windowId_;
	std::vector<std::string> paths_;
	std::string shortcut_;
	std::function<void( IAction* )> func_;
	std::function<bool( const IAction* )> enableFunc_;
	std::function<bool( const IAction* )> checkedFunc_;
	Variant data_;
	bool checkable_;
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
		std::string id = localName.toUtf8().operator const char *();
		auto actionData = std::unique_ptr< QtActionData >( new QtActionData );
		auto index = atts.index( "text" );
		if (index >= 0)
		{
			actionData->text_ = atts.value( index ).toUtf8().operator const char *();
		}
		else
		{
			actionData->text_ = id;
		}
		index = atts.index( "icon" );
		if (index >= 0)
		{
			actionData->icon_ = atts.value( index ).toUtf8().operator const char *();
		}
		index = atts.index( "window" );
		if (index >= 0)
		{
			actionData->windowId_ = atts.value( index ).toUtf8().operator const char *();
		}
		index = atts.index( "path" );
		if (index >= 0)
		{
			actionData->path_ = atts.value( index ).toUtf8().operator const char *();
		}
		index = atts.index( "shortcut" );
		if (index >= 0)
		{
			actionData->shortcut_ = atts.value( index ).toUtf8().operator const char *();
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
	std::function<void( IAction* )> func,
	std::function<bool( const IAction* )> enableFunc,
	std::function<bool( const IAction* )> checkedFunc )
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
			enableFunc, checkedFunc ) );
	}
	return std::unique_ptr< IAction >( new QtAction(
		id,	func, enableFunc, checkedFunc ) );
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