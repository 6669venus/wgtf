#include "pch.hpp"

#include "string_utils/text_utilities.hpp"

TEST( text_utilities_testIncrement )
{	
	int cursorPosition;
	int cursorPositionBefore;
	std::wstring container;

	container = L"255";
	cursorPosition = ( int ) ( container.length() - 1 );
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"256" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"255";
	cursorPosition = ( int ) ( container.length() - 2 );
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"265" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"255";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"355" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"99";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"109" );
	CHECK( cursorPosition == cursorPositionBefore + 1 );

	container = L"-11";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"-01" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"-009";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"109" );
	CHECK( cursorPosition == 0 );

	container = L"010";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"020" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"-100";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"000" );
	CHECK( cursorPosition == 0 );

	container = L"-100";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"-100" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"10";
	cursorPosition = 2;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"10.1" );
	CHECK( cursorPosition == cursorPositionBefore + 1 );

	container = L"10.1";
	cursorPosition = 3;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"10.2" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"10.1";
	cursorPosition = 2;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"10.2" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"10.1";
	cursorPosition = 4;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"10.11" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"12.346";
	cursorPosition = 4;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"12.356" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"-10";
	cursorPosition = 3;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"-9.9" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"-11";
	cursorPosition = 3;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"-10.9" );
	CHECK( cursorPosition == cursorPositionBefore + 1 );

	container = L"-0.01";
	cursorPosition = 4;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"0.00" );
	CHECK( cursorPosition == cursorPositionBefore - 1 );

	container = L"-1.5";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::incrementNumber( container, cursorPosition );
	CHECK( container == L"-0.5" );
	CHECK( cursorPosition == cursorPositionBefore );
}


TEST( text_utilities_testDecrement )
{	
	int cursorPosition;
	int cursorPositionBefore;
	std::wstring container;

	container = L"255";
	cursorPosition = ( int ) ( container.length() - 1 );
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"254" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"255";
	cursorPosition = ( int ) ( container.length() - 2 );
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"245" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"255";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"155" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"109";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"009" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"009";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-109" );
	CHECK( cursorPosition == 1 );

	container = L"100";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"000" );
	CHECK( cursorPosition == 0 );

	container = L"-010";
	cursorPosition = 2;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-020" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"000";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-010" );
	CHECK( cursorPosition == cursorPositionBefore + 1 );

	container = L"000";
	cursorPosition = 2;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-001" );
	CHECK( cursorPosition == cursorPositionBefore + 1 );

	container = L"-100";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-100" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"10";
	cursorPosition = 2;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"9.9" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"10.1";
	cursorPosition = 3;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"10.0" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"10.1";
	cursorPosition = 4;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"10.09" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"12.653";
	cursorPosition = 4;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"12.643" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"-10";
	cursorPosition = 3;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-10.1" );
	CHECK( cursorPosition == cursorPositionBefore + 1 );

	container = L"-1.5";
	cursorPosition = 1;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"-2.5" );
	CHECK( cursorPosition == cursorPositionBefore );

	container = L"12.4111";
	cursorPosition = 0;
	cursorPositionBefore = cursorPosition;
	TextUtilities::decrementNumber( container, cursorPosition );
	CHECK( container == L"02.4111" );
	CHECK( cursorPosition == cursorPositionBefore );
}


