/*
 * Copyright (C) 2013-2018 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Bag_h
#define Bag_h

#include <wtf/DumbPtrTraits.h>
#include <wtf/FastMalloc.h>
#include <wtf/Noncopyable.h>

namespace WTF {

namespace Private {

template<typename T>
class BagNode {
    WTF_MAKE_FAST_ALLOCATED;
public:
    template<typename... Args>
    BagNode(Args&&... args)
        : m_item(std::forward<Args>(args)...)
    { }
    
    T m_item;
    BagNode* m_next { nullptr };
};

} // namespace Private

template<typename T, typename PtrTraits = DumbPtrTraits<Private::BagNode<T>>>
class Bag {
    WTF_MAKE_NONCOPYABLE(Bag);
    WTF_MAKE_FAST_ALLOCATED;
    using Node = Private::BagNode<T>;

public:
    Bag() = default;

    template<typename U>
    Bag(Bag<T, U>&& other)
    {
        ASSERT(!m_head);
        m_head = other.unwrappedHead();
        other.m_head = nullptr;
    }

    template<typename U>
    Bag& operator=(Bag<T, U>&& other)
    {
        m_head = other.unwrappedHead();
        other.m_head = nullptr;
        return *this;
    }
    
    ~Bag()
    {
        clear();
    }
    
    void clear()
    {
        Node* head = this->unwrappedHead();
        while (head) {
            Node* current = head;
            head = current->m_next;
            delete current;
        }
        m_head = nullptr;
    }
    
    template<typename... Args>
    T* add(Args&&... args)
    {
        Node* newNode = new Node(std::forward<Args>(args)...);
        newNode->m_next = unwrappedHead();
        m_head = newNode;
        return &newNode->m_item;
    }
    
    class iterator {
    public:
        iterator()
            : m_node(0)
        {
        }
        
        // This is sort of cheating; it's equivalent to iter == end().
        bool operator!() const { return !m_node; }
        
        T* operator*() const { return &m_node->m_item; }
        
        iterator& operator++()
        {
            m_node = m_node->m_next;
            return *this;
        }
        
        bool operator==(const iterator& other) const
        {
            return m_node == other.m_node;
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }

    private:
        template<typename, typename> friend class WTF::Bag;
        Node* m_node;
    };
    
    iterator begin()
    {
        iterator result;
        result.m_node = unwrappedHead();
        return result;
    }
    
    iterator end() { return iterator(); }
    
    bool isEmpty() const { return !m_head; }
    
private:
    Node* unwrappedHead() { return PtrTraits::unwrap(m_head); }

    typename PtrTraits::StorageType m_head { nullptr };
};

template<typename Poison, typename T> struct PoisonedPtrTraits;

template<typename Poison, typename T>
using PoisonedBag = Bag<T, PoisonedPtrTraits<Poison, Private::BagNode<T>>>;

} // namespace WTF

using WTF::Bag;
using WTF::PoisonedBag;

#endif // Bag_h

