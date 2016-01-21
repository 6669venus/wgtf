//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_path.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "file_path.hpp"

#if defined( _WIN32 )
	const char& FilePath::kNativeDirectorySeparator = "\\"[0];
	const char& FilePath::kNativeAltDirectorySeparator = "/"[0];
#else
	const char& FilePath::kNativeDirectorySeparator = "/"[0];
	const char& FilePath::kNativeAltDirectorySeparator = "\\"[0];
#endif

const char& FilePath::kAltDirectorySeparator = "\\"[0];
const char& FilePath::kDirectorySeparator = "/"[0];
const char& FilePath::kExtensionSeparator = "."[0];
const char& FilePath::kVolumeSeparator = ":"[0];