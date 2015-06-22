//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  file_data_stream.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "file_data_stream.hpp"
#include <fstream>
#include <algorithm>

FileDataStream::FileDataStream(const char* path, std::ios::openmode mode) : m_fstream(path, mode | std::ios::ate), m_size(UINTPTR_MAX)
{
	m_size = static_cast<size_t>(m_fstream.tellp());
	m_fstream.seekg(0);
}

void FileDataStream::seek(size_t pos)
{
	m_fstream.seekg(pos);
}

size_t FileDataStream::pos() const
{
	return m_fstream.cur;
}

size_t FileDataStream::size() const
{
	return m_size;
}

const void * FileDataStream::rawBuffer() const
{
	return m_fstream.rdbuf();
}

size_t FileDataStream::readRaw(void * o_Data, size_t length)
{
	size_t cur = pos();
	m_fstream.read(static_cast<char*>(o_Data), length);
	return pos() - cur;
}

size_t FileDataStream::writeRaw(const void * data, size_t length)
{
	size_t cur = pos();
	m_fstream.write(static_cast<const char*>(data), length);
	return pos() - cur;
}

bool FileDataStream::writeValue(const Variant & variant)
{
	m_fstream << variant;
	return true;
}

bool FileDataStream::readValue(Variant & variant)
{
	m_fstream >> variant;
	return true;
}