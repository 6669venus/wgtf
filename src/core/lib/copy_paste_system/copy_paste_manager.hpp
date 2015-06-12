#ifndef COPY_PASTE_MANAGER_HPP
#define COPY_PASTE_MANAGER_HPP

#include "dependency_system/i_interface.hpp"
#include "i_copy_paste_manager.hpp"
#include <vector>

class ISerializationManager;

/**
 * CopyPasteManager
 * Responsible for serializing copy/paste data to the system clipboard.
 */
class CopyPasteManager
	: public Implements< ICopyPasteManager >
{
public:
	CopyPasteManager();
	~CopyPasteManager();

	void init( ISerializationManager * serializationMgr );
	void fini();

	void onSelect( ICopyableObject* pObject, bool append = false ) override;
	void onDeselect( ICopyableObject* pObject, bool reset = true ) override;
	bool copy() override;
	bool paste() override;
	bool canCopy() const override;
	bool canPaste() const override;

private:
	bool serializeData( IDataStream& stream, const Variant & value );
	bool deserializeData( IDataStream& stream, Variant & value );

	std::vector< ICopyableObject* > curObjects_;
	ISerializationManager * serializationMgr_;
};




#endif // COPY_PASTE_MANAGER_HPP
