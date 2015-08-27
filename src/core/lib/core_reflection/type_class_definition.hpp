#ifndef TYPE_CLASS_DEFINITION_HPP
#define TYPE_CLASS_DEFINITION_HPP

#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_class_definition_modifier.hpp"
#include "utilities/definition_helpers.hpp"
#include "metadata/meta_impl.hpp"
#include "metadata/meta_utilities.hpp"
#include <memory>

template< typename Type >
class TypeClassDefinition
	: public IClassDefinitionDetails
{
	typedef Type SelfType;

	std::unique_ptr< const MetaBase > metaData_;
	const char * parentName_;
	mutable CastHelperCache castHelperCache_;

public:
	TypeClassDefinition();

	void init( IClassDefinitionModifier & ) override;

	//--------------------------------------------------------------------------
	bool isAbstract() const override
	{
		return std::is_abstract< Type >();
	}


	//--------------------------------------------------------------------------
	bool isGeneric() const override
	{
		return false;
	}


	//--------------------------------------------------------------------------
	const char * getName() const override
	{
		return getClassIdentifier< Type >();
	}


	//--------------------------------------------------------------------------
	const char * getParentName() const override
	{
		return parentName_;
	}
	

	//--------------------------------------------------------------------------
	const MetaBase * getMetaData() const override
	{
		return metaData_.get();
	}

	//--------------------------------------------------------------------------
	ObjectHandle createBaseProvider(
		const ReflectedPolyStruct & polyStruct ) const override
	{
		auto pType =
			ReflectionUtilities::dynamicCast< Type >( polyStruct );
		return createBaseProvider( polyStruct.getDefinition(), pType );
	}


	//--------------------------------------------------------------------------
	ObjectHandle createBaseProvider( const IClassDefinition & definition,
		const void * pThis ) const override
	{
		return BaseProviderHelper::getBaseProvider( pThis, &definition );
	}


	//--------------------------------------------------------------------------
	ObjectHandle create( const IClassDefinition & definition ) const
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create() );
		PolyStructDefinitionSetter setter( pInst.get(), &definition );
		return ObjectHandle( std::move( pInst ), &definition );
	}

	//--------------------------------------------------------------------------
	template<class TArg1>
	static ObjectHandleT<Type> create(const IClassDefinition & definition, TArg1&& arg)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg) ) );
		PolyStructDefinitionSetter setter(pInst.get(), &definition);
		return ObjectHandle(std::move(pInst), &definition);
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2>
	static ObjectHandleT<Type> create(const IClassDefinition & definition, TArg1&& arg, TArg2&& arg2)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2) ) );
		PolyStructDefinitionSetter setter(pInst.get(), &definition);
		return ObjectHandle(std::move(pInst), &definition);
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg1>(arg2), std::forward<TArg3>(arg3) ) );
		PolyStructDefinitionSetter setter(pInst.get(), &definition);
		return ObjectHandle(std::move(pInst), &definition);
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3, class TArg4>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4 ) ) );
		PolyStructDefinitionSetter setter(pInst.get(), &definition);
		return ObjectHandle(std::move(pInst), &definition);
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4), std::forward<TArg5>(arg5) ) );
		PolyStructDefinitionSetter setter(pInst.get(), &definition);
		return ObjectHandle(std::move(pInst), &definition);
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4), std::forward<TArg5>(arg5), std::forward<TArg6>(arg6) ) );
		PolyStructDefinitionSetter setter(pInst.get(), &definition);
		return ObjectHandle(std::move(pInst), &definition);
	}
	
	//--------------------------------------------------------------------------
	CastHelperCache * getCastHelperCache() const override
	{
		return &castHelperCache_;
	}


	//==========================================================================
	struct BaseProviderHelper
	{
		static ObjectHandle getBaseProvider(
			const void * pThis, const IClassDefinition * definition  )
		{
			if (findFirstMetaData<MetaOnStackObj>( definition->getMetaData() ) != nullptr)
			{
				return ObjectHandle::makeStorageBackedProvider(
					*static_cast< const Type * >( pThis ), definition );
			}
			return ObjectHandle( *static_cast< const Type * >( pThis ), definition );
		}
	};
};

#endif // #define TYPE_CLASS_DEFINITION_HPP

