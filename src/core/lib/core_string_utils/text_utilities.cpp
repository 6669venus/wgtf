#include "text_utilities.hpp"
#include <math.h>
#include <cassert>
#include <algorithm>

namespace TextUtilities
{

//==============================================================================
void incrementNumber( std::wstring & text, int & currentPos, wchar_t decimalChar )
{
	size_t decimal = text.find( decimalChar );
	size_t prevLength = text.length();

	double prevValue = std::stof( text );

	if (prevValue < 0 &&
		currentPos == 0)
	{
		return;
	}

	int pos = ( int ) ( text.length() - currentPos - 1 );
	bool floatTarget = pos < 0;
	double incrementVal = pow( 10, pos );
	bool wasFloat = decimal != std::wstring::npos;

	int decimalPlaces = 1;
	if (wasFloat)
	{
		decimalPlaces = ( currentPos - ( int ) decimal );
		if (decimalPlaces < 0)
		{
			decimalPlaces++;
		}
		else if (decimalPlaces == 0)
		{
			decimalPlaces = 1;
		}
		incrementVal = pow( 10, -decimalPlaces );
		floatTarget = true;
	}
	double newValue = prevValue + incrementVal;

	//Handle case where we go from a negative number to a positive number
	if (prevValue < 0 &&
		newValue > 0)
	{
		newValue = -prevValue + incrementVal;
	}

	int newIntValue = ( int ) newValue;

	bool appendDecimal = floatTarget && currentPos == prevLength;

	wchar_t formatStr[ 1024 ];
	size_t targetDecimalPlaces = wasFloat ? prevLength - 1 - decimal : 0;
	if (floatTarget)
	{
		if (appendDecimal)
		{
			targetDecimalPlaces++;
		}
		swprintf( formatStr, 1024, L"%%.%df", targetDecimalPlaces );
	}
	else
	{
		swprintf( formatStr, 1024, L"%%d" );
	}

	//TODO: Change to to_wstring() in C++11
	size_t capacity = std::max( text.capacity(), text.get_allocator().max_size() );
	size_t newLength;
	if(floatTarget)
	{
		newLength =
			swprintf( &*text.begin(), capacity, formatStr, newValue );
	}
	else
	{
		newLength =
			swprintf( &*text.begin(), capacity, formatStr, newIntValue );
	}

	//Handle case where preceding zeros are truncated when incrementing 
	//a number
	if (newLength < prevLength)
	{
		size_t formatLength = prevLength;
		if (prevValue < 0 &&
			newValue >= 0)
		{
			formatLength--;
		}
		if (floatTarget)
		{
			swprintf( formatStr, 1024, L"%%0%d.%df", formatLength, targetDecimalPlaces );
		}
		else
		{
			swprintf( formatStr, 1024, L"%%0%dd", formatLength );
		}

		if(floatTarget)
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newValue );

		}
		else
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newIntValue );
		}
	}
	if (newLength > prevLength)
	{
		text.resize( newLength );

		if (floatTarget)
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newValue );

		}
		else
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newIntValue );
		}
	}
	assert( newLength >= 0 );
	text.resize( newLength );
	int newCursorPosition = ( int )
		( currentPos + ( text.length() - prevLength ) );
	if (floatTarget && currentPos == prevLength)
	{
		newCursorPosition--;
	}

	if (newCursorPosition < 0)
	{
		newCursorPosition = 0;
	}
	currentPos = newCursorPosition;
}


//==============================================================================
void decrementNumber( std::wstring & text, int & currentPos, wchar_t decimalChar )
{
	size_t decimal = text.find( decimalChar );
	size_t prevLength = text.length();

	double prevValue = std::stof( text );

	if (prevValue <= 0 &&
		currentPos == 0)
	{
		return;
	}

	int pos = ( int ) ( text.length() - currentPos - 1 );
	bool floatTarget = pos < 0;
	double incrementVal = pow( 10, pos );
	bool wasFloat = decimal != std::wstring::npos;

	int decimalPlaces = 1;
	if (wasFloat)
	{
		decimalPlaces = ( currentPos - ( int ) decimal );
		if (decimalPlaces < 0)
		{
			decimalPlaces++;
		}
		else if (decimalPlaces == 0)
		{
			decimalPlaces = 1;
		}
		incrementVal = pow( 10, -decimalPlaces );
		floatTarget = true;
	}
	double newValue = prevValue - incrementVal;

	//Handle case where we go from a positive number to a negative number
	if (prevValue > 0 &&
		newValue < 0 )
	{
		newValue = -prevValue - incrementVal;
	}

	bool appendDecimal = floatTarget && currentPos == prevLength;

	int newIntValue = ( int ) newValue;

	wchar_t formatStr[ 1024 ];
	if (floatTarget)
	{
		size_t targetDecimalPlaces = wasFloat ? prevLength - 1 - decimal : 0;
		if (appendDecimal)
		{
			targetDecimalPlaces++;
		}
		swprintf( formatStr, 1024, L"%%.%df", targetDecimalPlaces );
	}
	else
	{
		swprintf( formatStr, 1024, L"%%d" );
	}
	size_t capacity = std::max( text.capacity(), text.get_allocator().max_size() );
	size_t newLength;
	if(floatTarget)
	{
		newLength =
			swprintf( &*text.begin(), capacity, formatStr, newValue );
	}
	else
	{
		newLength =
			swprintf( &*text.begin(), capacity, formatStr, newIntValue );
	}

	bool becameNegative = prevValue >= 0 && newValue < 0;
	//Handle case where preceding zeros are truncated when decrementing
	//a number
	if (newLength < prevLength || becameNegative)
	{
		size_t formatLength = prevLength;
		if (becameNegative)
		{
			formatLength++;
		}
		if (floatTarget)
		{
			size_t targetDecimalPlaces = wasFloat ? prevLength - 1 - decimal : 0;
			if (appendDecimal)
			{
				targetDecimalPlaces++;
			}
			swprintf( formatStr, 1024, L"%%0%d.%df", formatLength, targetDecimalPlaces );
		}
		else
		{
			swprintf( formatStr, 1024, L"%%0%dd", formatLength );
		}
		if(floatTarget)
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newValue );

		}
		else
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newIntValue );
		}
	}
		
	if (newLength > prevLength)
	{
		text.resize( newLength );
		newLength =
			swprintf( &*text.begin(), capacity, formatStr, newValue );
		if(floatTarget)
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newValue );
		}
		else
		{
			newLength =
				swprintf( &*text.begin(), capacity, formatStr, newIntValue );
		}
	}

	if (prevValue >= 0 &&
		newValue < 0)
	{
		currentPos++;
		return;
	}
	int newCursorPosition = ( int )
		( currentPos + ( text.length() - prevLength ) );
	if (floatTarget && currentPos == prevLength)
	{
		newCursorPosition--;
	}
	if (newCursorPosition < 0)
	{
		newCursorPosition = 0;
	}
	currentPos = newCursorPosition;
}

}