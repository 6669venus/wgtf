#ifndef BINARY_BLOCK_HPP
#define BINARY_BLOCK_HPP

class BinaryBlock
{
public:
	BinaryBlock( const void * data, size_t len, bool externallyOwned );
	~BinaryBlock();

	const void * data() const;
	char *	cdata() const;
	size_t length() const;
	int compare( BinaryBlock& that ) const;

private:
	void *	data_;
	size_t	length_;
	bool    externallyOwned_;
};

#endif //BINARY_BLOCK_HPP