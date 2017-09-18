// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace leveldb
{

class Slice
{
public:
	// 创建一个空的 Slice
	Slice() : data_(""), size_(0) {}

	// 用 char *d 初始化一个 Sclice 
	Slice(const char *d, size_t n) : data_(d), size_(n) {}

	// 用一个 string 来初始化 Slice
	Slice(const std::string &s) : data_(s.data()), size_(s.size()) {}

	// 用 char* 类型初始化 Slice
	Slice(const char *s) : data_(s), size_(strlen(s)) {}

	// 返回被封装的 char* 数据
	const char *data() const { return data_; }

	// 返回数据大小
	size_t size() const { return size_; }

	// 判断数据是否为空
	bool empty() const { return size_ == 0; }

	// 返回第 n 个位置的字符
	// 要求 n < size()
	char operator[](size_t n) const
	{
		assert(n < size());
		return data_[n];
	}

	// 清除
	void clear()
	{
		data_ = "";
		size_ = 0;
	}

	// 删掉前 n 个数据（指针直接 +n ）
	void remove_prefix(size_t n) 
	{
		assert(n <= size());
		data_ += n;
		size_ -= n;
	}

	// Slice 转换为 string
	std::string ToString() const { return std::string(data_, size_); }

	// Three-way comparison.  Returns value:
	//   <  0 iff "*this" <  "b",
	//   == 0 iff "*this" == "b",
	//   >  0 iff "*this" >  "b"
	int compare(const Slice &b) const;

	// Return true iff "x" is a prefix of "*this"
	bool starts_with(const Slice &x) const
	{
		return ((size_ >= x.size_) &&
				(memcmp(data_, x.data_, x.size_) == 0));
	}

private:
	const char *data_; // 不能通过 data 修改其指向的值
	size_t size_; // 数据量

	// Intentionally copyable
};

inline bool operator==(const Slice &x, const Slice &y)
{
	// 长度相等 且 内容相等 时返回 true
	return ((x.size() == y.size()) && (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice &x, const Slice &y)
{
	return !(x == y);
}

inline int Slice::compare(const Slice &b) const
{
	const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
	int r = memcmp(data_, b.data_, min_len);
	if (r == 0) // 有重合
	{
		if (size_ < b.size_) // b 比较长
			r = -1;   // a < b
		else if (size_ > b.size_)
			r = +1;    // b < a
	}
	return r;
}

} // namespace leveldb

#endif // STORAGE_LEVELDB_INCLUDE_SLICE_H_
