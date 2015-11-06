#include "xml_writer.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "wg_types/binary_block.hpp"


namespace
{

	const char* valueType( const Variant& value, IDefinitionManager& definitionManager )
	{
		const char* result = value.type()->name();

		value.with<ObjectHandle>( [&]( const ObjectHandle& v )
		{
			if( const IClassDefinition* definition = v.getDefinition( definitionManager ) )
			{
				result = definition->getName();
			}
		} );

		return result;
	}

	bool writeTypeExplicitly( const TypeId& type )
	{
		return
			type == TypeId::getType<Variant>() ||
			type == TypeId::getType<ObjectHandle>() ||
			type == TypeId::getType< std::shared_ptr< BinaryBlock > >();
	}

}


XMLWriter::XMLWriter( TextStream& stream, IDefinitionManager& definitionManager, const XMLSerializer::Format& format ):
	stream_( stream ),
	definitionManager_( definitionManager ),
	format_( format ),
	indent_( format.indent ),
	tagOpening_( false ),
	hasChildElements_( false )
{
}


bool XMLWriter::write( const Variant& value )
{
	beginOpenTag( format_.rootName.c_str() );
	writeValue( value, true );
	closeTag( format_.rootName.c_str() );

	return !fail();
}


void XMLWriter::writeValue( const Variant& value, bool explicitType )
{
	value.with<ObjectHandle>( [=]( const ObjectHandle& v )
	{
		writeObject( v, explicitType );
	} ) ||
	value.with<Collection>( [=]( const Collection& v )
	{
		writeCollection( v, explicitType );
	} ) ||
	value.with< std::shared_ptr< BinaryBlock > >( [=]( const std::shared_ptr< BinaryBlock >& v )
	{
		writeVariant( v, true );
	} ) ||
	value.with<Variant>( [=]( const Variant& v )
	{
		if( auto definition = definitionManager_.getDefinition( v.type()->typeId().getName() ) )
		{
			writeObject( ObjectHandle( v, definition ), explicitType );
		}
		else
		{
			writeVariant( v, explicitType );
		}
	} );
}


void XMLWriter::writeObject( const ObjectHandle& object, bool explicitType )
{
	if(!object.isValid())
	{
		// nullptr objecthandle
		return;
	}
	const IClassDefinition* definition = object.getDefinition( definitionManager_ );
	if( !definition )
	{
		stream_.setState( std::ios_base::failbit );
		return;
	}

	if( explicitType )
	{
		// actual object type
		writeAttribute( format_.typeAttribute, quoted( definition->getName() ) );
	}

	for( IBaseProperty* property: definition->allProperties() )
	{
		if( property->isMethod() )
		{
			continue;
		}

		// write object property
		const char* propertyName = property->getName();

		beginOpenTag( propertyName );
		writeValue(
			property->get( object, definitionManager_ ),
			writeTypeExplicitly( property->getType() ) );
		closeTag( propertyName );

		if( fail() )
		{
			return;
		}
	}
}


void XMLWriter::writeCollection( const Collection& collection, bool explicitType )
{
	if( explicitType )
	{
		writeAttribute( format_.typeAttribute, quoted( Variant::findType<Collection>()->name() ) );
	}

	const bool explicitKeyType = writeTypeExplicitly( collection.keyType() );
	const bool explicitValueType = writeTypeExplicitly( collection.valueType() );

	intmax_t assumedKey = 0;
	for( auto it = collection.begin(), end = collection.end(); it != end; ++it, ++assumedKey )
	{
		const Variant key = it.key();

		beginOpenTag( format_.collectionItemElement.c_str() );

		// write key
		bool keyMatchesAssumed = false;
		intmax_t keyIndex = 0;
		if( key.tryCast( keyIndex ) )
		{
			if( keyIndex == assumedKey )
			{
				keyMatchesAssumed = true;
			}
			else
			{
				assumedKey = keyIndex; // assumedKey tracks actual key
			}
		}

		if( !keyMatchesAssumed )
		{
			if( !key.type()->testFlags( MetaType::DeducibleFromText ) )
			{
				if( explicitKeyType )
				{
					writeAttribute( format_.keyTypeAttribute, quoted( valueType( key, definitionManager_ ) ) );
				}

				std::string keyValue;
				if( !key.tryCast( keyValue ) )
				{
					// arbitrary type can be saved in attribute only as string
					stream_.setState( std::ios_base::failbit );
					return;
				}
				writeAttribute( format_.keyAttribute, quoted( keyValue ) );
			}
			else
			{
				writeAttribute( format_.keyAttribute, key );
			}
		}

		// write value
		writeValue( it.value(), explicitValueType );

		closeTag( format_.collectionItemElement.c_str() );

		if( fail() )
		{
			return;
		}
	}
}


void XMLWriter::writeVariant( const Variant& variant, bool explicitType )
{
	if( variant.isVoid() )
	{
		// nop
		return;
	}

	if( explicitType &&
		!variant.type()->testFlags( MetaType::DeducibleFromText ) )
	{
		writeAttribute( format_.typeAttribute, quoted( variant.type()->name() ) );
	}

	endOpenTag();

	stream_ << format_.padding << variant << format_.padding;
}


void XMLWriter::writeIndent()
{
	if( !format_.indentString.empty() )
	{
		for( unsigned i = 0; i < indent_; i++ )
		{
			stream_ << format_.indentString;
		}
	}
}


void XMLWriter::writeNewline()
{
	if( format_.newLines )
	{
		stream_ << "\n";
	}
}


void XMLWriter::beginOpenTag( const char* tag )
{
	if( tagOpening_ )
	{
		stream_ << ">";
		writeNewline();
	}

	writeIndent();
	stream_ << "<" << tag;
	tagOpening_ = true;
	hasChildElements_ = false;
	indent_ += 1;
}


void XMLWriter::endOpenTag()
{
	if( !tagOpening_ )
	{
		return;
	}

	stream_ << ">";
	tagOpening_ = false;
}


void XMLWriter::closeTag( const char* tag )
{
	indent_ -= 1;

	if( tagOpening_ )
	{
		stream_ << "/>";
		tagOpening_ = false;
	}
	else
	{
		if( hasChildElements_ )
		{
			writeIndent();
		}
		stream_ << "</" << tag << ">";
	}

	writeNewline();

	hasChildElements_ = true;
}


