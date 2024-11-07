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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "LinkedList.h"

template <class T> class Queue {
public: //  TEMP
        // private:
  struct Node {
    T item;
    Node *next, *prev;
  };
  LinkedList<Node> list;
  Node *lastNode;

public:
  void addItem(const T &t) {
    Node *n = new Node;
    list.initNode(n);
    n->item = t;

    if (!list.getFirst())
      list.addNode(n);
    else
      list.insertNode(n, lastNode);
    lastNode = n;
  }

  T getItem() {
    Node *n = list.getFirst();
    CHECK_DEBUG(n != NULL, "Empty Queue");

    T tmp = n->item;
    list.removeNode(n);
    delete n;

    return tmp;
  }

  void getItem(T *t) {
    Node *n = list.getFirst();
    CHECK_DEBUG(n != NULL, "Empty Queue");

    (*t) = n->item;
    list.removeNode(n);
    delete n;
  }

  __inline bool hasItem() const { return list.getFirst() != NULL; }
};

#endif