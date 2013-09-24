//
//  LinkedList.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 06/11/2012.
//  Copyright (c) 2013. All rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// For affordable commercial licensing please contact nyhl@ngrawlings.com
//

#ifndef PeerConnector_LinkedList_h
#define PeerConnector_LinkedList_h

#include <libnrcore/base/Object.h>

#define LINKEDLIST_NODE_HANDLE  void*

namespace nrcore {

    template <class T>
    class LinkedList : public Object {
    public:
        
        LinkedList<T>() : _first(0), count(0) {}
        
        LinkedList<T>(const LinkedList<T>& orig) : _first(0), count(0) { // Copy constructor
            copy(orig);
        }
        
        ~LinkedList() {
            clear();
        }
        
        LINKEDLIST_NODE_HANDLE add(const T obj) {
            ENTRY* entry = new ENTRY;
            entry->obj = obj;
            if (!count) {
                entry->prev = entry;
                entry->next = entry;
                count = 1;
                _first = entry;
            } else {
                entry->prev = _first->prev;
                entry->next = _first;
                _first->prev->next = entry;
                _first->prev = entry;
                count++;
            }
            return entry;
        }
        
        LINKEDLIST_NODE_HANDLE nextNode(LINKEDLIST_NODE_HANDLE node) {
            return ((ENTRY*)node)->next;
        }
        
        LINKEDLIST_NODE_HANDLE prevNode(LINKEDLIST_NODE_HANDLE node) {
            return ((ENTRY*)node)->prev;
        }
        
        LINKEDLIST_NODE_HANDLE firstNode() {
            return _first;
        }
        
        LINKEDLIST_NODE_HANDLE lastNode() {
            return _first ? _first->prev : 0;
        }
        
        T &get(LINKEDLIST_NODE_HANDLE node) {
            return ((ENTRY*)node)->obj;
        }
        
        T &get(int index) {
            ENTRY *node = _first;
            while (index--)
                node = node->next;
            return ((ENTRY*)node)->obj;
        }
        
        void set(LINKEDLIST_NODE_HANDLE node, T obj) {
            ((ENTRY*)node)->obj = obj;
        }
        
        void set(int index, T obj) {
            ENTRY *node = _first;
            while (index--)
                node = node->next;
            ((ENTRY*)node)->obj = obj;
        }
        
        void remove(int index) {
            ENTRY *node = _first;
            while (index--)
                node = node->next;
            remove(node);
        }
        
        void remove(LINKEDLIST_NODE_HANDLE node) {
            if (!node)
                return;
            
            if (_first == ((ENTRY*)node)) {
                if (count > 1)
                    _first = _first->next;
                else
                    _first = 0;
            }
            
            if (((ENTRY*)node)->prev)
                ((ENTRY*)node)->prev->next = ((ENTRY*)node)->next;
            
            if (((ENTRY*)node)->next)
                ((ENTRY*)node)->next->prev = ((ENTRY*)node)->prev;
            
            delete ((ENTRY*)node);
            
            count--;
        }
        
        void remove(const T &obj) {
            if (!_first)
                return;
            
            ENTRY *node = _first->next;
            
            if (obj == _first->obj) {
                remove(_first);
                return;
            }
            
            while (node!=_first) {
                if (obj == node->obj) {
                    remove(node);
                    break;
                }
                node = node->next;
            }
        }
        
        void clear() {
            ENTRY *tmp;
            if (_first && count) {
                ENTRY *current = _first;
                _first->next = 0;
                
                while(current) {
                    tmp = current;
                    current = current->next;
                    delete tmp;
                }
            }
            _first = 0;
            count = 0;
        }
        
        void copy(const LinkedList<T> *lst) {
            copy(*lst);
        }
        
        void copy(const LinkedList<T> &lst) {
            clear();
            append(lst);
        }
        
        void append(const LinkedList<T> *lst) {
            append(*lst);
        }
        
        void append(const LinkedList<T> &lst) {
            LINKEDLIST_NODE_HANDLE node = ((LinkedList<T>&)lst).firstNode();
            if (node)
                do {
                    add(((LinkedList<T>&)lst).get(node));
                } while ((node = ((LinkedList<T>&)lst).nextNode(node)) && node != ((LinkedList<T>&)lst).firstNode());
        }
        
        int length() {
            return count;
        }
        
        LinkedList<T> &operator =(LinkedList<T> &list) {
            copy(&list);
            return *this;
        }
        
        const LinkedList<T> &operator =(const LinkedList<T> &list) {
            copy(&list);
            return *this;
        }
        
    protected:
        typedef struct ENTRY {
            ENTRY* next;
            ENTRY* prev;
            T obj;
        } ENTRY;
        ENTRY *_first;
        int count;
    };

    template <class T>
    class LinkedListState {
    public:
        LinkedListState(LinkedList<T> *list) {
            this->list = list;
            node = list->firstNode();
        }
        
        LinkedListState(const LinkedListState<T> &list) {
            this->list = list.list;
            this->node = node;
        }
        
        T& first() {
            node = list->firstNode();
            return list->get(node);
        }
        
        T& next() {
            node = list->nextNode(node);
            return list->get(node);
        }
        
        T& prev() {
            node = list->prevNode(node);
            return list->get(node);
        }
        
        T& last() {
            node = list->lastNode();
            return list->get(node);
        }
        
        T& get() {
            return list->get(node);
        }
        
        void remove() {
            LINKEDLIST_NODE_HANDLE tmp = node;
            
            if (list->length() > 1) {
                node = list->nextNode(node);
            } else {
                node = 0;
            }
            
            list->remove(tmp);
        }
        
        int length() {
            return list->length();
        }
        
    protected:
        LinkedList<T> *list;
        LINKEDLIST_NODE_HANDLE node;
    };
    
}

#endif
