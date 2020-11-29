/*****************************************************************************
*                                                                            *
*  PrimeSense PSCommon Library                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PSCommon.                                            *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef _XN_HASH_H_
#define _XN_HASH_H_

#include <list>

#include "XnMemory.h"
#include "XnPair.h"

namespace xnl
{
template <class _Key, class _Value>
struct KeyValuePair : private Pair<_Key, _Value>
{
	typedef _Key TKey;
	typedef _Value TValue;

	using Pair<_Key, _Value>::first;
	using Pair<_Key, _Value>::second;

	KeyValuePair() : Pair<_Key, _Value>() {}
	KeyValuePair(TKey key, TValue value) : Pair<_Key, _Value>(key, value) {}
	KeyValuePair(const KeyValuePair& other) : Pair<_Key, _Value>(other) {}
public:
	const TKey& Key() const {return first;}
	const TValue& Value() const {return second;}
	TValue& Value() {return second;}
};

typedef XnUInt8 HashCode;

template <class TKey>
class DefaultKeyManager
{
public:
	static xnl::HashCode Hash(const TKey& key)
	{
		return ((XnSizeT)key) & 0xff;
	}
	static int32_t Compare(const TKey& key1, const TKey& key2)
	{
		return int32_t(XnSizeT(key1)-XnSizeT(key2));
	}
};

template <class TKey, class TValue, class KeyManager = DefaultKeyManager<TKey>>
class Hash
{
public:
	typedef KeyValuePair<TKey, TValue> TPair;
	typedef std::list<TPair> TPairList;

	enum
	{
		LAST_BIN = (1 << sizeof(xnl::HashCode)*8),
		NUM_BINS = LAST_BIN+1
	};

	class ConstIterator
	{
	public:
		ConstIterator() : m_ppBins(NULL), m_currBin(0) {}
		ConstIterator(TPairList* const* apBins, uint32_t currBin, typename TPairList::const_iterator currIt) :
			m_ppBins(apBins), m_currBin(currBin), m_currIt(currIt)
		{
			if (currBin != LAST_BIN && m_currIt == m_ppBins[m_currBin]->end())
			{
				++*this;
			}
		}
		ConstIterator(const ConstIterator& other) :
			m_ppBins(other.m_ppBins), m_currBin(other.m_currBin), m_currIt(other.m_currIt)
		{}

		ConstIterator& operator=(const ConstIterator& other) = default;

		ConstIterator& operator++()
		{
			XN_ASSERT(m_currBin != LAST_BIN);

			if (m_currIt != m_ppBins[m_currBin]->end())
			{
				++m_currIt;
			}

			if (m_currIt == m_ppBins[m_currBin]->end())
			{
				do
				{
					++m_currBin;
				} while (m_currBin < LAST_BIN && (m_ppBins[m_currBin] == NULL || m_ppBins[m_currBin]->empty()));

				m_currIt = m_ppBins[m_currBin]->begin();
			}
			return *this;
		}
		ConstIterator operator++(int32_t)
		{
			ConstIterator retVal(*this);
			++*this;
			return retVal;
		}

		inline bool operator==(const ConstIterator& other) const
		{
			return m_currIt == other.m_currIt;
		}
		inline bool operator!=(const ConstIterator& other) const
		{
			return m_currIt != other.m_currIt;
		}

		inline const TPair& operator*() const
		{
			return *m_currIt;
		}
		inline const TPair* operator->() const
		{
			return m_currIt.operator->();
		}
	protected:
		friend class Hash;

		TPairList* const* m_ppBins;
		uint32_t m_currBin;
		typename TPairList::const_iterator m_currIt;
	};

	class Iterator : public ConstIterator
	{
	public:
		Iterator() : ConstIterator() {}

		Iterator(TPairList** apBins, uint32_t currBin, typename TPairList::iterator currIt) :
			ConstIterator(apBins, currBin, currIt)
		{}
		Iterator(const Iterator& other) : ConstIterator(other) {}

		Iterator& operator++()
		{
			++(*(ConstIterator*)this);
			return *this;
		}
		Iterator operator++(int32_t)
		{
			Iterator retVal(*this);
			++*this;
			return retVal;
		}
		Iterator& operator--()
		{
			--(*(ConstIterator*)this);
			return *this;
		}
		Iterator operator--(int32_t)
		{
			Iterator retVal(*this);
			--*this;
			return retVal;
		}
		inline TPair& operator*() const
		{
			return const_cast<TPair&>(*this->m_currIt);
		}
		inline TPair* operator->() const
		{
			return const_cast<TPair*>(this->m_currIt.operator->());
		}
		Iterator& operator=(const Iterator& other) = default;
	};

	Hash()
	{
		Init();
	}
	Hash(const Hash& other)
	{
		Init();
		*this = other;
	}

	Hash& operator=(const Hash& other)
	{
		Clear();

		XnStatus retVal = XN_STATUS_OK;
		for (ConstIterator it = other.Begin(); it != other.End(); ++it)
		{
			retVal = Set(it->Key(), it->Value());
			XN_ASSERT(retVal == XN_STATUS_OK);
			XN_REFERENCE_VARIABLE(retVal);
		}
		return *this;
	}

	~Hash()
	{
		for (uint32_t i = 0; i < LAST_BIN; ++i)
		{
			if (m_apBins[i] != NULL)
			{
				XN_DELETE(m_apBins[i]);
			}
		}
	}

	Iterator Begin()
	{
		return Iterator(m_apBins, m_minBin, m_apBins[m_minBin]->begin());
	}
	ConstIterator Begin() const
	{
		return ConstIterator(m_apBins, m_minBin, m_apBins[m_minBin]->begin());
	}
	Iterator End()
	{
		return Iterator(m_apBins, LAST_BIN, m_apBins[LAST_BIN]->begin());
	}
	ConstIterator End() const
	{
		return ConstIterator(m_apBins, LAST_BIN, m_apBins[LAST_BIN]->begin());
	}

	XnStatus Set(const TKey& key, const TValue& value)
	{
		xnl::HashCode hash = KeyManager::Hash(key);

		if (m_apBins[hash] == NULL)
		{
			m_apBins[hash] = XN_NEW(TPairList);

			if (hash < m_minBin)
			{
				m_minBin = hash;
			}
		}
		for (typename TPairList::iterator it = m_apBins[hash]->begin(); it != m_apBins[hash]->end(); ++it)
		{
			if (KeyManager::Compare(it->Key(), key) == 0)
			{
				// Replace
				it->Value() = value;
				return XN_STATUS_OK;
			}
		}

		m_apBins[hash]->push_back(TPair(key, value));
		return XN_STATUS_OK;
	}

	ConstIterator Find(const TKey& key) const
	{
		uint32_t bin = LAST_BIN;
		typename TPairList::const_iterator it;
		if (Find(key, bin, it))
		{
			return ConstIterator(m_apBins, bin, it);
		}
		return End();
	}
	Iterator Find(const TKey& key)
	{
		uint32_t bin = LAST_BIN;
		typename TPairList::iterator it;
		if (Find(key, bin, it))
		{
			return Iterator(m_apBins, bin, it);
		}
		return End();
	}
	XnStatus Find(const TKey& key, ConstIterator& it) const
	{
		it = Find(key);
		return it == End() ? XN_STATUS_NO_MATCH : XN_STATUS_OK;
	}
	XnStatus Find(const TKey& key, Iterator& it)
	{
		it = Find(key);
		return it == End() ? XN_STATUS_NO_MATCH : XN_STATUS_OK;
	}

	XnStatus Get(const TKey& key, TValue& value) const
	{
		ConstIterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}

		value = it->Value();
		return XN_STATUS_OK;
	}
	XnStatus Get(const TKey& key, const TValue*& pValue) const
	{
		ConstIterator it = Find(key);
		if(it == End())
		{
			return XN_STATUS_NO_MATCH;
		}

		pValue = &it->Value();
		return XN_STATUS_OK;
	}
	XnStatus Get(const TKey& key, TValue& value)
	{
		Iterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}

		value = it->Value();
		return XN_STATUS_OK;
	}
	XnStatus Get(const TKey& key, TValue*& pValue)
	{
		Iterator it = Find(key);
		if(it == End())
		{
			return XN_STATUS_NO_MATCH;
		}

		pValue = &it->Value();
		return XN_STATUS_OK;
	}

	TValue& operator[](const TKey& key)
	{
		Iterator it = Find(key);
		if (it == End())
		{
			XnStatus retVal = Set(key, TValue());
			XN_ASSERT(retVal == XN_STATUS_OK);
			XN_REFERENCE_VARIABLE(retVal);

			it = Find(key);
			XN_ASSERT(it != End());
		}
		return it->Value();
	}

	XnStatus Remove(ConstIterator it)
	{
		if (it == End())
		{
			XN_ASSERT(false);
			return XN_STATUS_ILLEGAL_POSITION;
		}

 		XN_ASSERT(m_apBins == it.m_ppBins);
 		XN_ASSERT(m_apBins[it.m_currBin] != NULL);

		m_apBins[it.m_currBin]->erase(it.m_currIt);
		return XN_STATUS_OK;
	}

	XnStatus Remove(const TKey& key)
	{
		ConstIterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}

		return Remove(it);
	}

	XnStatus Clear()
	{
		while (Begin() != End())
		{
			Remove(Begin());
		}

		return XN_STATUS_OK;
	}

	bool IsEmpty() const
	{
		return (Begin() == End());
	}

	uint32_t Size() const
	{
		uint32_t size = 0;
		for (ConstIterator iter = Begin(); iter != End(); ++iter, ++size)
		{}

		return size;
	}
private:
	bool Find(const TKey& key, uint32_t& bin, typename TPairList::const_iterator& currIt) const
	{
		xnl::HashCode hash = KeyManager::Hash(key);

		if (m_apBins[hash] != NULL)
		{
			for (typename TPairList::const_iterator it = m_apBins[hash]->begin(); it != m_apBins[hash]->end(); ++it)
			{
				if (KeyManager::Compare(it->Key(), key) == 0)
				{
					bin = hash;
					currIt = it;
					return true;
				}
			}
		}

		return false;
	}

	bool Find(const TKey& key, uint32_t& bin, typename TPairList::iterator& currIt) const
	{
		xnl::HashCode hash = KeyManager::Hash(key);

		if (m_apBins[hash] != NULL)
		{
			for (typename TPairList::iterator it = m_apBins[hash]->begin(); it != m_apBins[hash]->end(); ++it)
			{
				if (KeyManager::Compare(it->Key(), key) == 0)
				{
					bin = hash;
					currIt = it;
					return true;
				}
			}
		}

		return false;
	}

	void Init()
	{
		xnOSMemSet(m_apBins, 0, sizeof(m_apBins));
		m_apBins[LAST_BIN] = &m_lastBin;
		m_minBin = LAST_BIN;
	}

	TPairList* m_apBins[NUM_BINS];
	TPairList m_lastBin;
	uint32_t m_minBin;

};

} // xnl

#endif // _XN_HASH_H_
