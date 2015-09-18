#ifndef XML_SERIALIZER_HPP_INCLUDED
#define XML_SERIALIZER_HPP_INCLUDED

#include "i_serializer.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/text_stream.hpp"
#include <string>


class XMLSerializer:
	public Implements< ISerializer >
{
public:
	using ISerializer::deserialize;

	struct Format
	{
		struct Unformatted {};

		/**
		Construct Format with default values.
		*/
		Format();

		/**
		Construct Format with all formatting turned off.
		*/
		Format( Unformatted );

		/**
		Name of the root node.

		Default is `"root"`.
		*/
		std::string rootName;

		/**
		String used for indentation.

		Default is `"\t"`.
		*/
		std::string indentString;

		/**
		Use new lines for formatting.

		Default is `true`.
		*/
		bool newLines;

		/**
		String used for tag content padding.

		Default is `"\t"`.
		*/
		std::string padding;

		/**
		Indentation level.

		Specifies initial indentation level of the root node.

		Default is `0`.
		*/
		unsigned indent;

		// names for XML nodes
		std::string typeAttribute;
		std::string keyTypeAttribute;
		std::string keyAttribute;
		std::string collectionItemElement;
	};

	XMLSerializer( IDataStream& dataStream, IDefinitionManager& definitionManager );

	bool serialize( const Variant& value ) override;
	bool deserialize( Variant& value ) override;

	const Format& format() const
	{
		return format_;
	}

	void setFormat( const Format& format );

	bool sync();

private:
	TextStream stream_;
	IDefinitionManager& definitionManager_;
	Format format_;

};


#endif
