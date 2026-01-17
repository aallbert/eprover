/*-----------------------------------------------------------------------

File  : clb_numxtrees.c

Author: Stephan Schulz

Contents

  Functions for long-indexed splay trees with fixed-sized array of
  values.

Copyright 1998-2011 by the author.
  This code is released under the GNU General Public Licence and
  the GNU Lesser General Public License.
  See the file COPYING in the main E directory for details..
  Run "eprover -h" for contact information.

Changes (vastly incomplete, see CVS log)

<1> Mon Aug  1 11:03:32 CEST 2011
    New from clb_numtree.h

-----------------------------------------------------------------------*/

#include "clb_numxtrees2.h"



/*---------------------------------------------------------------------*/
/*                        Global Variables                             */
/*---------------------------------------------------------------------*/


/*---------------------------------------------------------------------*/
/*                      Forward Declarations                           */
/*---------------------------------------------------------------------*/


/*---------------------------------------------------------------------*/
/*                         Internal Functions                          */
/*---------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
//
// Function: splay_tree()
//
//   Perform the splay operation on tree at node with key.
//
// Global Variables: -
//
// Side Effects    : Changes tree
//
/----------------------------------------------------------------------*/

static NumXTree_p splay_tree(NumXTree_p tree, long key)
{
   NumXTree_p   left, right, tmp;
   NumXTreeCell newnode;

   if (!tree)
   {
      return tree;
   }

   newnode.lson = NULL;
   newnode.rson = NULL;
   left = &newnode;
   right = &newnode;

   for (;;)
   {
      long cmpres = key-tree->key;
      if (cmpres < 0)
      {
         if(!tree->lson)
         {
            break;
         }
         if((key- tree->lson->key) < 0)
         {
            tmp = tree->lson;
            tree->lson = tmp->rson;
            tmp->rson = tree;
            tree = tmp;
            if (!tree->lson)
            {
               break;
            }
         }
         right->lson = tree;
         right = tree;
         tree = tree->lson;
      }
      else if(cmpres > 0)
      {
         if (!tree->rson)
         {
            break;
         }
         if((key-tree->rson->key) > 0)
         {
            tmp = tree->rson;
            tree->rson = tmp->lson;
            tmp->lson = tree;
            tree = tmp;
            if (!tree->rson)
            {
               break;
            }
         }
         left->rson = tree;
         left = tree;
         tree = tree->rson;
      }
      else
      {
         break;
      }
   }
   left->rson = tree->lson;
   right->lson = tree->rson;
   tree->lson = newnode.rson;
   tree->rson = newnode.lson;

   return tree;
}


/*-----------------------------------------------------------------------
//
// Function: node_empty()
//
//   Return true if the elements of node->vals of node are NULL.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

static NumXTree_p node_empty(NumXTree_p node)
{
   for(int i = 0; i < NUMXTREEVALUES; i++) 
   {
      if (node->vals[i].p_val) 
      {
         return false;
      }
   }
   return true;
}



/*---------------------------------------------------------------------*/
/*                         Exported Functions                          */
/*---------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
//
// Function: NumXTreeCellAllocEmpty()
//
//   Allocate a empty, initialized NumXTreeCell. Pointers to children
//   are NULL, int values are 0 (and pointer values in ANSI-World
//   undefined, in practice NULL on 32 bit machines)(This comment is
//   superfluous!). The balance field is (correctly) set to 0.
//
// Global Variables: -
//
// Side Effects    : Memory operations
//
/----------------------------------------------------------------------*/

NumXTree_p NumXTreeCellAllocEmpty(void)
{
   NumXTree_p handle = NumXTreeCellAlloc();
   int i;

   for(i=0; i<NUMXTREEVALUES; i++)
   {
      handle->vals[i].i_val = 0;
   }
   handle->lson = handle->rson       = NULL;

   return handle;
}

/*-----------------------------------------------------------------------
//
// Function: NumXTreeFree()
//
//   Free a numtree (including the keys, but not potential objects
//   pointed to in the val fields
//
// Global Variables: -
//
// Side Effects    : Memory operations
//
/----------------------------------------------------------------------*/

