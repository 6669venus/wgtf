#include "wg_action.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_action.hpp"
#include "helpers/qt_helpers.hpp"

#include <QQmlEngine>
#include <QQmlContext>

struct WGAction::Implementation
{
	Implementation()
		: uiApplication_( nullptr )
		, uiFramework_( nullptr )
		, func_( nullptr )
		, active_( false )
	{
	}

	~Implementation()
	{
		destroyAction();
	}

	void onComponentComplete( WGAction * action )
	{
		func_ = [action](IAction*){ emit action->triggered(); };

		auto context = QQmlEngine::contextForObject( action );
		assert( context != nullptr );

		auto componentContextProperty = context->contextProperty( "componentContext" );
		assert( componentContextProperty.isValid() );
		auto componentContextVariant = QtHelpers::toVariant( componentContextProperty );
		assert( !componentContextVariant.isVoid() );
		auto componentContextHandle = componentContextVariant.value< ObjectHandle >();
		assert( componentContextHandle != nullptr );
		auto componentContext = componentContextHandle.getBase< IComponentContext >();
		assert( componentContext != nullptr );

		uiApplication_ = componentContext->queryInterface< IUIApplication >();
		uiFramework_ = componentContext->queryInterface< IUIFramework >();

		createAction();
	}

	bool getActive() const
	{
		return active_;
	}

	void setActive( bool active )
	{
		if (active_ == active)
		{
			return;
		}

		active_ = active;

		if (uiApplication_ == nullptr || action_ == nullptr)
		{
			return;
		}

		active_ ? uiApplication_->addAction( *action_ ) : uiApplication_->removeAction( *action_ );
	}

	QString getActionId() const
	{
		return actionId_.c_str();
	}

	void setActionId( const QString& actionId )
	{
		destroyAction();
		actionId_ = actionId.toUtf8().data();
		createAction();
	}

	void createAction()
	{
		if (actionId_.empty())
		{
			return;
		}

		if (uiFramework_ == nullptr)
		{
			return;
		}

		action_ = uiFramework_->createAction( actionId_.c_str(), func_ );
		
		if (uiApplication_ == nullptr || active_ == false)
		{
			return;
		}

		uiApplication_->addAction( *action_ );
	}

	void destroyAction()
	{
		if (action_ == nullptr)
		{
			return;
		}

		if (uiApplication_ != nullptr && active_)
		{
			uiApplication_->removeAction( *action_ );
		}

		action_.reset();
	}

	IUIFramework * uiFramework_;
	IUIApplication * uiApplication_;
	std::function<void(IAction*)> func_;
	bool active_;
	std::string actionId_;
	std::unique_ptr< IAction > action_;
};

WGAction::WGAction( QQuickItem * parent )
	: QQuickItem( parent )
{
	impl_.reset( new Implementation() );
}


WGAction::~WGAction()
{
}


void WGAction::componentComplete()
{
	QQuickItem::componentComplete();
	impl_->onComponentComplete( this );
}


bool WGAction::getActive() const
{
	return impl_->getActive();
}


void WGAction::setActive( bool active )
{
	impl_->setActive( active );
}


QString WGAction::getActionId() const
{
	return impl_->getActionId();
}


void WGAction::setActionId( const QString& actionId )
{
	impl_->setActionId( actionId );
}