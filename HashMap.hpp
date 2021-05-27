#ifndef EX6_LIBRARY_H
#define EX6_LIBRARY_H

#define MAX_LOAD_FACTOR 0.75
#define MIN_LOAD_FACTOR 0.25

#define MIN_CAPACITY 16


#include <vector>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <iostream>

template<typename keyT>
int getHashTo(keyT k, int cap);

/**
 * HashMap class, maps keys of type keyT to values of type valueT
 * @tparam keyT
 * @tparam valueT
 */
template<typename keyT, typename valueT>
class HashMap
{
public:
    size_t _capacity;
    size_t _size;
    typedef std::vector<std::pair<keyT, valueT>> bucket;
    bucket *map; //

    //***constructors***
    //default constructor
    HashMap();
    //constructor 1
    template<typename KeysInputIterator, typename ValuesInputIterator>
    HashMap(KeysInputIterator keysBegin,
            KeysInputIterator keysEnd,
            ValuesInputIterator valuesBegin,
            ValuesInputIterator valuesEnd);
    //copy constructor
    HashMap(HashMap const &other);

    //destructor
    ~HashMap();

    //***methods***
    size_t size() const;
    size_t capacity() const;
    bool empty() const;
    bool insert(keyT k, valueT v);
    bool contains_key(keyT k) const;
    valueT &at(const keyT & k); //for writing
    const valueT &at(const keyT & k) const; //for reading
    bool erase(keyT k);
    double load_factor() const;
    size_t bucket_size(keyT k) const;
    size_t bucket_index(keyT k) const;
    void clear();
    void rehash(bool expand = false);

    //iterator
    class iterator;
    typedef iterator const_iterator;

    iterator begin() const;
    iterator end() const;

    iterator cbegin() const;
    iterator cend() const;

    //***operators***
    HashMap & operator=(const HashMap<keyT, valueT> &other);
    const valueT & operator[](keyT k) const; //constant, for access only
    valueT & operator[](keyT k); //non-constant, for assignment
    bool operator==(const HashMap<keyT, valueT> &other) const;
    bool operator!=(const HashMap<keyT, valueT> &other) const;
};


/**
 * constructor 1
 * @tparam keyT type of HashMap keys
 * @tparam valueT type of HashMap values
 * @tparam KeysInputIterator type of iterator for vector of type keyT
 * @tparam ValuesInputIterator type of iterator for vector of type valueT
 * @param keysBegin begin() iterator for vector of keys
 * @param keysEnd end() iterator for vector of keys
 * @param valuesBegin begin() iterator for vector of values
 * @param valuesEnd end() iterator for vector of values
 */
template<typename keyT, typename valueT>
template<typename KeysInputIterator, typename ValuesInputIterator>
HashMap<keyT, valueT>::HashMap(const KeysInputIterator keysBegin, const KeysInputIterator keysEnd,
                               const ValuesInputIterator valuesBegin,
                               const ValuesInputIterator valuesEnd)
{
    if (std::distance(keysBegin, keysEnd) != std::distance(valuesBegin, valuesEnd))
    {
        throw std::invalid_argument("key and value vectors of different lengths");
    }
    _capacity = 16;
    _size = 0;
    map = new bucket [_capacity];
    auto itVal = valuesBegin;
    for (auto itKey = keysBegin; itKey != keysEnd; itKey++)
    {
        keyT key = *itKey;
        valueT val = *itVal;
        this->operator[](key) = val;
        itVal++;
    }
}

/**
 * default constructor, constructs empty HashMap
 * @tparam keyT type of HashMap keys
 * @tparam valueT type of HashMap values
 */
template<typename keyT, typename valueT>
HashMap<keyT, valueT>::HashMap()
{
    _capacity = 16;
    _size = 0;
    map = new bucket [_capacity];
}

/**
 * copy constructor
 * @tparam keyT
 * @tparam valueT
 * @param other
 */
template<typename keyT, typename valueT>
HashMap<keyT, valueT>::HashMap(HashMap const &other)
{
    _capacity = other.capacity();
    _size = other.size();
    map = new bucket[_capacity];
    for (size_t i = 0; i < _capacity; i++)
    {
        map[i] = other.map[i];
    }
}


/**
 * destructor
 * @tparam keyT
 * @tparam valueT
 */
template<typename keyT, typename valueT>
HashMap<keyT, valueT>::~HashMap()
{
    delete [] map;
}