void NumXTreeFree(NumXTree_p junk)
{
   if(junk)
   {
      PStack_p stack = PStackAlloc();

      PStackPushP(stack, junk);

      while(!PStackEmpty(stack))
      {
         junk = PStackPopP(stack);
         if(junk->lson)
         {
            PStackPushP(stack, junk->lson);
         }
         if(junk->rson)
         {
            PStackPushP(stack, junk->rson);
         }
         NumXTreeCellFree(junk);
      }
      PStackFree(stack);
   }
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeInsertNode()
//
//   If an entry with key *newnode->key exists in the tree return a
//   pointer to it. Otherwise insert *newnode in the tree and return
//   NULL.
//
// Global Variables: -
//
// Side Effects    : Changes the tree
//
/----------------------------------------------------------------------*/

NumXTree_p NumXTreeInsertNode(NumXTree_p *root, NumXTree_p newnode)
{
   if (!*root)
   {
      newnode->lson = newnode->rson = NULL;
      *root = newnode;
      return NULL;
   }
   *root = splay_tree(*root, newnode->key);

   long cmpres = newnode->key-(*root)->key;

   if (cmpres < 0)
   {
      newnode->lson = (*root)->lson;
      newnode->rson = *root;
      (*root)->lson = NULL;
      *root = newnode;
      return NULL;
   }
   else if(cmpres > 0)
   {
      newnode->rson = (*root)->rson;
      newnode->lson = *root;
      (*root)->rson = NULL;
      *root = newnode;
      return NULL;
   }
   return *root;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeInsertVal()
//    
//   If a val at key exists in the tree return a pointer to the node of
//   the array conatining it. Otherwise insert val in the tree and return
//   NULL.
//
// Global Variables: -
//
// Side Effects    : Changes the tree
//
/----------------------------------------------------------------------*/

NumXTree_p NumXTreeInsertVal(NumXTree_p *root, long key, IntOrP val)
{
   long index = key % NUMXTREEVALUES;
   long nodekey = key - index;
   NumXTree_p newnode = NumXTreeFind(root, nodekey);
   if (newnode)
   {
      if (!newnode->vals[index].p_val)
      {
         newnode->vals[index] = val;
         return NULL;
      }
      return newnode;
   }
   newnode->key = nodekey;
   newnode->vals[index] = val;

   long cmpres = nodekey - (*root)->key;

   if (cmpres < 0)
   {
      newnode->lson = (*root)->lson;
      newnode->rson = *root;
      (*root)->lson = NULL;
      *root = newnode;
      return NULL;
   }
   else if(cmpres > 0)
   {
      newnode->rson = (*root)->rson;
      newnode->lson = *root;
      (*root)->rson = NULL;
      *root = newnode;
      return NULL;
   }
   return *root;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeStoreNode()
//
//   Insert a cell associating key with val into the
//   tree. Return false if an entry for this key exists, true
//   otherwise.
//
// Global Variables: -
//
// Side Effects    : Changes tree
//
/----------------------------------------------------------------------*/

bool NumXTreeStoreNode(NumXTree_p *root, long key, IntOrP val)
{
   NumXTree_p handle, newnode;

   handle = NumXTreeCellAlloc();
   handle->key = key - (key % NUMXTREEVALUES);
   handle->vals[key % NUMXTREEVALUES] = val;

   newnode = NumXTreeInsertNode(root, handle);

   if(newnode)
   {
      NumXTreeCellFree(handle);
      return false;
   }
   return true;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeStoreVal()
//
//   Insert a cell associating key with val1 and val2 into the
//   tree. Return false if an entry for this key exists, true
//   otherwise.
//
// Global Variables: -
//
// Side Effects    : Changes tree
//
/----------------------------------------------------------------------*/

bool NumXTreeStoreVal(NumXTree_p *root, long key, IntOrP val)
{
   NumXTree_p handle, newnode;

   handle = NumXTreeCellAlloc();
   handle->key = key - (key % NUMXTREEVALUES);
   handle->vals[key % NUMXTREEVALUES] = val;

   // If node exists, newnode will be NULL and NumXTreeInsterVal will be called
   newnode = NumXTreeInsertNode(root, handle);

   if (!newnode) 
   {
      NumXTreeInsertVal(root, key, val);
      return true;
   }
   else 
   {
      NumXTreeCellFree(handle);
      return false;
   }
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeFind()
//
//   Find the node containing the val with key key in the tree and return
//   it. Return NULL if no such key exists.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

NumXTree_p NumXTreeFind(NumXTree_p *root, long key)
{
   if(*root)
   {
      *root = splay_tree(*root, key - (key % NUMXTREEVALUES));
      if(((*root)->key - (key - (key % NUMXTREEVALUES)))==0)
      {
         return *root;
      }
   }
   return NULL;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeExtractValue()
//
//   Find the entry with key key, remove it from the tree and return the
//   pointer to the node containing it. If the node containing it is
//   empty, remove the node from the tree and rebalance the tree.
//   Return NULL if no matching element exists.
//
// Global Variables: -
//
// Side Effects    : Changes the tree
//
/----------------------------------------------------------------------*/

NumXTree_p NumXTreeExtractValue(NumXTree_p *root, long key)
{
   NumXTree_p x, cell;

   if (!(*root))
   {
      return NULL;
   }

   long index = key % NUMXTREEVALUES;
   long nodekey = key - index;

   *root = splay_tree(*root, nodekey);

   if((nodekey-(*root)->key)==0)
   {
      cell = *root;
      (*root)->vals[index].p_val = NULL;
      // if removing of value at key would make root empty
      if (node_empty(root))
      {         
         if (!(*root)->lson)
         {
            x = (*root)->rson;
         }
         else
         {
            x = splay_tree((*root)->lson, nodekey);
            x->rson = (*root)->rson;
         }
         // necessary bc of splay (?)
         cell = *root;
         cell->lson = cell->rson = NULL;
         *root = x;
         return cell;
      }
      // marker to know, that cell is not empty after removal
      cell->lson = cell->rson = cell;
      return cell;
   }
   // node with nodekey does not exist
   return NULL;
}


// /*-----------------------------------------------------------------------
// //
// // Function: NumXTreeExtractRoot()
// //
// //   Extract the NumXTreeCell at the root of the tree and return it (or
// //   NULL if the tree is empty).
// //
// // Global Variables:
// //
// // Side Effects    :
// //
// /----------------------------------------------------------------------*/
// TODO: probably delete since it's not needed
// NumXTree_p NumXTreeExtractRoot(NumXTree_p *root)
// {
//    if(*root)
//    {
//       return NumXTreeExtractValue(root, (*root)->key);
//    }
//    return NULL;
// }


/*-----------------------------------------------------------------------
//
// Function: NumXTreeDeleteEntry()
//
//   Delete the entry with key key from the tree.
//
// Global Variables: -
//
// Side Effects    : By NumXTreeExtract(), memory operations
//
/----------------------------------------------------------------------*/

bool NumXTreeDeleteEntry(NumXTree_p *root, long key)
{
   NumXTree_p cell;

   cell = NumXTreeExtractValue(root, key);
   if(cell)
   {
      NumXTreeFree(cell);
      return true;
   }
   return false;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeNodes()
//
//   Return the number of nodes in the tree.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

long NumXTreeNodes(NumXTree_p root)
{
   PStack_p stack = PStackAlloc();
   long     res   = 0;

   PStackPushP(stack, root);

   while(!PStackEmpty(stack))
   {
      root = PStackPopP(stack);
      if(root)
      {
    PStackPushP(stack, root->lson);
    PStackPushP(stack, root->rson);
    res++;
      }
   }
   PStackFree(stack);

   return res;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeMaxNode()
//
//   Return the node with the largest key in the tree (or NULL if tree
//   is empty). Non-destructive/non-reorganizing.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

NumXTree_p NumXTreeMaxNode(NumXTree_p root)
{
   if(root)
   {
      while(root->rson)
      {
         root = root->rson;
      }
   }
   return root;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeLimitedTraverseInit()
//
//   Return a stack containing the path to the smallest element
//   smaller than or equal to limit in the tree.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

PStack_p NumXTreeLimitedTraverseInit(NumXTree_p root, long limit)
{
   PStack_p stack = PStackAlloc();

   while(root)
   {
      if(root->key<limit)
      {
         root = root->rson;
      }
      else
      {
         PStackPushP(stack, root);
         if(root->key == limit)
         {
            root = NULL;
         }
         else
         {
            root = root->lson;
         }
      }
   }
   return stack;
}


// AVL_TRAVERSE_DEFINITION(NumXTree, NumXTree_p)

/*----------------------------------------------------------------------
//
// Function: NumXTreeTraverseNext()
//
//   Given a stack describing a traversal state, return the next node
//   and update the stack.
//
// Global Variables: -
//
// Side Effects    : Updates stack
//
/---------------------------------------------------------------------*/

NumXTree_p NumXTreeTraverseNext(PStack_p state)
{
   NumXTree_p handle, res;

   if(PStackEmpty(state))
   {
      return NULL;
   }
   res = PStackPopP(state);
   handle = res->rson;
   while(handle)
   {
      PStackPushP(state, handle);
      handle = handle->lson;
   }
   return res;
}


/*---------------------------------------------------------------------*/
/*                        End of File                                  */
/*---------------------------------------------------------------------*/


