/****************************************************************************************************
 * Copyright (c) 2013
 ****************************************************************************************************
 * @file: List.h
 *
 * @description: template class for a single concatenated list
 *
 * @date: Jun 23, 2012
 * @author: ou shile
 * @last modified by:
 *        $Author: 
 ****************************************************************************************************
 */

#ifndef LIST_H_
#define LIST_H_

#include <assert.h>

#include <iostream>

namespace SYS
{
   /**
    *  Template class for list
    */
   template<class T>
   class List
   {
      public:
         /**
          * class for a single item in the list
          */
         class item
         {
            public:
               T _data;
               item * _next;

            public:
               item(const T & e, item * n = NULL) :
                  _data(e), _next(n)
               {
               }
         };

         /**
          * class for an iterator for the previous list
          */
         class iterator
         {
               friend class List<T> ;

            protected:
               // currently selected item
               item * _item;

            public:
               /**
                * constructor and destructor
                */
               iterator() :
                  _item(NULL)
               {
               }
               iterator(item * item) :
                  _item(item)
               {
               }
               iterator(const iterator & iter) :
                  _item(iter._item)
               {
               }

               // get status (end-of-list)
               bool eol() const
               {
                  return (_item == NULL);
               }

               // increment
               iterator & next()
               {
                  return ++*this;
               }

               // access actual element
               T & data()
               {
                  assert(_item != NULL);
                  return _item->_data;
               }
               const T & data() const
               {
                  assert(_item != NULL);
                  return _item->_data;
               }

               //
               // usual operators
               //

               // copy
               iterator & operator =(const iterator & i)
               {
                  _item = i._item;
                  return *this;
               }

               // access items
               T & operator ()()
               {
                  assert(_item != NULL);
                  return _item->_data;
               }
               const T & operator ()() const
               {
                  assert(_item != NULL);
                  return _item->_data;
               }

               // iterate (prefix/postfix)
               iterator & operator ++()
               {
                  if (_item)
                     _item = _item->_next;
                  return *this;
               }
               iterator operator ++(int)
               {
                  iterator tmp(*this);
                  ++*this;
                  return tmp;
               }

               // dereference
               T & operator *()
               {
                  assert(_item != NULL);
                  return _item->_data;
               }
               T * operator ->()
               {
                  assert(_item != NULL);
                  return &_item->_data;
               }

               // compare
               bool operator ==(const iterator & i) const
               {
                  return (_item == i._item);
               }
               bool operator !=(const iterator & i) const
               {
                  return (_item != i._item);
               }

            protected:
               // handle case when no item is present
               void no_item() const
               {
                  std::cout << "List::iterator::no_item : no item in iterator present" << std::endl;
               }
         };

      protected:
         // first and last element of list
         item * _first;
         item * _last;

         // size of the list aka number of stored items
         unsigned int _size;

      public:
         /////////////////////////////////////////////////
         //
         // constructors and destructors
         //

         List() :
            _first(NULL), _last(NULL), _size(0)
         {
         }
         List(const List<T> & l) :
            _first(NULL), _last(NULL), _size(0)
         {
            copy(l);
         }

         ~List()
         {
            if (_size > 0)
               clear();
         }

         /////////////////////////////////////////////////
         //
         // access member variables
         //

         unsigned int size() const
         {
            return _size;
         }

         iterator first()
         {
            return _first;
         }
         const iterator first() const
         {
            return _first;
         }

         iterator last()
         {
            return _last;
         }
         const iterator last() const
         {
            return _last;
         }

         iterator begin()
         {
            return _first;
         }
         const iterator begin() const
         {
            return _first;
         }

         iterator end()
         {
            return _last;
         }
         const iterator end() const
         {
            return _last;
         }

         // same as above but access real data
         T & first_item()
         {
            assert(_first != NULL);
            return _first->_data;
         }
         const T & first_item() const
         {
            assert(_first != NULL);
            return _first->_data;
         }

         T & last_item()
         {
            assert(_last != NULL);
            return _last->_data;
         }
         const T & last_item() const
         {
            assert(_last != NULL);
            return _last->_data;
         }

         /////////////////////////////////////////////////
         //
         // methods for manipulating list
         //

         //
         // add a new object to the front of list
         //
         List<T> & push_front(const T & elem)
         {
            item * tmp = new item(elem, _first);

            _first = tmp;
            _size++;

            if (_last == NULL)
               _last = tmp;

            return *this;
         }

         //
         // add a new object to the back of list
         //
         List<T> & push_back(const T & elem)
         {
            item * tmp = new item(elem);

            if (_last)
               _last->_next = tmp;

            _last = tmp;

            if (!_first)
               _first = tmp;

            _size++;

            return *this;
         }

         //
         // remove given element from list
         //
         void remove(const T & elem)
         {
            item * cur = _first;
            item * old = NULL;

            while (cur != NULL)
            {
               if (cur->_data == elem)
               {
                  if (cur == _first)
                     _first = cur->_next;
                  else
                     // (cur != first) => (old != NULL)
                     old->_next = cur->_next;

                  if (cur == _last)
                     _last = old;

                  delete cur;
                  _size--;

                  if (old == NULL)
                     cur = _first;
                  else
                     cur = old->_next;
               }// if
               else
               {
                  old = cur;
                  cur = cur->_next;
               }// else
            }// while
         }

         //
         // remove given element (by iterator) from list
         void remove(iterator & i)
         {
            assert(i._item != NULL);
            remove(i._item->_data);
         }

         //
         // remove first element of list
         //
         void pop_front()
         {
            if (_first)
            {
               item * tmp = _first->_next;

               delete _first;

               if (tmp == NULL)
                  _last = NULL;

               _first = tmp;

               _size--;
            }// if
         }

         //
         // return first item and remove it from list
         //
         T behead()
         {
            assert( _first != NULL );

            T ret = _first->_data;
            item * tmp = _first->_next;

            delete _first;

            if (tmp == NULL)
               _last = NULL;

            _first = tmp;

            _size--;

            return ret;
         }

         //
         // remove all elements from list
         //
         void clear()
         {
            if (_size == 0)
               return;

            item * tmp = _first;
            item * next;

            while (tmp != NULL)
            {
               next = tmp->_next;
               delete tmp;
               tmp = next;
            }// while

            _first = _last = NULL;
            _size = 0;
         }

         //
         // copy given list into local
         //
         void copy(const List<T> & list)
         {
            item * tmp = list._first;
            item * old = NULL;
            item * cur = list._first;

            clear();

            while (tmp != NULL)
            {
               cur = new item(tmp->_data);

               if (_first == NULL)
                  _first = cur;

               if (old != NULL)
                  old->_next = cur;

               old = cur;
               tmp = tmp->_next;
            }// while

            _last = old;
            _size = list._size;
         }

         /////////////////////////////////////////////////
         //
         // search/sort related methods
         //

         //
         // look if element is in list
         //
         bool is_in(const T & elem) const
         {
            item * tmp = _first;

            while (tmp)
            {
               if (tmp->_data == elem)
                  return true;

               tmp = tmp->_next;
            }// while

            return false;
         }

         bool has_more()
         {
            if(_size > 0)
               return true;
            else
               return false;
         }

         /////////////////////////////////////////////////
         //
         // misc.
         //

         //
         // assignment
         //
         List<T> & operator =(const List<T> & list)
         {
            copy(list);
            return *this;
         }

      protected:
         //
         // handle case when no item is present
         //
         void no_item() const
         {
            std::cout << "List::no_item : I miss an item to play with" << std::endl;
         }
   };

}; //namespace SYS

#endif   // LIST_H_