/**
 * getter for size of HashMap
 * @tparam keyT
 * @tparam valueT
 * @return size_t size
 */
template<typename keyT, typename valueT>
size_t HashMap<keyT, valueT>::size() const
{
    return _size;
}

/**
 * getter for capacity of HashMap
 * @tparam keyT
 * @tparam valueT
 * @return size_t capacity
 */
template<typename keyT, typename valueT>
size_t HashMap<keyT, valueT>::capacity() const
{
    return _capacity;
}

/**
 * checks if HashMap is empty
 * @tparam keyT
 * @tparam valueT
 * @return true if empty, false otherwise
 */
template<typename keyT, typename valueT>
bool HashMap<keyT, valueT>::empty() const
{
    return (_size == 0);
}

/**
 * inserts a key-value pair into HashTable by hashing the key
 * @tparam keyT
 * @tparam valueT
 * @param k key to be hashed
 * @param v corresponding value
 * @return true if successful, false otherwise
 */
template<typename keyT, typename valueT>
bool HashMap<keyT, valueT>::insert(keyT k, valueT v)
{
    std::pair<keyT, valueT> kvPair;
    kvPair.first = k;
    kvPair.second = v;
    int place = getHashTo(k, (int) _capacity);
    //check if key is already in map
    for (std::pair<keyT, valueT> i : map[place])
    {
        if (i.first == k)
        {
            return false;
        }
    }
    //push key-value pair into vector
    map[place].push_back(kvPair);
    _size++;
    double loadFactor = load_factor();
    if (loadFactor > MAX_LOAD_FACTOR)
    {
        rehash(true);
    }
    return true;
}

/**
 * checks if a key has been hashed to the table
 * @tparam keyT
 * @tparam valueT
 * @param k key to check for
 * @return true if key is in HashTable, else false
 */
template<typename keyT, typename valueT>
bool HashMap<keyT, valueT>::contains_key(keyT k) const
{
    int place = getHashTo(k, (int) _capacity);
    for (std::pair<keyT, valueT> i : map[place])
    {
        if (i.first == k)
        {
            return true;
        }
    }
    return false;
}

/**
 * gets the value corresponding to the given key
 * @tparam keyT
 * @tparam valueT
 * @param k key whose value to get
 * @return valueT: the value
 */
template<typename keyT, typename valueT>
valueT & HashMap<keyT, valueT>::at(const keyT & k)
{
    if (!contains_key(k))
    {
        throw std::out_of_range("key not in HashTable");
    }
    int place = getHashTo(k, _capacity);
    for (auto it = map[place].begin(); it != map[place].end(); it++)
    {
        std::pair<keyT, valueT> &cur = *it;
        if (cur.first == k)
        {
            return cur.second;
        }
    }
    assert(false);
}

/**
 * gets the value corresponding to the given key
 * @tparam keyT
 * @tparam valueT
 * @param k key whose value to get
 * @return valueT: the value
 */
template<typename keyT, typename valueT>
const valueT & HashMap<keyT, valueT>::at(const keyT & k) const
{
    if (!contains_key(k))
    {
        throw std::out_of_range("key not in HashTable");
    }
    int place = getHashTo(k, _capacity);
    for (auto it = map[place].begin(); it != map[place].end(); it++)
    {
        std::pair<keyT, valueT> &cur = *it;
        if (cur.first == k)
        {
            return cur.second;
        }
    }
    assert(false);
}

/**
 * erases a given value from the HashMap
 * @tparam keyT
 * @tparam valueT
 * @param k key to erase
 * @return true if successful, false if key not found in the map
 */
template<typename keyT, typename valueT>
bool HashMap<keyT, valueT>::erase(keyT k)
{
    int place = getHashTo(k, _capacity);
    int x = 0;
    for (std::pair<keyT, valueT> i : map[place])
    {
        if (i.first == k)
        {
            map[place].erase(map[place].begin() + x, map[place].begin() + x + 1);
            _size--;
            double loadFactor = load_factor();
            if (loadFactor < MIN_LOAD_FACTOR)
            {
                rehash(false);
            }
            return true;
        }
        x++;
    }
    return false; //key not found
}

/**
 * calculates the current load factor
 * @tparam keyT
 * @tparam valueT
 * @return double: load factor
 */
template<typename keyT, typename valueT>
double HashMap<keyT, valueT>::load_factor() const
{
    double result = ((double) size()) / capacity();
    return result;
}

