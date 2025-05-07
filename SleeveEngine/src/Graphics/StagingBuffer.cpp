#include "Graphics/StagingBuffer.h"

StagingBuffer::StagingBuffer( VkDevice device, uint64_t maxSize )
	:m_device(device), m_maxSize(maxSize)
{
	m_memoryBlocks.reserve( 4 );
	m_memoryBlocks.push_back( BufferMemoryBlock{ 0, maxSize } );
}

StagingBuffer::~StagingBuffer()
{
	vkUnmapMemory( m_device, m_stagingBufferMemory );

	vkDestroyBuffer( m_device, m_stagingBuffer, nullptr );
	vkFreeMemory( m_device, m_stagingBufferMemory, nullptr );
}

void StagingBuffer::Refresh()
{
	m_memoryBlocks.clear();
	m_memoryBlocks.push_back( BufferMemoryBlock{ 0, m_maxSize } );
}

bool StagingBuffer::FindProperPositionForSizeInBuffer( uint64_t& out_pos, uint64_t size )
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

void StagingBuffer::ReturnMemory( uint64_t offset, uint64_t size )
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
