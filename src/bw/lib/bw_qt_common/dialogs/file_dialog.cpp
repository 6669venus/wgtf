#include "file_dialog.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QStringBuilder>

#include "folder_guard.hpp"

#include <windows.h>

#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

namespace
{
	QFileDialog * s_fileDialogImpl = NULL;
}

//==============================================================================
FolderSetter::FolderSetter()
{
	GetCurrentDirectory( ARRAY_SIZE( envFolder_ ), envFolder_ );
	GetCurrentDirectory( ARRAY_SIZE( curFolder_ ), curFolder_ );
}


//==============================================================================
void FolderSetter::enter()
{
	GetCurrentDirectory( ARRAY_SIZE( envFolder_ ), envFolder_ );
	SetCurrentDirectory( curFolder_ );
}


//==============================================================================
void FolderSetter::leave()
{
	GetCurrentDirectory( ARRAY_SIZE( curFolder_ ), curFolder_ );
	SetCurrentDirectory( envFolder_ );
}


//==============================================================================
FolderGuard::FolderGuard()
{
	setter_.enter();
}


//==============================================================================
FolderGuard::~FolderGuard()
{
	setter_.leave();
}


//==============================================================================
BWFileDialog::BWFileDialog(
	bool openFileDialog, const wchar_t * defaultExt,
	const wchar_t * initialFileName,
	FDFlags flags, const wchar_t * filter, void * parentWnd )
{
	s_fileDialogImpl = 
			new QFileDialog( ( QWidget * ) parentWnd );
	s_fileDialogImpl->setAcceptMode(
		openFileDialog ? QFileDialog::AcceptOpen : QFileDialog::AcceptSave );

	QFileDialog::Options options = 
		( flags & FD_HIDEREADONLY	? QFileDialog::ReadOnly : 0 ) |
		( flags & FD_OVERWRITEPROMPT ? 0 : QFileDialog::DontConfirmOverwrite );
	s_fileDialogImpl->setOptions( options );

	QFileDialog::FileMode fileMode = s_fileDialogImpl->fileMode();
	if (flags & FD_PATHMUSTEXIST)
	{
		fileMode = QFileDialog::Directory;
	}
	if (flags & FD_FILEMUSTEXIST)
	{
		fileMode = QFileDialog::ExistingFile;
	}
	if (flags & FD_ALLOWMULTISELECT)
	{
		fileMode = QFileDialog::ExistingFiles;
	}
	s_fileDialogImpl->setFileMode( fileMode );
	QString nameFilter = QString::fromWCharArray( filter );
	QString builtString;
	QStringList stringList = nameFilter.split( '|' );
	for(	QStringList::const_iterator it = stringList.constBegin();
			it != stringList.constEnd(); ++it )
	{
		if ( it->length() == 0 )
		{
			++it;
			continue;
		}
		if (it != stringList.constBegin())
		{
			builtString = builtString % ";;";
		}
		builtString = builtString % *it;
		++it;
	}
	s_fileDialogImpl->setNameFilter( builtString );
}


//==============================================================================
BWFileDialog::~BWFileDialog()
{
	delete s_fileDialogImpl;
	s_fileDialogImpl = NULL;
}

//==============================================================================
bool BWFileDialog::showDialog()
{
	return s_fileDialogImpl->exec();
}

//==============================================================================
void BWFileDialog::initialDir( const wchar_t * initialDir )
{
	s_fileDialogImpl->setDirectory( QString::fromWCharArray( initialDir ) );
}


//==============================================================================
std::wstring BWFileDialog::getFileName() const
{
	return getFileNames()[ 0 ];
}

//==============================================================================
std::vector< std::wstring > BWFileDialog::getFileNames() const
{
	QStringList files = s_fileDialogImpl->selectedFiles();
	std::vector< std::wstring > fileNames;
	for( QStringList::const_iterator it = files.constBegin();
		 it != files.constEnd(); ++it )
	{
		fileNames.push_back( ( wchar_t * ) it->constData() );
	}
	return fileNames;
}
