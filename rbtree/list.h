//
//  list.h
//  rbtree
//
//  Created by Joseph on 10/03/14.
//  Copyright (c) 2014 J A Mark. All rights reserved.
//

#ifndef __rbtree__list__
#define __rbtree__list__

#include <iostream>
#include <cassert>
#include <functional>
#include <initializer_list>
//#include <type_traits>

template<class T>
class List
{
    struct Item
    {
        Item(T v, Item const * tail) : val_(v), next_(tail) {}
        T val_;
        Item const * next_;
    };
    friend Item;
    explicit List (Item const * items) : head_(items) {}
public:
    
    // Empty list
    List() : head_(nullptr) {}
    
    // Cons
    List(T v, List tail) : head_(new Item(v, tail.head_)) {}
    
    // From initializer list
    List(std::initializer_list<T> init) : head_(nullptr)
    {
        for (auto it = std::begin(init); it != std::end(init); ++it)
        {
            head_ = new Item(*it, head_);
        }
    }
    
    bool isEmpty() const
    {
        return !head_;
    }
    
    T front() const
    {
        assert(!isEmpty());
        return head_->val_;
    }
    
    int size() const
    {
        return foldr([](T v, int u){ return 1 + u; }, 0, *this);
    }
    
    List pop_front() const
    {
        assert(!isEmpty());
        return List(head_->next_);
    }
    
    List push_front(T v) const
    {
        return List(v, *this);
    }
    
    List insertAt(int i, T v) const
    {
        if (i == 0){
            return push_front(v);
        } else {
            assert(!isEmpty());
            return List(front(), pop_front().insertAt(i - 1, v));
        }
    }
    
    List removeAt(int i) const
    {
        if (i == 0){
            return pop_front();
        } else {
            assert(!isEmpty());
            return List(front(), pop_front().removeAt(i - 1));
        }
    }
    
    List remove(T v) const
    {
        if (v == head_->val_){
            return pop_front();
        } else {
            assert(!isEmpty());
            return List(front(), pop_front().remove(v));
        }
    }
    
private:
    Item const * head_;
};

template<class T>
List<T> concat(List<T> a, List<T> b)
{
    if (a.isEmpty())
        return b;
    return List<T>(a.front(), concat(a.pop_front(), b));
}

template<class U, class T, class F>
List<U> fmap(F f, List<T> lst)
{
    static_assert(std::is_convertible<F, std::function<U(T)>>::value,
                  "fmap requires a function type U(T)");
    if (lst.isEmpty())
        return List<U>();
    else
        return List<U>(f(lst.front()), fmap<U>(f, lst.pop_front()));
}

template<class T, class P>
List<T> filter(P p, List<T> lst)
{
    static_assert(std::is_convertible<P, std::function<bool(T)>>::value,
                  "filter requires a function type bool(T)");
    if (lst.isEmpty())
        return List<T>();
    if (p(lst.front()))
        return List<T>(lst.front(), filter(p, lst.pop_front()));
    else
        return filter(p, lst.pop_front());
}

template<class T, class U, class F>
U foldr(F f, U acc, List<T> lst)
{
    static_assert(std::is_convertible<F, std::function<U(T, U)>>::value,
                  "foldr requires a function type U(T, U)");
    if (lst.isEmpty())
        return acc;
    else
        return f(lst.front(), foldr(f, acc, lst.pop_front()));
}

template<class T, class U, class F>
U foldl(F f, U acc, List<T> lst)
{
    static_assert(std::is_convertible<F, std::function<U(U, T)>>::value,
                  "foldl requires a function type U(U, T)");
    if (lst.isEmpty())
        return acc;
    else
        return foldl(f, f(acc, lst.front()), lst.pop_front());
}

template<class T, class F>
void forEach(List<T> lst, F f)
{
    static_assert(std::is_convertible<F, std::function<void(T)>>::value,
                  "forEach requires a function type void(T)");
    if (!lst.isEmpty()) {
        f(lst.front());
        forEach(lst.pop_front(), f);
    }
}

template<class Beg, class End>
auto fromIt(Beg it, End end) -> List<typename Beg::value_type>
{
    typedef typename Beg::value_type T;
    if (it == end){
        return List<T>();
        T item = *it;
        return List<T>(item, fromIt(++it, end));
    }
}

template<class T>
void print(List<T> lst)
{
    std::cout << "[ ";
    forEach(lst, [](T v)
            {
                std::cout << v << " ";
            });
    std::cout << "] " << std::endl;
}

#endif /* defined(__rbtree__list__) */
