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
    Template class for One Dimensional Array, the Array is managed as a set of
   blocks which allows efficient resizing.  Block size must be 2^n elements,
   which allows "efficient" indexing. Two sizes are maintained, AllocatedSize
   and Size.  After initial allocation both size is set to the required size.
*/
#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <math.h>
#include <stdlib.h>
// #include <malloc.h>
#include <memory.h>
#include "../Base/Types.h"
#include "../Exceptions/CheckDebug.h"
#include "../Exceptions/CheckMemory.h"

template <class T> class Array {
protected:
  struct Block {
    bool alloc; //  was the block separately allocated
    T *ptr;
  } *blocks;
  int numBlocks, szBlock; //  number and size of blocks (as power of two)
  int size;               //  number of used elements
  int blockMask;          //  mask to get offset in block

public:
  //  construction
  Array() {
    size = 0;
    blocks = NULL;
    numBlocks = 0;
    szBlock = 8; //  256 elements per block
    blockMask = getMask(szBlock);
  }

  Array(int initSize, int blockSize = 8) {
    size = 0;
    blocks = NULL;
    numBlocks = 0;
    allocate(initSize, blockSize);
  }

  //  destruction
  ~Array() {
    if (numBlocks != 0)
      free();
  }

  //  allocation
  void allocate(int initSize, int initBlockSize = 8) {
    CHECK_DEBUG2(initSize > 0 && initBlockSize > 0,
                 "InitSize %d, InitBlockSize %d", initSize, initBlockSize);
    CHECK_DEBUG(numBlocks == 0, "Use GROW/REALLOCATE instead");

    int elementsPerBlock = getEPB(initBlockSize);
    int initBlocks = (int)ceil((float)initSize / elementsPerBlock);

    blocks = (Block *)malloc(initBlocks * sizeof(Block));
    CHECK_MEMORY1(blocks != NULL, "Tryed to Allocate %d bytes",
                  (initBlocks * sizeof(Block)));

    blocks[0].alloc = true;
    blocks[0].ptr = new T[elementsPerBlock * initBlocks];
    CHECK_MEMORY1(blocks[0].ptr != NULL, "Tryed to Allocate %d bytes",
                  elementsPerBlock * initBlocks);
    for (int i = 1; i < initBlocks; i++) {
      blocks[i].alloc = false;
      blocks[i].ptr = &blocks[0].ptr[i * elementsPerBlock];
    }

    size = initSize;
    szBlock = initBlockSize;
    numBlocks = initBlocks;
    blockMask = getMask(szBlock);
  }

  //  deallocation
  void free() {
    CHECK_DEBUG(numBlocks != 0, "Not Allocated");
    for (int i = 0; i < numBlocks; i++)
      if (blocks[i].alloc)
        delete[] blocks[i].ptr;

    ::free((void *)blocks);
    numBlocks = 0;
    blocks = NULL;
    size = 0;
  }

  //  reallocate - doesn't copy the contents
  void reallocate(int newSize, int newBlockSize = -1) {
    if (numBlocks != 0)
      free();

    if (newBlockSize < 0)
      newBlockSize = szBlock;

    allocate(newSize, newBlockSize);
  }

  //  leave existing array blocks TOTALLY UNCHANGED
  void resize(int newSize) {
    if (newSize >= getAllocSize()) {
      int elementsPerBlock = getEPB(szBlock);
      int newBlocks = (int)ceil((float)newSize / elementsPerBlock);

      if (newBlocks > numBlocks) {
        //  allocate blocks
        blocks = (Block *)realloc(blocks, newBlocks * sizeof(Block));
        CHECK_MEMORY1(blocks != NULL, "Tryed to Allocate %d bytes",
                      newBlocks * sizeof(Block));

        blocks[numBlocks].alloc = true;
        blocks[numBlocks].ptr =
            new T[elementsPerBlock * (newBlocks - numBlocks)];
        CHECK_MEMORY1(blocks[numBlocks].ptr != NULL,
                      "Tryed to Allocate %d bytes",
                      elementsPerBlock * (newBlocks - numBlocks));
        for (int i = numBlocks + 1; i < newBlocks; i++) {
          blocks[i].alloc = false;
          blocks[i].ptr =
              &blocks[numBlocks].ptr[(i - numBlocks) * elementsPerBlock];
        }

        numBlocks = newBlocks;
      }
    }
    setSize(newSize);
  }

  //  size
  __inline void setSize(int newSize) {
    CHECK_DEBUG2(newSize >= 0 && newSize <= getAllocSize(),
                 "Invalid Size : %d, AllocSize : %d", newSize, getAllocSize());
    size = newSize;
  }

  __inline int getSize() const { return size; }

  //  indexing
  __inline const T &index(int i) const {
    CHECK_DEBUG2((unsigned int)i < (unsigned int)size,
                 "Index Array Out of Bounds, Size : %d, Index : %d", size, i);
    CHECK_DEBUG(blocks != NULL, "BLOCKS isn't Valid");
    return blocks[i >> szBlock].ptr[i & blockMask];
  }

  __inline T &index(int i) {
    CHECK_DEBUG2((unsigned int)i < (unsigned int)size,
                 "Index Array Out of Bounds, Size : %d, Index : %d", size, i);
    CHECK_DEBUG(blocks != NULL, "BLOCKS isn't Valid");
    return blocks[i >> szBlock].ptr[i & blockMask];
  }

  //  copy
  void copy(const Array<T> &src) {
    CHECK_DEBUG2(getAllocSize() >= src.size,
                 "Array not big enough Src : %d, Dest : %d", src.size,
                 getAllocSize());

    //  needs to be more effic
    size = src.size;
    for (int i = 0; i < src.size; i++)
      index(i) = src.index(i);
  }

  void clone(const Array<T> &src) {
    resize(src.getSize());
    copy(src);
  }

  //  append
  void append(const Array<T> &src) {
    int n = getSize();
    int m = src.getSize();

    resize(n + m);
    for (int i = 0; i < m; i++)
      index(n + i) = src.index(i);
  }

  //  zero contents
  void clear() {
    int BpB = sizeof(T) * getEPB(szBlock);
    for (int i = 0; i < numBlocks; i++)
      memset(blocks[i].ptr, 0, BpB);
  }

  // put in one new item (return index)
  __inline int addIndex() {
    int s = getSize();
    resize(s + 1);
    return s;
  }

  // put in one new item (return item)
  __inline T &addItem() {
    int s = addIndex();
    return index(s);
  }

  //  remove item and shift down
  void removeItem(int i) {
    CHECK_DEBUG2((unsigned int)i < (unsigned int)size,
                 "Index Array Out of Bounds, Size : %d, Index : %d", size, i);
    CHECK_DEBUG(blocks != NULL, "BLOCKS isn't Valid");

    int s = getSize();
    for (int j = i; j < size - 1; j++)
      index(j) = index(j + 1);
    resize(s - 1);
  }

  //  max size before regrow
  __inline int getAllocSize() const { return numBlocks * getEPB(szBlock); }

  //  linear search for primitive types
  bool inList(int v, int sI = 0, int eI = -1) const {
    if (eI < 0)
      eI = getSize();
    for (int i = sI; i < eI; i++)
      if (index(i) == v)
        return true;

    return false;
  }

private:
  //  helpers
  __inline static int getMask(int bs) { return ~((unsigned int)-1 << bs); }

  __inline static int getEPB(int bs) {
    return 0x01 << bs; // 2^initBlockSize
  }
};

#endif
