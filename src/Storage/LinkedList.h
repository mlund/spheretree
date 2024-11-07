/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/

/*
    Template for doubly linked list, nodes must supply next and prev pointers
*/
#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "../Exceptions/CheckDebug.h"

template <class T> class LinkedList {
private:
  T *first;

public:
  //  construction
  LinkedList() { first = NULL; }

  void deleteNodes() {
    T *node = first;
    while (node) {
      T *next = node->next;
      delete node;
      node = next;
    }
    first = NULL;
  }

  //  add node
  void addNode(T *newNode) {
    CHECK_DEBUG(newNode->next == NULL && newNode->prev == NULL,
                "Node is already in a list");

    newNode->next = first;
    if (first)
      first->prev = newNode;
    first = newNode;
  }

  void insertNode(T *newNode, T *prev) {
    CHECK_DEBUG(newNode != NULL, "Null Node");
    CHECK_DEBUG(prev != NULL, "Null Previous Node");
    CHECK_DEBUG(newNode->next == NULL && newNode->prev == NULL,
                "Node is already in a list");

    newNode->prev = prev;
    newNode->next = prev->next;
    if (prev->next)
      prev->next->prev = newNode;
    prev->next = newNode;
  }

  //  remove node
  void removeNode(T *node) {
    if (node->prev == NULL) {
      //  removing from head
      CHECK_DEBUG(first == node, "Node not at head of list");
      first = node->next;
      if (first)
        first->prev = NULL;
      node->next = NULL;
    } else {
      //  remove from middle or end
      node->prev->next = node->next;
      if (node->next)
        node->next->prev = node->prev;
      node->prev = NULL;
      node->next = NULL;
    }
  }

  //  iteration
  __inline T *getFirst() const { return first; }

  __inline T *getNext(const T *node) const {
    CHECK_DEBUG(node != NULL, "NULL node");
    return node->next;
  }

  __inline T *getPrev(const T *node) const {
    CHECK_DEBUG(node != NULL, "NULL node");
    return node->prev;
  }

  //  init node
  __inline void initNode(T *node) {
    CHECK_DEBUG(node != NULL, "NULL node");
    node->prev = NULL;
    node->next = NULL;
  }
};

#endif