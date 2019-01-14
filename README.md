# Decayed Iterator
Decays away the container from an iterator

If you want to return an iterator from a class, but you do not want to expose the container used inside the class,
then you have to decay away the container (or range manipulator) from the iterator.

So instead of
```
class MyClass
{
std::vector<int>::iterator numbers_begin() const;
};
```
You can use:
```
class MyClass
{
DecayedIterator::random_iterator<int> numbers_begin() const;
};
```

