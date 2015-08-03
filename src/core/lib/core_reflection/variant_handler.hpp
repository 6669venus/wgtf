#ifndef REFLECTION_STORAGE_LOOKUP_HANDLER
#define REFLECTION_STORAGE_LOOKUP_HANDLER

#include "variant/interfaces/i_meta_type_manager.hpp"

class IDefinitionManager;

class ReflectionStorageLookupHandler
	: public IStorageLookupHandler
{
public:
	ReflectionStorageLookupHandler(
		const IDefinitionManager * definitionManager,
		const MetaType * reflectionMetaType );
	const MetaType * findStorageType( const TypeId & typeId ) const override;
	size_t storageSize() const override;
	void initStorage(
		void * pStorage,
		const TypeId & typeId,
		const void * value ) const override;
	void tryConvert( const TypeId & sourceType, const void * pSource ) const override;
private:
	const IDefinitionManager *	definitionManager_;
	const MetaType *			reflectionMetaType_;
};

#endif //REFLECTION_STORAGE_LOOKUP_HANDLER