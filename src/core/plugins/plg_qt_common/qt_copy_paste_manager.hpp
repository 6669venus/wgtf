#ifndef QT_COPY_PASTE_MANAGER_HPP
#define QT_COPY_PASTE_MANAGER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"
#include <vector>

class QClipboard;
class ISerializationManager;
class ICommandManager;

/**
 * QtCopyPasteManager
 * Responsible for serializing copy/paste data to the system clipboard.
 */
class QtCopyPasteManager
	: public Implements< ICopyPasteManager >
{
public:
	QtCopyPasteManager();
	~QtCopyPasteManager();

	void init( ISerializationManager * serializationMgr, ICommandManager * commandSystem );
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

    QClipboard * clipboard_;
	std::vector< ICopyableObject* > curObjects_;
	ISerializationManager * serializationMgr_;
	ICommandManager * commandSystem_;
};




#endif // QT_COPY_PASTE_MANAGER_HPP
