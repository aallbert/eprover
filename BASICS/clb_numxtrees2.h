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

#ifndef CLB_NUMXTREES2

#define CLB_NUMXTREES2

#include <clb_dstrings.h>
#include <clb_avlgeneric.h>
#include <clb_pstacks.h>

/*---------------------------------------------------------------------*/
/*                    Data type declarations                           */
/*---------------------------------------------------------------------*/

#ifndef NUMXTREEVALUES
#define NUMXTREEVALUES 4
#endif

/* General purpose data structure for indexing objects by a numerical
   key. Integer values are supported directly, for all other objects
   pointers can be used (and need to be casted carefully by the
   wrapper functions). Objects pointed to by the value fields are not
   part of the data stucture and will not be touched by deallocating
   trees or tree nodes. */

typedef struct numxtree2cell
{
   long                key;
   IntOrP              vals[NUMXTREEVALUES];
   struct numxtree2cell *lson;
   struct numxtree2cell *rson;
}NumXTree2Cell, *NumXTree2_p;


/*---------------------------------------------------------------------*/
/*                Exported Functions and Variables                     */
/*---------------------------------------------------------------------*/

#define NumXTree2CellAlloc() (NumXTree2Cell*)SizeMalloc(sizeof(NumXTree2Cell))
#define NumXTree2CellFree(junk)        SizeFree(junk, sizeof(NumXTree2Cell))

#ifdef CONSTANT_MEM_ESTIMATE
#define NUMTREECELL_MEM 24
#else
#define NUMXTREECELL_MEM MEMSIZE(NumXTree2Cell)
#endif

// TODO: remove the '2' eventually

bool      NumXTreeNodeSingleElement(NumXTree2_p node, int index);
NumXTree2_p NumXTree2CellAllocEmpty(void);
void      NumXTree2Free(NumXTree2_p junk);
NumXTree2_p NumXTree2InsertNode(NumXTree2_p *root, NumXTree2_p newnode);
NumXTree2_p NumXTreeInsertKeyValPair(NumXTree2_p *root, long key, void* val);
bool       NumXTree2StoreNode(NumXTree2_p *root, long key, IntOrP val);
NumXTree2_p NumXTree2Find(NumXTree2_p *root, long key);
NumXTree2_p NumXTree2ExtractValue(NumXTree2_p *root, long key);
NumXTree2_p NumXTree2ExtractRoot(NumXTree2_p *root);
bool      NumXTree2DeleteEntry(NumXTree2_p *root, long key);
long      NumXTree2Nodes(NumXTree2_p root);
NumXTree2_p NumXTree2MaxNode(NumXTree2_p root);
long      NumXTree2MaxKey(NumXTree2_p node);

PStack_p NumXTree2LimitedTraverseInit(NumXTree2_p root, long limit);

NumXTree2_p NumXTree2TraverseNext(PStack_p state, long* last_seen_key);
// AVL_TRAVERSE_DECLARATION(NumXTree, NumXTree2_p)
#define NumXTree2TraverseExit(stack) PStackFree(stack)


#endif

/*---------------------------------------------------------------------*/
/*                        End of File                                  */
/*---------------------------------------------------------------------*/





