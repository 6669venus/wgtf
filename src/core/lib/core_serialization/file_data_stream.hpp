
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_data_stream.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef FILE_DATA_STREAM_H_
#define FILE_DATA_STREAM_H_

#pragma once

#include "core_serialization/i_datastream.hpp"
#include <fstream>

class FileDataStream : public IDataStream
{
public:
	FileDataStream(const char* path, std::ios::openmode mode);
	virtual void seek(size_t pos) override;

	virtual size_t pos() const override;

	virtual size_t size() const override;

	virtual const void * rawBuffer() const override;

	virtual size_t readRaw(void * o_Data, size_t length) override;

	virtual size_t writeRaw(const void * data, size_t length) override;

	virtual bool writeValue(const Variant & variant) override;

	virtual bool readValue(Variant & variant) override;
private:
	std::fstream	m_fstream;
	size_t			m_size;
    size_t          m_position;
};

#endif // FILE_DATA_STREAM_H_