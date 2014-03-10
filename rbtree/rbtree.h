//
//  rbtree.h
//  rbtree
//
//  Created by Joseph on 10/03/14.
//  Copyright (c) 2014 J A Mark. All rights reserved.
//

#ifndef __rbtree__rbtree__
#define __rbtree__rbtree__

#include <iostream>
#include <cassert>
#include <memory>
#include <initializer_list>

#include "List.h"

enum Color
{
    NEGATIVE_BLACK = -1,
    RED = 0,
    BLACK = 1,
    DOUBLE_BLACK = 2
};

inline Color &operator++(Color &c) {
    assert(c != DOUBLE_BLACK);
    c = static_cast<Color>(c + 1);
    return c;
}

inline Color operator++(Color &c, int) {
    assert(c != DOUBLE_BLACK);
    ++c;
    return static_cast<Color>(c - 1);
}

inline Color &operator--(Color &c) {
    assert(c != NEGATIVE_BLACK);
    return c = static_cast<Color>(c - 1);
}

inline Color operator--(Color &c, int) {
    assert(c != NEGATIVE_BLACK);
    --c;
    return static_cast<Color>(c + 1);
}

template<class T, class U>
class RBTree
{
    struct Node
    {
        Node(Color c,
             std::shared_ptr<const Node> const & lft,
             T val,
             List<U> items,
             std::shared_ptr<const Node> const & rgt)
        : c_(c), lft_(lft), val_(val), items_(items), rgt_(rgt)
        {}
        Color c_;
        std::shared_ptr<const Node> lft_;
        T val_;
        List<U> items_;
        std::shared_ptr<const Node> rgt_;
    };
    
    explicit RBTree(std::shared_ptr<const Node> const & node)
    : root_(node)
    {}
    
public:
    
    RBTree() {} // empty tree
    
    RBTree(Color c, RBTree const & lft, T val, List<U> items, RBTree const & rgt)
    : root_(std::make_shared<const Node>(c, lft.root_, val, items, rgt.root_))
    {
        assert(lft.isEmpty() || lft.value() < val);
        assert(rgt.isEmpty() || val < rgt.value());
    }
    
    RBTree(std::initializer_list<T> init)
    {
        RBTree t;
        for (T v : init)
        {
            t = t.insert(v);
        }
        root_ = t.root_;
    }
    
    struct Contents
    {
        T value_;
        List<U> items_;
    public:
        Contents()
        : value_(-1), items_(List<U>())
        {}
        Contents(T value, List<U> items)
        : value_(value), items_(items)
        {}
        T value() const { return value_; }
        List<U> items() const { return items_; }
    };
    
    bool isEmpty() const
    {
        return !root_;
    }
    
    T value() const
    {
        assert(!isEmpty());
        return root_->val_;
    }
    
    Color rootColor() const
    {
        assert (!isEmpty());
        return root_->c_;
    }
    
    List<U> items() const
    {
        assert(!isEmpty());
        return root_->items_;
    }
    
    RBTree left() const
    {
        assert(!isEmpty());
        return RBTree(root_->lft_);
    }
    
    RBTree right() const
    {
        assert(!isEmpty());
        return RBTree(root_->rgt_);
    }
    
    RBTree insert(T x, U item) const
    {
        RBTree t = ins(x, item);
        return RBTree(BLACK, t.left(), t.value(), t.items(), t.right());
    }
    
    RBTree remove(T x) const
    {
        if (value() == x && childless()){
            return RBTree();
        }
        RBTree r = rem(x);
        return RBTree(BLACK, r.left(), r.value(), r.items(), r.right());
    }
    
    RBTree remove(T x, U item) const
    {
        if (value() == x && childless()){
            List<U> n = items().remove(item);
            if (n.size() < 1){
                return RBTree();
            } else {
                return RBTree<T, U>(rootColor(), left(), value(), n, right());
            }
        }
        RBTree r = rem(x, item);
        return RBTree(BLACK, r.left(), r.value(), r.items(), r.right());
    }
    
    bool member(T x) const
    {
        if (isEmpty())
            return false;
        T y = value();
        if (x < y)
            return left().member(x);
        else if (y < x)
            return right().member(x);
        else
            return true;
    }
    
    List<U> getItems(T x)
    {
        if (isEmpty()){
            return NULL;
        }
        T y = value();
        if (y > x){
            return left().getItems(x);
        } else if (x > y){
            return right().getItems(x);
        } else {
            return items();
        }
    }
    
    Contents getNodeJustGreaterThan(T x)
    {
        if (isEmpty()){
            return Contents();
        }
        T y = value();
        if (y > x){
            if (left().isEmpty()){
                return Contents(value(), items());
            } else {
                Contents lft = left().getNodeJustGreaterThan(x);
                return lft.value() < value() ? lft : Contents(value(), items());
            }
        } else {
            if (right().isEmpty()){
                return Contents();
            } else {
                return right().getNodeJustGreaterThan(x);
            }
        }
    }
    
