//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_path.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "file_path.hpp"

#if defined( _WIN32 )
	const char FilePath::kNativeDirectorySeparator = '\\';
	const char FilePath::kNativeAltDirectorySeparator = '/';
#else
	const char FilePath::kNativeDirectorySeparator = '/';
	const char FilePath::kNativeAltDirectorySeparator = '\\';
#endif

const char FilePath::kAltDirectorySeparator = '\\';
const char FilePath::kDirectorySeparator = '/';
const char FilePath::kExtensionSeparator = '.';
const char FilePath::kVolumeSeparator = ':';