/**
 * get the size of the bucket containing a given key
 * @tparam keyT
 * @tparam valueT
 * @param k key to check for
 * @return the size of the bucket containing k
 */
template<typename keyT, typename valueT>
size_t HashMap<keyT, valueT>::bucket_size(keyT k) const
{
    int place = getHashTo(k, (int) _capacity);
    for (std::pair<keyT, valueT> i : map[place])
    {
        if (i.first == k)
        {
            return map[place].size();
        }
    }
    throw std::invalid_argument("key not in HashMap");
}

/**
 * get the index of the bucket containing a given key
 * @tparam keyT
 * @tparam valueT
 * @param k key to check for
 * @return return the index of the bucket in the map
 */
template<typename keyT, typename valueT>
size_t HashMap<keyT, valueT>::bucket_index(keyT k) const
{
    int place = getHashTo(k, (int) _capacity);
    for (std::pair<keyT, valueT> i : map[place])
    {
        if (i.first == k)
        {
            return place;
        }
    }
    throw std::invalid_argument("key not in HashMap");
}

/**
 * clears the HashMap
 * @tparam keyT
 * @tparam valueT
 */
template<typename keyT, typename valueT>
void HashMap<keyT, valueT>::clear()
{
    //iterate over capacity and clear each vector
    for (size_t i = 0; i < _capacity; i++)
    {
        map[i].clear();
    }
    _size = 0;
}

/**
 * overloaded assignment operator for HashMaps
 * @tparam keyT
 * @tparam valueT
 * @param other
 * @return
 */
template<typename keyT, typename valueT>
HashMap<keyT, valueT> & HashMap<keyT, valueT>::operator=(const HashMap<keyT, valueT> &other)
{
    _capacity = other.capacity();
    _size = other.size();
    map = new bucket[_capacity];
    for (size_t i = 0; i < _capacity; i++)
    {
        map[i] = other.map[i];
    }
    return *this;
}

/**
 * const version of overloaded subscript operator
 * @tparam keyT
 * @tparam valueT
 * @param k key to look up
 * @return value corresponding to k
 */
template<typename keyT, typename valueT>
const valueT & HashMap<keyT, valueT>::operator[](const keyT k) const
{
    int place = getHashTo(k, _capacity);
    for (auto it = map[place].begin(); it != map[place].end(); it++)
    {
        std::pair<keyT, valueT> &cur = *it;
        if (cur.first == k) {
            return cur.second;
        }
    }
    static valueT ret;
    return ret;
}

/**
 * overloaded subscript operator
 * @tparam keyT
 * @tparam valueT
 * @param k key
 * @return value corresponding to k
 */
template<typename keyT, typename valueT>
valueT & HashMap<keyT, valueT>::operator[](keyT k)
{
    int place = getHashTo(k, _capacity);
    for (auto it = map[place].begin(); it != map[place].end(); it++)
    {
        std::pair<keyT, valueT> &cur = *it;
        if (cur.first == k) {
            return cur.second;
        }
    }
    //if key not found in map
    valueT *val = new valueT;
    insert(k, *val);
    return at(k);
}

/**
 * overloaded equality operator
 * @tparam keyT
 * @tparam valueT
 * @param other
 * @return true if this==other, false otherwise
 */
template<typename keyT, typename valueT>
bool HashMap<keyT, valueT>::operator==(const HashMap<keyT, valueT> &other) const
{
    if (size() != other.size() || capacity() != other.capacity())
    {
        return false;
    }
    for (size_t i = 0; i < capacity(); i++)
    {
        //check if bucket i contains same elements in both HashMaps
        if (!(std::is_permutation(map[i].begin(), map[i].end(), other.map[i].begin())))
        {
            return false;
        }
    }
    return true;
}

/**
 * overloaded inequality operator
 * @tparam keyT
 * @tparam valueT
 * @param other
 * @return true if this!=other, false otherwise
 */
template<typename keyT, typename valueT>
bool HashMap<keyT, valueT>::operator!=(const HashMap<keyT, valueT> &other) const
{
    if (size() != other.size() || capacity() != other.capacity())
    {
        return true;
    }
    for (int i = 0; i < capacity(); i++)
    {
        if (!(std::is_permutation(map[i].begin(), map[i].end(), other.map[i].begin())))
        {
            return true;
        }
    }
    return false;
}

/**
 * change capacity of HashMap and rehash values to fit new capacity
 * @tparam keyT
 * @tparam valueT
 * @param expand true if expanding map capacity, false if shrinking (default false)
 */