    void checkRed() const
    {
        if (!isEmpty())
        {
            auto lft = left();
            auto rgt = right();
            if (rootColor() == RED)
            {
                assert(lft.isEmpty() || lft.rootColor() == BLACK);
                assert(rgt.isEmpty() || rgt.rootColor() == BLACK);
            }
            lft.checkRed();
            rgt.checkRed();
        }
    }
    
    int countBlack() const
    {
        if (isEmpty())
            return 0;
        int lft = left().countBlack();
        int rgt = right().countBlack();
        assert(lft == rgt);
        int blk;
        if (rootColor() == BLACK){
            blk = 1;
        } else if (rootColor() == DOUBLE_BLACK){
            blk = 2;
        } else if (rootColor() == NEGATIVE_BLACK){
            blk = -1;
        } else {
            blk = 0;
        }
        return blk + lft;
    }
    
    void printVal() const
    {
        std::cout << value() << std::endl;
    }
    
    void printTree() const
    {
        std::cout << "Checking tree..." <<std::endl;
        checkRed();
        countBlack();
        std::cout << "Printing tree..." <<std::endl;
        prTree();
    }
    
private:
    
    template<class Y, class Z>
    struct StateContainer
    {
        StateContainer(Y y, Z z)
        : y_(y), z_(z)
        {}
        Y y_;
        Z z_;
    };
    
    RBTree ins(T x, U item) const
    {
        checkRed();
        if (isEmpty()) {
            return RBTree(RED, RBTree(), x, List<U>(item, List<U>()), RBTree());
        }
        T y = value();
        List<U> yi = items();
        Color c = rootColor();
        if (x < y)
            return balance(c, left().ins(x, item), y, yi, right());
        else if (y < x)
            return balance(c, left(), y, yi, right().ins(x, item));
        else
            return RBTree(c, left(), y, yi.push_front(item), right());
    }
    
    RBTree rem(T x) const
    {
        T y = value();
        List<U> yi = items();
        Color c = rootColor();
        if (x < y) {
            return balance(c, left().rem(x), y, yi, right());
        } else if (y < x) {
            return balance(c, left(), y, yi, right().rem(x));
        } else {
            if (!left().isEmpty()){
                StateContainer<RBTree, RBTree> s = left().getRemoveMax();
                return balance(c, s.y_, s.z_.value(), s.z_.items(), right());
            } else if (!right().isEmpty()){
                StateContainer<RBTree, RBTree> s = right().getRemoveMin();
                return balance(c, left(), s.z_.value(), s.z_.items(), s.y_);
            } else {
                return this->paint(DOUBLE_BLACK);
            }
        }
    }
    
    RBTree rem(T x, U item) const
    {
        T y = value();
        List<U> yi = items();
        Color c = rootColor();
        if (x < y) {
            return balance(c, left().rem(x, item), y, yi, right());
        } else if (y < x) {
            return balance(c, left(), y, yi, right().rem(x, item));
        } else {
            List<U> n = items().remove(item);
            if (!n.isEmpty()){
                return RBTree<T, U>(c, left(), y, n, right());
            } else {
                if (!left().isEmpty()){
                    StateContainer<RBTree, RBTree> s = left().getRemoveMax();
                    return balance(c, s.y_, s.z_.value(), s.z_.items(), right());
                } else if (!right().isEmpty()){
                    StateContainer<RBTree, RBTree> s = right().getRemoveMin();
                    return balance(c, left(), s.z_.value(), s.z_.items(), s.y_);
                } else {
                    return this->paint(DOUBLE_BLACK);
                }
            }
        }
    }
    
    StateContainer<RBTree, RBTree> getRemoveMax() const
    {
        if (right().isEmpty()){
            if (left().isEmpty()){
                if (rootColor() == BLACK){
                    return StateContainer<RBTree, RBTree>(this->paint(DOUBLE_BLACK), *this);
                } else {
                    return StateContainer<RBTree, RBTree>(RBTree(), *this);
                }
            } else {
                return StateContainer<RBTree, RBTree>(left().paint(BLACK), *this);
            }
        } else {
            StateContainer<RBTree, RBTree> s = right().getRemoveMax();
            return StateContainer<RBTree, RBTree>(balance(rootColor(), left(), value(), items(), s.y_), s.z_);
        }
    }
    
    StateContainer<RBTree, RBTree> getRemoveMin() const
    {
        if (left().isEmpty()){
            if (right().isEmpty()){
                if (rootColor() == BLACK){
                    return StateContainer<RBTree, RBTree>(this->paint(DOUBLE_BLACK), *this);
                } else {
                    return StateContainer<RBTree, RBTree>(RBTree(), *this);
                }
            } else {
                return StateContainer<RBTree, RBTree>(right().paint(BLACK), *this);
            }
        } else {
            StateContainer<RBTree, RBTree> s = left().getRemoveMin();
            return StateContainer<RBTree, RBTree>(balance(rootColor(), s.y_, value(), items(), right()),
                                                  s.z_);
        }
    }
    
