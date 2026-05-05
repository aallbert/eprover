/*-----------------------------------------------------------------------

File  : clb_numxtrees2.c

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

static NumXTree2_p splay_tree(NumXTree2_p tree, long key)
{
   NumXTree2_p   left, right, tmp;
   NumXTree2Cell newnode;

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


/*---------------------------------------------------------------------*/
/*                         Exported Functions                          */
/*---------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
//
// Function: NumXTree2CellAllocEmpty()
//
//   Allocate a empty, initialized NumXTree2Cell. Pointers to children
//   are NULL, int values are 0 (and pointer values in ANSI-World
//   undefined, in practice NULL on 32 bit machines)(This comment is
//   superfluous!). The balance field is (correctly) set to 0.
//
// Global Variables: -
//
// Side Effects    : Memory operations
//
/----------------------------------------------------------------------*/

NumXTree2_p NumXTree2CellAllocEmpty(void)
{
   NumXTree2_p handle = NumXTree2CellAlloc();
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
// Function: NumXTree2Free()
//
//   Free a numtree (including the keys, but not potential objects
//   pointed to in the val fields
//
// Global Variables: -
//
// Side Effects    : Memory operations
//
/----------------------------------------------------------------------*/

void NumXTree2Free(NumXTree2_p junk)
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
         NumXTree2CellFree(junk);
      }
      PStackFree(stack);
   }
}


/*-----------------------------------------------------------------------
//
// Function: NumXTreeNodeEmpty()
//
//   Return true if the elements of node->vals of node are NULL.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

bool NumXTreeNodeEmpty(NumXTree2_p node)
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


/*-----------------------------------------------------------------------
//
// Function: NumXTreeNodeSingleElement()
//
//   Return true if the elements of node->vals of node would be NULL
//   After removing the element at index
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

bool NumXTreeNodeSingleElement(NumXTree2_p node, int index)
{
   for(int i=0; i < NUMXTREEVALUES; i++)
   {
      if(i == index) continue;
      if(node->vals[i].p_val != NULL)
      {
         return false; 
      }
   }
   return true;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTree2InsertNode()
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

NumXTree2_p NumXTree2InsertNode(NumXTree2_p *root, NumXTree2_p newnode)
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
// Function: NumXTreeInsertKeyValPair()
//    
//   If a val at key exists in the tree return NULL. Otherwise insert val
//   in the tree and return a pointer to the node of the array containing
//   it.
//
// Global Variables: -
//
// Side Effects    : Changes the tree
//
/----------------------------------------------------------------------*/