template<typename keyT, typename valueT>
void HashMap<keyT, valueT>::rehash(bool expand)
{
    size_t oldCapacity = _capacity;
    if (expand) //expanding, rehash called by insert
    {
        _capacity *= 2;
    }
    else //shrinking, rehash called by erase
    {
        _capacity /= 2;
    }
    auto *newMap = new bucket[_capacity];
    for (size_t i = 0; i < oldCapacity; i++)//iterate over buckets in old map
    {
        for (auto it = map[i].begin(); it != map[i].end(); it++)//iterate over bucket of pairs
        {
            std::pair<keyT, valueT> curPair = *it;
            int place = getHashTo(curPair.first, _capacity);
            newMap[place].push_back(curPair);
        }
    }
    map = newMap;
}


/**
 * iterator class for HashMap class
 * @tparam keyT
 * @tparam valueT
 */
template<typename keyT, typename valueT>
class HashMap<keyT, valueT>::iterator
{
    const HashMap<keyT, valueT> *myMap;
    typename bucket::iterator iter;
    int curBucketIndex; //index of current vector in myMap
public:
    typedef std::pair<keyT, valueT> value_type;
    typedef const std::pair<keyT, valueT>& reference;
    typedef const std::pair<keyT, valueT>* pointer;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

    value_type operator*()
    {
        return *iter;
    }

    iterator & operator++()
    {
        iter++;
        if (iter == myMap->map[curBucketIndex].end()) //iter has reached end of current vector
        {
            iter = myMap->map[++curBucketIndex].begin();
        }
        return *this;
    }

    iterator operator++(int)
    {
        iterator tmp = *this;
        iter++;
        if (iter == myMap->map[curBucketIndex].end()) //iter has reached end of current vector
        {
            iter = myMap->map[++curBucketIndex].begin();
        }
        return tmp;
    }

    friend bool operator==(iterator lhs, iterator rhs)
    {
        return lhs.iter == rhs.iter;
    }

    friend bool operator!=(iterator lhs, iterator rhs)
    {
        return lhs.iter != rhs.iter;
    }

    pointer operator->()
    {
        return &(*iter);
    }

    iterator() = default;

    iterator(const HashMap<keyT, valueT> *hashMap, bool begin = true)
    {
        myMap = hashMap;
        if (begin)
        {
            curBucketIndex = 0;
            iter = myMap->map[0].begin();
        }
        else
        {
            //returning end() iterator - end() of bucket of smallest index for which
            //all subsequent buckets are empty
            size_t sizeCount = 0;
            for (int i = 0; i < myMap->capacity(); i++)
            {
                sizeCount += myMap->map[i].size();
                if (sizeCount == myMap->size())
                {
                    iter = myMap->map[i].end();
                }
            }
        }
    }
};

/**
 * gets iterator at start of HashMap
 * @tparam keyT
 * @tparam valueT
 * @return
 */
template<typename keyT, typename valueT>
typename HashMap<keyT, valueT>::iterator HashMap<keyT, valueT>::begin() const
{
    return iterator(this);
}

/**
 * gets end() iterator for HashMap
 * @tparam keyT
 * @tparam valueT
 * @return
 */
template<typename keyT, typename valueT>
typename HashMap<keyT, valueT>::iterator HashMap<keyT, valueT>::end() const
{
    return iterator(this, false);
}

/**
 * gets constant iterator at start of HashMap
 * @tparam keyT
 * @tparam valueT
 * @return
 */
template<typename keyT, typename valueT>
typename HashMap<keyT, valueT>::iterator HashMap<keyT, valueT>::cbegin() const
{
    return iterator(this);
}

/**
 * gets constant end() iterator for HashMap
 * @tparam keyT
 * @tparam valueT
 * @return
 */
template<typename keyT, typename valueT>
typename HashMap<keyT, valueT>::iterator HashMap<keyT, valueT>::cend() const
{
    return iterator(this, false);
}


/**
 * get the index that the given key should be hashed to in a HashMap of the given capacity
 * @tparam keyT
 * @param k
 * @param cap
 * @return index
 */
template<typename keyT>
int getHashTo(keyT k, int cap)
{
    int x = std::hash<keyT>() (k);
    auto hashFunc = [cap](int x) ->int
    {
        return (x & (cap - 1));
    };
    int place = hashFunc(x);
    return place;
}

#endif //EX6_LIBRARY_H