    static RBTree balance(Color currColor, RBTree const & lft, T x, List<U> xi, RBTree const & rgt)
    {
        if (lft.doubleBlack()){
            if (lft.childless()){
                return bubble(currColor, RBTree(), x, xi, rgt);
            } else {
                return bubble(currColor, lft, x, xi, rgt);
            }
        } else if (rgt.doubleBlack()){
            if (rgt.childless()){
                return bubble(currColor, lft, x, xi, RBTree());
            } else {
                return bubble(currColor, lft, x, xi, rgt);
            }
        }
        
        if (currColor == RED){
            return RBTree(currColor, lft, x, xi, rgt);
        }
        
        Color c = currColor == BLACK ? RED : BLACK;
        if (lft.negative()){
            return RBTree(c,
                          balance(BLACK,
                                  lft.left().paint(RED),
                                  lft.value(),
                                  lft.items(),
                                  lft.right().left()),
                          lft.right().value(),
                          lft.right().items(),
                          RBTree(BLACK,
                                 lft.right().right(),
                                 x,
                                 xi,
                                 rgt));
        } else if (rgt.negative()){
            return RBTree(c,
                          RBTree(BLACK,
                                 rgt.left().left(),
                                 x,
                                 xi,
                                 rgt),
                          rgt.left().value(),
                          rgt.left().items(),
                          balance(BLACK,
                                  rgt.right().paint(RED),
                                  rgt.value(),
                                  rgt.items(),
                                  rgt.left().right()));
        } else if (lft.doubledLeft()){
            return RBTree(c,
                          lft.left().paint(BLACK),
                          lft.value(),
                          lft.items(),
                          RBTree(BLACK, lft.right(), x, xi, rgt));
        } else if (lft.doubledRight()){
            return RBTree(c,
                          RBTree(BLACK, lft.left(), lft.value(), lft.items(), lft.right().left()),
                          lft.right().value(),
                          lft.right().items(),
                          RBTree(BLACK, lft.right().right(), x, xi, rgt));
        } else if (rgt.doubledLeft()){
            return RBTree(c,
                          RBTree(BLACK, lft, x, xi, rgt.left().left()),
                          rgt.left().value(),
                          rgt.left().items(),
                          RBTree(BLACK, rgt.left().right(), rgt.value(), rgt.items(), rgt.right()));
        } else if (rgt.doubledRight()){
            return RBTree(c,
                          RBTree(BLACK, lft, x, xi, rgt.left()),
                          rgt.value(),
                          rgt.items(),
                          rgt.right().paint(BLACK));
        } else {
            return RBTree(++c, lft, x, xi, rgt);
        }
    }
    
    static RBTree bubble(Color currColor, RBTree const & lft, T x, List<U> xi, RBTree const & rgt)
    {
        if (!lft.isEmpty() && !rgt.isEmpty()){
            return balance(++currColor, lft.paint((Color)(lft.rootColor() - 1)), x, xi, rgt.paint((Color)(rgt.rootColor() - 1)));
        } else if (!lft.isEmpty()){
            return balance(++currColor, lft.paint((Color)(lft.rootColor() - 1)), x, xi, RBTree());
        } else if (!rgt.isEmpty()){
            return balance(++currColor, RBTree(), x, xi, rgt.paint((Color)(rgt.rootColor() - 1)));
        } else {
            return balance(++currColor, RBTree(), x, xi, RBTree());
        }
    }
    
    bool childless() const
    {
        assert(!isEmpty());
        return left().isEmpty() && right().isEmpty();
    }
    
    bool doubleBlack() const
    {
        return !isEmpty() && rootColor() == DOUBLE_BLACK;
    }
    
    bool negative() const
    {
        return !isEmpty() && rootColor() == NEGATIVE_BLACK;
    }
    
    bool doubledLeft() const
    {
        return !isEmpty()
        && rootColor() == RED
        && !left().isEmpty()
        && left().rootColor() == RED;
    }
    
    bool doubledRight() const
    {
        return !isEmpty()
        && rootColor() == RED
        && !right().isEmpty()
        && right().rootColor() == RED;
    }
    
    RBTree paint(Color c) const
    {
        assert(!isEmpty());
        return RBTree(c, left(), value(), items(), right());
    }
    
    void prTree() const
    {
        if (isEmpty()){
            return;
        }
        if (!left().isEmpty()){
            left().prTree();
        }
        printVal();
        print(items());
        if (!right().isEmpty()){
            right().prTree();
        }
    }
    
    std::shared_ptr<const Node> root_;
};

#endif /* defined(__rbtree__rbtree__) */