NumXTree2_p NumXTreeInsertKeyValPair(NumXTree2_p *root, long key, void* val)
{
   NumXTree2_p handle, newnode;
   long index, nodekey;

   index = key & (NUMXTREEVALUES - 1);
   nodekey = key - index;

   handle = NumXTree2Find(root, nodekey);
   if (handle)
   {
      if (!handle->vals[index].p_val)
      {
         handle->vals[index].p_val = val;
         return handle;
      }
      return NULL;
   }

   newnode = NumXTree2CellAllocEmpty();
   newnode->key = nodekey;
   newnode->vals[index].p_val = val;

   if (!*root)
   {
      newnode->lson = newnode->rson = NULL;
      *root = newnode;
      return *root;
   }

   *root = splay_tree(*root, nodekey);

   long cmpres = nodekey - (*root)->key;

   if (cmpres < 0)
   {
      newnode->lson = (*root)->lson;
      newnode->rson = *root;
      (*root)->lson = NULL;
      *root = newnode;
      return newnode;
   }
   else if(cmpres > 0)
   {
      newnode->rson = (*root)->rson;
      newnode->lson = *root;
      (*root)->rson = NULL;
      *root = newnode;
      return newnode;
   }
   return *root;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTree2StoreNode()
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

bool NumXTree2StoreNode(NumXTree2_p *root, long key, IntOrP val)
{
   NumXTree2_p handle, newnode;

   handle = NumXTree2CellAllocEmpty();
   handle->key = key & ~(NUMXTREEVALUES - 1);
   handle->vals[key & (NUMXTREEVALUES - 1)] = val;

   newnode = NumXTree2InsertNode(root, handle);

   if(newnode)
   {
      NumXTree2CellFree(handle);
      return false;
   }
   return true;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTree2Find()
//
//   Find the node containing the val with key key in the tree and return
//   it. Return NULL if no such key exists.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

NumXTree2_p NumXTree2Find(NumXTree2_p *root, long key)
{
   if (*root)
   {
      /* Modulo workaround with bitwise AND so negative numbers are
       * treated correctly */
      long nodekey = key & ~(NUMXTREEVALUES - 1);

      *root = splay_tree(*root, nodekey);

      if ((*root)->key - nodekey == 0)
      {
         return *root;
      }
   }
   return NULL;
}

/*-----------------------------------------------------------------------
//
// Function: NumXTree2ExtractValue()
//
//   Find the entry with key, rebalance the tree and return the pointer
//   to the node that shall be removed. 
//   ! This function alone does not remove the node from the tree !
//   If the node containing it is
//   empty, remove the node from the tree and rebalance the tree.
//   Return NULL if no matching element exists.
//
// Global Variables: -
//
// Side Effects    : Changes the tree
//
/----------------------------------------------------------------------*/

NumXTree2_p NumXTree2ExtractValue(NumXTree2_p *root, long key)
{
   NumXTree2_p x, cell;

   if (!(*root))
   {
      return NULL;
   }

   long index = key & (NUMXTREEVALUES - 1);
   long nodekey = key - index;

   *root = splay_tree(*root, nodekey);

   if (!(*root)->vals[index].p_val)
   {
      return NULL;
   }

   if((nodekey-(*root)->key)==0)
   {
      if (NumXTreeNodeSingleElement(*root, index))
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
         cell = *root;
         cell->lson = cell->rson = NULL;
         *root = x;
         return cell;
      }
      /*Important notice: in this step, the value is not
      extracted yet, but only when called in IntMapDelKey().
      This is so that the Caller of IntMapDelKey knows what
      value got deleted and NumXTree2ExtractValue() does not
      have to clone the node*/
      return *root;
   }
   return NULL;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTree2DeleteEntry()
//
//   Delete the entry with key key from the tree.
//
// Global Variables: -
//
// Side Effects    : By NumXTreeExtract(), memory operations
//
/----------------------------------------------------------------------*/

bool NumXTree2DeleteEntry(NumXTree2_p *root, long key)
{
   NumXTree2_p cell;

   cell = NumXTree2ExtractValue(root, key);
   if(cell)
   {
      NumXTree2Free(cell);
      return true;
   }
   return false;
}


/*-----------------------------------------------------------------------
//
// Function: NumXTree2Nodes()
//
//   Return the number of nodes in the tree.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

long NumXTree2Nodes(NumXTree2_p root)
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
// Function: NumXTree2MaxNode()
//
//   Return the node with the largest key in the tree (or NULL if tree
//   is empty). Non-destructive/non-reorganizing.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

NumXTree2_p NumXTree2MaxNode(NumXTree2_p root)
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
// Function: NumXTree2MaxKey()
//
//   Return the node with the largest key in the node (or NULL if tree
//   is empty). Non-destructive/non-reorganizing.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

long NumXTree2MaxKey(NumXTree2_p node)
{
   long index;
   for(int i = 0; i < NUMXTREEVALUES; i++) 
   {
      if (node->vals[i].p_val) 
      {
         index = i;
      }
   }
   return node->key + index;   
}


/*-----------------------------------------------------------------------
//
// Function: NumXTree2LimitedTraverseInit()
//
//   Return a stack containing the path to the smallest element
//   smaller than or equal to limit in the tree.
//
// Global Variables: -
//
// Side Effects    : -
//
/----------------------------------------------------------------------*/

PStack_p NumXTree2LimitedTraverseInit(NumXTree2_p root, long limit)
{
   PStack_p stack = PStackAlloc();

   while(root)
   {
      /* limit is not super strict, key/val pairs exceeding the limit 
       * are handled in the IntMap */
      if(root->key < (limit & ~(NUMXTREEVALUES - 1)))
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


// AVL_TRAVERSE_DEFINITION(NumXTree, NumXTree2_p)

/*----------------------------------------------------------------------
//
// Function: NumXTree2TraverseNext()
//
//   Given a stack describing a traversal state, return the next key/val
//   pair and update the stack.
//
// Global Variables: -
//
// Side Effects    : Updates stack
//
/---------------------------------------------------------------------*/

NumXTree2_p NumXTree2TraverseNext(PStack_p state, long* last_seen_key)
{
   NumXTree2_p handle, res;

   if(PStackEmpty(state))
   {
      return NULL;
   }
   res = PStackTopP(state);

   for(int i = ((*last_seen_key & (NUMXTREEVALUES - 1)) + 1) & (NUMXTREEVALUES - 1); i < NUMXTREEVALUES - 1; i++) 
   {
      if(res->vals[i].p_val)
      {  
         // val at index is not null
         return res;
      }
   }

   PStackDiscardTop(state);
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


