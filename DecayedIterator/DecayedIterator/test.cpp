#include "pch.h"

#include <vector>
#include <ostream>

namespace DecayedIterator
{

template<typename TData>
class forward_iterator
{
private:
	struct IteratorWrapperI
	{
		virtual ~IteratorWrapperI() = default;
		virtual void stepForward() = 0;
		virtual bool equals(const IteratorWrapperI& other) const = 0;
		virtual TData& value() const = 0;
		virtual void clone(IteratorWrapperI* place) const = 0;
		virtual IteratorWrapperI* clone() const = 0;
	};

public:
	forward_iterator()
		: _iterWrapper(nullptr)
	{}

	template<typename TIterator>
	forward_iterator(const TIterator& iterator)
	{
		struct IteratorWrapper : IteratorWrapperI
		{
			IteratorWrapper(TIterator&& iterator)
				: _iterator(std::move(iterator))
			{}

			IteratorWrapper(const TIterator& iterator)
				: _iterator(iterator)
			{}
		
			void stepForward() override { ++_iterator; }
			bool equals(const IteratorWrapperI& other) const { return _iterator == dynamic_cast<const IteratorWrapper&>(other)._iterator; };
			TData& value() const override { return *_iterator; }
			void clone(IteratorWrapperI* place) const override { new(place) IteratorWrapper(_iterator); };
			IteratorWrapperI* clone() const override { return new IteratorWrapper(_iterator); };

			TIterator _iterator;
		};

		bool fitsIntoInternalStorage = sizeof(IteratorWrapper) <= sizeof(_iterWrapperInternalStorage);
		if (fitsIntoInternalStorage)
		{
			_iterWrapper = reinterpret_cast<IteratorWrapper*>(_iterWrapperInternalStorage);
			new(_iterWrapper) IteratorWrapper(std::move(iterator));
		}
		else
		{
			_iterWrapper = new IteratorWrapper(std::move(iterator));
		}
	}

	~forward_iterator()
	{
		auto iteratorIsOnTheInternalStorage = reinterpret_cast<char*>(_iterWrapper) == _iterWrapperInternalStorage;
		if (iteratorIsOnTheInternalStorage)
			_iterWrapper->~IteratorWrapperI();
		else
			delete _iterWrapper;

		_iterWrapper = nullptr;
	}
	
	forward_iterator(const forward_iterator& other)
	{
		copyFrom(other);
	}

	forward_iterator& operator=(const forward_iterator& other)
	{
		if (this == &other)
			return *this;

		copyFrom(other);
		return *this;
	}

	void operator++()
	{
		_iterWrapper->stepForward();
	}

	bool operator==(const forward_iterator& other) const
	{
		return _iterWrapper->equals(*other._iterWrapper);
	}

	bool operator!=(const forward_iterator& other) const
	{
		return !(*this == other);
	}

	TData& operator*() const
	{
		return _iterWrapper->value();
	}

private:
	void copyFrom(const forward_iterator& other)
	{
		if (!other._iterWrapper)
		{
			_iterWrapper = nullptr;
			return;
		}

		bool iteratorWrapperIsInInternalStorage = reinterpret_cast<char*>(other._iterWrapper) == other._iterWrapperInternalStorage;
		if (iteratorWrapperIsInInternalStorage)
		{
			_iterWrapper = reinterpret_cast<IteratorWrapperI*>(_iterWrapperInternalStorage);
			other._iterWrapper->clone(_iterWrapper);
		}
		else
		{
			_iterWrapper = other._iterWrapper->clone();
		}
	}

	IteratorWrapperI *_iterWrapper;
	char _iterWrapperInternalStorage[32];
};

}

#include <numeric>

TEST(ForwardIteratorTest, NormalConstructorTest)
{
	std::vector<int> numbers{ 1,2,3,4,5 };

	DecayedIterator::forward_iterator<int> iter = numbers.begin(), endIter = numbers.end();

	auto sum = std::accumulate(iter, endIter, 0);

	EXPECT_EQ(15, sum);
}

TEST(ForwardIteratorTest, DefaultConstructorTest)
{
	DecayedIterator::forward_iterator<int> iter, endIter;
}

TEST(ForwardIteratorTest, CopyAssignmentTest)
{
	DecayedIterator::forward_iterator<int> iter, endIter;

	std::vector<int> numbers{ 1,2,3,4,5 };
	iter = numbers.begin();
	endIter = numbers.end();

	auto sum = std::accumulate(iter, endIter, 0);

	EXPECT_EQ(15, sum);
}

TEST(ForwardIteratorTest, ComparisonTest)
{
	std::vector<int> numbers{ 1,2,3,4,5 };

	DecayedIterator::forward_iterator<int> iter = numbers.begin();

	EXPECT_EQ(DecayedIterator::forward_iterator<int>(numbers.begin()), iter);
	EXPECT_NE(DecayedIterator::forward_iterator<int>(numbers.end()), iter);
}

TEST(ForwardIteratorTest, DereferencingTest)
{
	std::vector<int> numbers{ 1,2,3,4,5 };

	DecayedIterator::forward_iterator<int> iter = numbers.begin();

	EXPECT_EQ(1, *iter);
}

TEST(ForwardIteratorTest, IterationTest)
{
	std::vector<int> numbers{ 1,2,3 };

	DecayedIterator::forward_iterator<int> iter = numbers.begin(), endIter = numbers.end();

	EXPECT_EQ(DecayedIterator::forward_iterator<int>(numbers.begin()), iter);
	++iter;
	EXPECT_NE(DecayedIterator::forward_iterator<int>(numbers.end()), iter);
	++iter;
	EXPECT_NE(DecayedIterator::forward_iterator<int>(numbers.end()), iter);
	++iter;
	EXPECT_EQ(DecayedIterator::forward_iterator<int>(numbers.end()), iter);
}
