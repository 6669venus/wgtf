#ifndef HEADER_FOOTER_TEXT_EXTENSION_HPP
#define HEADER_FOOTER_TEXT_EXTENSION_HPP

#include "i_model_extension.hpp"

class HeaderFooterTextExtension: public IModelExtension
{
public:
	HeaderFooterTextExtension();
	virtual ~HeaderFooterTextExtension();

	QHash<int, QByteArray> roleNames() const override;

	virtual QVariant headerData( int column, int role ) const override;
};

#endif
