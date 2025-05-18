#include "Graphics/IndexBuffer.h"

IndexBuffer::IndexBuffer( VkDevice device, uint32_t indexCount, uint64_t size ) :m_device( device ), m_indexCount( indexCount ), m_maxSize( size )
{
	m_memoryBlocks.reserve( 4 );
	m_memoryBlocks.push_back( BufferMemoryBlock{ 0, size } );
}

bool IndexBuffer::FindProperPositionForSizeInBuffer( uint64_t& out_pos, uint64_t size )
{
	// no memory, return and prepare to enlarge the buffer
	if (m_memoryBlocks.empty()) {
		return false;
	}
	else {
		// find empty memory blocks which can hold the data size
		for (size_t i = 0; i < m_memoryBlocks.size(); ++i) {
			// if the block's size is bigger than data size, split it into two blocks and use the first one
			if (m_memoryBlocks[i].m_size > size) {
				out_pos = m_memoryBlocks[i].m_startPos;
				m_memoryBlocks[i].m_size -= size;
				m_memoryBlocks[i].m_startPos += size;
				return true;
			}
			// if the block's size is equal to data size, return the position and erase it
			else if (m_memoryBlocks[i].m_size == size) {
				out_pos = m_memoryBlocks[i].m_startPos;
				m_memoryBlocks.erase( m_memoryBlocks.begin() + i );
				return true;
			}
		}
	}
	return false;
}

void IndexBuffer::EnlargeBuffer( uint64_t newSize )
{
	if (newSize <= m_maxSize) {
		return;
	}
	if (m_memoryBlocks.empty()) {
		m_memoryBlocks.emplace_back( BufferMemoryBlock{ m_maxSize, newSize - m_maxSize } );
	}
	else {
		m_memoryBlocks[m_memoryBlocks.size() - 1].m_size += (newSize - m_maxSize);
	}
	m_maxSize = newSize;
}

void IndexBuffer::ReturnMemory( uint64_t offset, uint64_t size )
{
	if (size > 0) {
		for (size_t i = 0; i < m_memoryBlocks.size(); ++i) {
			// if go pass the offset
			if (m_memoryBlocks[i].m_startPos > offset) {
				if (i >= 1 && m_memoryBlocks[i - 1].m_startPos + m_memoryBlocks[i - 1].m_size == offset) {
					// if the end of last block is the start of this block, add size to the last block
					m_memoryBlocks[i - 1].m_size += size;
					return;
				}
				else {
					// add a new block between i and i - 1
					m_memoryBlocks.insert( m_memoryBlocks.begin() + i, BufferMemoryBlock{ offset, size } );
					return;
				}
			}
		}
		// should be the last one
		m_memoryBlocks.emplace_back( BufferMemoryBlock{ offset, size } );
	}
}
