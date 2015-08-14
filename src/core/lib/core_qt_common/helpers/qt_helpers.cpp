#include "qt_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/i_qt_type_converter.hpp"

#include <set>

#include <QQmlEngine>
#include <QQuickItem>
#include <QWindow>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "core_variant/variant.hpp"

namespace QtHelpers
{

static IQtFramework * s_qtFramework = nullptr;

//==============================================================================
QVariant toQVariant( const Variant & variant )
{
	if (s_qtFramework == nullptr)
	{
		s_qtFramework = Context::queryInterface< IQtFramework >();
	}
	if (s_qtFramework != nullptr)
	{
		return s_qtFramework->toQVariant( variant );
	}

	return QVariant( QVariant::Invalid );
}

//==============================================================================
QVariant toQVariant( const ObjectHandle & object )
{
	return toQVariant( Variant( object ) );
}


//==============================================================================
Variant toVariant( const QVariant & qVariant )
{
	Variant variant;

	if (s_qtFramework == nullptr)
	{
		s_qtFramework = Context::queryInterface< IQtFramework >();
	}
	if (s_qtFramework != nullptr)
	{
		return s_qtFramework->toVariant( qVariant );
	}

	return Variant();
}


//==============================================================================
QQuickItem * findChildByObjectName( QObject * parent, const char * controlName )
{
	std::list< QObject * > queue;
	queue.push_back( parent );

	QString targetName( controlName );

	std::set< QObject * > visited;
	while (queue.empty() == false)
	{
		QQuickItem * child = qobject_cast< QQuickItem * >( queue.front() );
		if (child == NULL)
		{
			QWindow * window = qobject_cast< QWindow * >( queue.front() );
			queue.pop_front();
			if (window == NULL)
			{
				continue;
			}
			const QObjectList & children = window->children();
			for( QObjectList::const_iterator it = children.begin();
				it != children.end(); ++it )
			{
				std::pair< std::set< QObject * >::iterator, bool > insertIt =
					visited.insert( *it );
				if( insertIt.second )
				{
					queue.push_back( *it );
				}
			}
			continue;
		}
		if (targetName == child->objectName())
		{
			return child;
		}
		queue.pop_front();

		QList< QQuickItem * > children = child->childItems();
		for( QList< QQuickItem * >::const_iterator it = children.begin();
			it != children.end(); ++it )
		{
			std::pair< std::set< QObject * >::iterator, bool > insertIt =
				visited.insert( *it );
			if( insertIt.second )
			{
				queue.push_back( *it );
			}
		}
	}
	return NULL;
}


QUrl resolveQmlPath( const QQmlEngine & qmlEngine, const std::string & relativePath )
{
	QStringList paths = qmlEngine.importPathList();

	QUrl url;
	for (auto path : paths)
	{
		QFileInfo info( QDir( path ), relativePath.c_str() );
		if (info.exists() && info.isFile())
		{
			url = QUrl::fromLocalFile( info.canonicalFilePath() );
			break;
		}
	}

	//fallback to qrc
	if (url.isEmpty())
	{
		url.setScheme( "qrc" );
		url.setPath( relativePath.c_str() );
	}

	return url;
}

};