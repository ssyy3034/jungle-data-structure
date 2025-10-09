#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if(!p) {
    return NULL;
  }
  node_t *nil = (node_t *)calloc(1,sizeof(*nil));
  
  if (!nil) {
    free(p); 
    return NULL;
  }
  
  nil->color = RBTREE_BLACK;
  nil->left = nil->right = nil->parent =nil;
  p->nil = nil;
  p->root = nil;

  return p;
}

static void free_subtree(rbtree *t,node_t *x){
  if(x==t->nil) return;
  free_subtree(t,x->left);
  free_subtree(t,x->right);
  free(x);
}

void delete_rbtree(rbtree *t) {
  if (!t) return;
  free_subtree(t,t->root);
  free(t->nil);
  free(t);
}

static void left_rotate(rbtree *t,node_t *x){
  node_t *y = x->right;
  x->right = y->left;
  if(y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if(x->parent==t->nil){
    t->root = y;
  }
  else if (x==x->parent->left)
  {
    x->parent->left =y;
  }
  else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

static void right_rotate(rbtree *t,node_t *y){
  node_t *x = y->left;
  y->left = x->right;
  if(x->right != t->nil){
    x->right->parent = y;
  }
  x->parent = y->parent;
  if(y->parent==t->nil){
    t->root = x;
  }
  else if (y==y->parent->left)
  {
    y->parent->left =x;
  }
  else{
    y->parent->right = x;
  }
  x->right = y;
  y->parent = x;
}


static void rb_fixup(rbtree *t,node_t *node){
  node_t *sibling = t->nil;
  while(node->parent->color == RBTREE_RED){
    if(node->parent == node->parent->parent->left){
      sibling = node->parent->parent->right;
      if(sibling->color == RBTREE_RED){
        node->parent->color = RBTREE_BLACK;
        sibling->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }
      else{
        if(node == node->parent->right){
          node = node->parent;
          left_rotate(t,node);
        }
        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        right_rotate(t,node->parent->parent);
      }
    }
    else{
      sibling = node->parent->parent->left;
      if(sibling->color == RBTREE_RED){
        node->parent->color = RBTREE_BLACK;
        sibling->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        node = node->parent->parent;
      }
      else{
        if(node == node->parent->left){
          node = node->parent;
          right_rotate(t,node);
        }
        node->parent->color = RBTREE_BLACK;
        node->parent->parent->color = RBTREE_RED;
        left_rotate(t,node->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  t->root->parent = t->nil;
}

static node_t *init_new_node(rbtree *t, key_t key) {
  node_t *z = (node_t *)malloc(sizeof *z);
  if (!z) return NULL;
  z->key   = key;
  z->color = RBTREE_RED;
  z->left = z->right = z->parent = t->nil;
  return z;
}




node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *z = init_new_node(t,key);
  node_t *x = t->root;
  node_t *y = t->nil;

  while(x!=t->nil){
    y=x;
    if(key < x->key){
      x= x->left;
    }
    else{
      x=x->right;
    }
  }
  z->parent = y;
  if(y==t->nil){
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else{
    y->right = z;
  
  }

  rb_fixup(t,z);
  t->root->parent = t->nil;
  return z;

}
  


node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *find = t->root;
  while(find !=t->nil&& find->key != key){
    if(key<find->key){
      find = find->left;
    }
    else{
      find= find->right;
    }
  }
  if(find==t->nil){
    return NULL;
  }
  return find;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *min_node = t->root;
  if(min_node==t->nil) return NULL;

  while(min_node->left != t->nil){
    min_node = min_node->left;
    }
  return min_node;
}


node_t *rbtree_max(const rbtree *t) {
  node_t *max_node = t->root;
  if(max_node==t->nil) return NULL;

  while(max_node->right != t->nil){
    max_node = max_node->right;
    }
  return max_node;
}

node_t *rbtree_min_sub(const rbtree *t, node_t *subtree) {
  node_t *min_node = subtree;
  if (min_node == t->nil) return t->nil;
  while (min_node->left != t->nil)
    min_node = min_node->left;
  return min_node;
}

static void rb_transplant(rbtree *t,node_t *u,node_t *v){
  if(u->parent ==t->nil){
    t->root = v;
  }
  else if( u==u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}
static void rb_delete_fixup(rbtree *t,node_t *x){
  node_t *sibling;
  while(x != t->root && x->color == RBTREE_BLACK){
    if(x==x->parent->left){
      sibling=x->parent->right;
      if(sibling->color ==RBTREE_RED){
        sibling->color=RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t,x->parent);
        sibling = x->parent->right;
      }
      if(sibling->left->color ==RBTREE_BLACK && sibling->right->color == RBTREE_BLACK){
        sibling->color= RBTREE_RED;
        x=x->parent;
      }
      else{
        if (sibling->right->color ==RBTREE_BLACK){
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          right_rotate(t,sibling);
          sibling = x->parent->right;
        }
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        left_rotate(t,x->parent);
        x = t->root;
      }
    }
    else{
      sibling=x->parent->left;
      if(sibling->color ==RBTREE_RED){
        sibling->color=RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t,x->parent);
        sibling = x->parent->left;
      }
      if(sibling->left->color ==RBTREE_BLACK && sibling->right->color == RBTREE_BLACK){
        sibling->color= RBTREE_RED;
        x=x->parent;
      }
      else{
        if (sibling->left->color ==RBTREE_BLACK){
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t,sibling);
          sibling = x->parent->left;
        }
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color =RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  node_t *x;
  color_t y_origin = y->color;
  if(p->left == t->nil){
    x=p->right;
    rb_transplant(t,p,p->right);
  }
  else if(p->right == t->nil){
    x=p->left;
    rb_transplant(t,p,p->left);
  }
  else{
    y = rbtree_min_sub(t,p->right);
    y_origin = y->color;
    x=y->right;
    if(y!=p->right){
      rb_transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    else{
      x->parent = y;
    }
    rb_transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
    }
  if(y_origin == RBTREE_BLACK){
    rb_delete_fixup(t,x);
  } 
  return 0;
}

static void inorder_fill(const rbtree *t,node_t *x,key_t *arr,size_t *idx,size_t n){
  if(*idx>=n || x==t->nil) return;
  inorder_fill(t,x->left,arr,idx,n);
  if(*idx >=n) return;
  arr[*idx] = x->key;
  (*idx)++;
  inorder_fill(t,x->right,arr,idx,n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  size_t idx = 0;
  if(n==0) return 0;
  inorder_fill(t,t->root,arr,&idx,n);
  return 0;
}
