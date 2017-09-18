// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "util/arena.h"
#include <assert.h>

namespace leveldb
{

static const int kBlockSize = 4096;

Arena::Arena() : memory_usage_(0)
{
	alloc_ptr_ = NULL; // First allocation will allocate a block
	alloc_bytes_remaining_ = 0;
}

Arena::~Arena()
{
	for (size_t i = 0; i < blocks_.size(); i++)
	{
		delete[] blocks_[i];
	}
}

char *Arena::AllocateFallback(size_t bytes)
{
	// kBlockSize/4 是一个阈值
	if (bytes > kBlockSize / 4)
	{
		// Object is more than a quarter of our block size.  Allocate it separately
		// to avoid wasting too much space in leftover bytes.
		// bytes 较大，另外申请 bytes 个直接返回，不改变 alloc_ptr_，避免浪费内存（如果改变了就不能再用剩余的内存了）
		char *result = AllocateNewBlock(bytes);// 按实际需求分配一个新的 block，不按 4096 分配
		return result;
	}
	else if (bytes <= kBlockSize / 4)
	{
		// 原来剩下的内存扔掉不用了，浪费量 小于 1024 字节
		alloc_ptr_ = AllocateNewBlock(kBlockSize);
		alloc_bytes_remaining_ = kBlockSize;
		char *result = alloc_ptr_;
		alloc_ptr_ += bytes;
		alloc_bytes_remaining_ -= bytes;
		return result;
	}
}

// 字节对齐内存分配
char *Arena::AllocateAligned(size_t bytes)
{
	const int align = (sizeof(void *) > 8) ? sizeof(void *) : 8; // 确定按 8 字节对齐还是其他
	assert((align & (align - 1)) == 0); // 对齐量应该是 2 的幂
	size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);// 对 align 按位运算取模（align位 2 的幂时才可以）
	size_t slop = (current_mod == 0 ? 0 : align - current_mod);  // 距离最近的对齐差几位
	size_t needed = bytes + slop;   // 总共需要的是 bytes 再补上差的位数
	char *result;
	if (needed <= alloc_bytes_remaining_) // 当前 block 足够
	{
		result = alloc_ptr_ + slop; // 返回最近的对齐点
		alloc_ptr_ += needed;  // 下次可用的开始点
		alloc_bytes_remaining_ -= needed;  //
	}
	else
	{
		// AllocateFallback always returned aligned memory
		result = AllocateFallback(bytes); // 直接返回 bytes
	}
	assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
	return result;
}

char *Arena::AllocateNewBlock(size_t block_bytes) // 申请一块新的内存，char[block_bytes]
{
	char *result = new char[block_bytes];
	blocks_.push_back(result);
	memory_usage_.NoBarrier_Store(reinterpret_cast<void *>(MemoryUsage() + block_bytes + sizeof(char *)));
	return result;
}

} // namespace leveldb
