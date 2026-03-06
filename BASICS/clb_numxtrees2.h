/*-----------------------------------------------------------------------

File  : clb_numxtrees2.h

Author: Stephan Schulz

Contents

  Definitions for SPLAY trees with long integer keys and vectors of
  IntOrPs as values. Copied from clb_numtrees.h

  Copyright 1998, 1999, 2011 by the author.
  This code is released under the GNU General Public Licence and
  the GNU Lesser General Public License.
  See the file COPYING in the main E directory for details..
  Run "eprover -h" for contact information.

Changes

<1> Mon Aug  1 11:04:53 CEST 2011
    New from clb_numtrees.h

-----------------------------------------------------------------------*/

// #define NumXTreeTraverseNext          NumXTree2TraverseNext
// #define NumXTreeFind                  NumXTree2Find
// #define NumXTreeCellAllocEmpty        NumXTree2CellAllocEmpty
// #define NumXTreeFree                  NumXTree2Free
// #define NumXTreeInsertNode            NumXTree2InsertNode
// #define NumXTreeStoreNode             NumXTree2StoreNode
// #define NumXTreeFind                  NumXTree2Find
// #define NumXTreeExtractValue          NumXTree2ExtractValue
// #define NumXTreeExtractRoot           NumXTree2ExtractRoot
// #define NumXTreeDeleteEntry           NumXTree2DeleteEntry
// #define NumXTreeNodes                 NumXTree2Nodes
// #define NumXTreeMaxNode               NumXTree2MaxNode
// #define NumXTreeMaxKey                NumXTree2MaxKey
// #define NumXTreeLimitedTraverseInit   NumXTree2LimitedTraverseInit
// #define NumXTreeTraverseExit          NumXTree2TraverseExit

#ifndef CLB_NUMXTREES

#define CLB_NUMXTREES

#include <clb_dstrings.h>
#include <clb_avlgeneric.h>
#include <clb_pstacks.h>

/*---------------------------------------------------------------------*/
/*                    Data type declarations                           */
/*---------------------------------------------------------------------*/


#define NUMXTREEVALUES 4

/* General purpose data structure for indexing objects by a numerical
   key. Integer values are supported directly, for all other objects
   pointers can be used (and need to be casted carefully by the
   wrapper functions). Objects pointed to by the value fields are not
   part of the data stucture and will not be touched by deallocating
   trees or tree nodes. */

typedef struct numxtreecell
{
   long                key;
   IntOrP              vals[NUMXTREEVALUES];
   struct numxtreecell *lson;
   struct numxtreecell *rson;
}NumXTreeCell, *NumXTree_p;


/*---------------------------------------------------------------------*/
/*                Exported Functions and Variables                     */
/*---------------------------------------------------------------------*/

#define NumXTreeCellAlloc() (NumXTreeCell*)SizeMalloc(sizeof(NumXTreeCell))
#define NumXTreeCellFree(junk)        SizeFree(junk, sizeof(NumXTreeCell))

#ifdef CONSTANT_MEM_ESTIMATE
#define NUMTREECELL_MEM 24
#else
#define NUMXTREECELL_MEM MEMSIZE(NumXTreeCell)
#endif

// TODO: remove the '2' eventually

bool      NumXTreeNodeSingleElement(NumXTree_p node, int index);
NumXTree_p NumXTree2CellAllocEmpty(void);
void      NumXTree2Free(NumXTree_p junk);
NumXTree_p NumXTree2InsertNode(NumXTree_p *root, NumXTree_p newnode);
NumXTree_p NumXTreeInsertKeyValPair(NumXTree_p *root, long key, void* val);
bool       NumXTree2StoreNode(NumXTree_p *root, long key, IntOrP val);
NumXTree_p NumXTree2Find(NumXTree_p *root, long key);
NumXTree_p NumXTree2ExtractValue(NumXTree_p *root, long key);
NumXTree_p NumXTree2ExtractRoot(NumXTree_p *root);
bool      NumXTree2DeleteEntry(NumXTree_p *root, long key);
long      NumXTree2Nodes(NumXTree_p root);
NumXTree_p NumXTree2MaxNode(NumXTree_p root);
long      NumXTree2MaxKey(NumXTree_p node);

PStack_p NumXTree2LimitedTraverseInit(NumXTree_p root, long limit);

NumXTree_p NumXTree2TraverseNext(PStack_p state);
// AVL_TRAVERSE_DECLARATION(NumXTree, NumXTree_p)
#define NumXTree2TraverseExit(stack) PStackFree(stack)


#endif

/*---------------------------------------------------------------------*/
/*                        End of File                                  */
/*---------------------------------------------------------------------*/





