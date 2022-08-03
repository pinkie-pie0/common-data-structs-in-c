#include <stdio.h>
#include <stdlib.h>
#include "deque.h"

#define DS_NAME "avltree"
#include "err/ds_assert.h"

#define NODE_HEIGHT(node) (((node) == NULL) ? (-1) : (node->height))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct avltree_node avltree_node;
typedef struct avltree_ds {
	int (*compare)(const void*, const void*);
	struct avltree_node {
		void *val;
		int height;
		avltree_node *left;
		avltree_node *right;
	} *root;
} avltree_ds;

typedef struct avltree_ds_iterator {
	avltree_ds *this;
	deque_ds *stack;
} avltree_ds_iterator;

static avltree_node *alloc_avltree_node(void *val) {
	avltree_node *node = malloc(sizeof *node);
	if (node != NULL) {
		node->val = val;
		node->height = 0;
		node->left = NULL;
		node->right = NULL;
	}
	return node;
}

avltree_ds *alloc_avltree(int comparator(const void*, const void*)) {
	avltree_ds *this = malloc(sizeof *this);
	DS_ASSERT(this != NULL, "failed to allocate memory for new " DS_NAME);

	this->compare = comparator;
	this->root = NULL;
	return this;
}

static void avltree_delete_subtree(avltree_node *root) {
	if (root != NULL) {
		avltree_delete_subtree(root->left);
		avltree_delete_subtree(root->right);
		free(root);
	}
}

void dealloc_avltree(avltree_ds *const this) {
	avltree_delete_subtree(this->root);
	free(this);
}

#undef DS_NAME
#define DS_NAME "avltree iterator"

avltree_ds_iterator *alloc_avltree_iterator(avltree_ds *const this) {
	avltree_ds_iterator *itr = malloc(sizeof *itr);
	DS_ASSERT(itr != NULL, "failed to allocate memory for new " DS_NAME);
	
	itr->this = this;
	itr->stack = alloc_deque();
	if (this->root != NULL) {
		deque_push(itr->stack, this->root);
		while (((avltree_node*)deque_stackpeek(itr->stack))->left != NULL) {
			deque_push(itr->stack, ((avltree_node*)deque_stackpeek(itr->stack))->left);
		}
	}
	return itr;
}

void dealloc_avltree_iterator(avltree_ds_iterator *const itr) {
	dealloc_deque(itr->stack);
	free(itr);
}

int avltree_iterator_hasnext(avltree_ds_iterator *const itr) {
	return !deque_isempty(itr->stack);
}

void *avltree_iterator_next(avltree_ds_iterator *const itr) {
	avltree_node *process;
	void *val;
	
	DS_ASSERT(avltree_iterator_hasnext(itr), "no elements left to iterate");
	
	process = deque_pop(itr->stack);
	val = process->val;
	
	if (process->right != NULL) {
		deque_push(itr->stack, process->right);
		while (((avltree_node*)deque_stackpeek(itr->stack))->left != NULL) {
			deque_push(itr->stack, ((avltree_node*)deque_stackpeek(itr->stack))->left);
		}
	}
	
	return val;
}

#undef DS_NAME
#define DS_NAME "avltree"

static void avltree_rotate_right(avltree_node **const rootref) {
	int left_height, right_height;
	
	avltree_node *left = (*rootref)->left;
	avltree_node *left_right = (*rootref)->left->right;
	(*rootref)->left->right = *rootref;
	(*rootref)->left = left_right;
	*rootref = left;
	
	/* recalculating heights */
	left_height = NODE_HEIGHT((*rootref)->right->left);
	right_height = NODE_HEIGHT((*rootref)->right->right);
	(*rootref)->right->height = 1 + MAX(left_height, right_height);
	
	left_height = NODE_HEIGHT((*rootref)->left);
	right_height = NODE_HEIGHT((*rootref)->right);
	(*rootref)->height = 1 + MAX(left_height, right_height);
}

static void avltree_rotate_left(avltree_node **const rootref) {
	int left_height, right_height;
	
	avltree_node *right = (*rootref)->right;
	avltree_node *right_left = (*rootref)->right->left;
	(*rootref)->right->left = *rootref;
	(*rootref)->right = right_left;
	*rootref = right;
	
	/* recalculating heights */
	left_height = NODE_HEIGHT((*rootref)->left->left);
	right_height = NODE_HEIGHT((*rootref)->left->right);
	(*rootref)->left->height = 1 + MAX(left_height, right_height);
	
	left_height = NODE_HEIGHT((*rootref)->left);
	right_height = NODE_HEIGHT((*rootref)->right);
	(*rootref)->height = 1 + MAX(left_height, right_height);
}

#define avltree_rotate_leftright(rootref) do {\
	avltree_rotate_left(&(*rootref)->left); \
	avltree_rotate_right(rootref); \
} while (0)

#define avltree_rotate_rightleft(rootref) do {\
	avltree_rotate_right(&(*rootref)->right); \
	avltree_rotate_left(rootref); \
} while (0)

static int avltree_insert_helper(avltree_ds *const this, avltree_node **traversal, void *val) {
	int left_height, right_height;
	
	/* base case */
	if (*traversal == NULL) {
		*traversal = alloc_avltree_node(val);
		DS_ASSERT(*traversal != NULL, "failed to allocate new memory for new node");
		return 1;
	}
	
	if (this->compare(val, (*traversal)->val) < 0) {
		return avltree_insert_helper(this, &(*traversal)->left, val);

		if (NODE_HEIGHT((*traversal)->left) - NODE_HEIGHT((*traversal)->right) == 2) {
			if (this->compare(val, (*traversal)->left->val) < 0)
				avltree_rotate_right(traversal); /* imbalance due to '/'-shaped subtree */
			else
				avltree_rotate_leftright(traversal); /* imbalance due to '<' shaped subtree */
		}
		
	} else if (this->compare(val, (*traversal)->val) > 0) {
		return avltree_insert_helper(this, &(*traversal)->right, val);

		if (NODE_HEIGHT((*traversal)->right) - NODE_HEIGHT((*traversal)->left) == 2) {
			if (this->compare(val, (*traversal)->right->val) > 0)
				avltree_rotate_left(traversal); /* imbalance due to '\'-shaped subtree */
			else
				avltree_rotate_rightleft(traversal); /* imbalance due to '>'-shaped subtree */
		}
		
	} else return 0;
	
	/* recalculating heights */
	left_height = NODE_HEIGHT((*traversal)->left);
	right_height = NODE_HEIGHT((*traversal)->right);
	(*traversal)->height = 1 + MAX(left_height, right_height);
}

int avltree_insert(avltree_ds *const this, void *val) {
	return val != NULL ? avltree_insert_helper(this, &this->root, val) : 0;
}

static int avltree_remove_helper(avltree_ds *const this, avltree_node **traversal, void *val) {
	int left_height, right_height, balance;
	if (*traversal == NULL) {
		return 0;
	}
	
	if (this->compare(val, (*traversal)->val) < 0) {
		return avltree_remove_helper(this, &(*traversal)->left, val);
	} else if (this->compare(val, (*traversal)->val) > 0) {
		return avltree_remove_helper(this, &(*traversal)->right, val);
	} else { /* target deleletion found */
		
		if ((*traversal)->left != NULL && (*traversal)->right != NULL) { /* has two children: inner node */
			avltree_node *minimum = (*traversal)->right;
			while (minimum->left != NULL) {
				minimum = minimum->left;
			}
			
			(*traversal)->val = minimum->val;
			return avltree_remove_helper(this, &(*traversal)->right, minimum->val);
		} else if ((*traversal)->left == NULL && (*traversal)->right == NULL) { /* has no children: leaf node */
			free(*traversal);
			*traversal = NULL;
		} else { /* has either left or right child */
			avltree_node *connection = ((*traversal)->left != NULL) ? (*traversal)->left : (*traversal)->right;
			avltree_node *target = *traversal;
			*traversal = connection;	/* analogous to assigning connection to either parent->left or parent->right */
			free(target);
		}
		
	}
	
	if (*traversal == NULL) {
		return 1;
	}
	
	/* recalculating heights */
	left_height = NODE_HEIGHT((*traversal)->left);
	right_height = NODE_HEIGHT((*traversal)->right);
	(*traversal)->height = 1 + MAX(left_height, right_height);
	
	/* check if any rotations need to be done */
	balance = NODE_HEIGHT((*traversal)->left) - NODE_HEIGHT((*traversal)->right);
	if (balance == 2) {
			if (NODE_HEIGHT((*traversal)->left->left) - NODE_HEIGHT((*traversal)->left->right) >= 0)
				avltree_rotate_right(traversal);
			else
				avltree_rotate_leftright(traversal);
	} else if (balance == -2) {
			if (NODE_HEIGHT((*traversal)->right->left) - NODE_HEIGHT((*traversal)->right->right) <= 0)
				avltree_rotate_left(traversal);
			else
				avltree_rotate_rightleft(traversal);
	}
	
	return 1;
}

int avltree_remove(avltree_ds *const this, void *val) {
	return val != NULL ? avltree_remove_helper(this, &this->root, val) : 0;
}

void *avltree_min(avltree_ds *const this) {
	if (this->root != NULL) {
		avltree_node *traversal = this->root;
		while (traversal->left != NULL)
			traversal = traversal->left;
		return traversal->val;
	}
	return NULL;
}

void *avltree_max(avltree_ds *const this) {
	if (this->root != NULL) {
		avltree_node *traversal = this->root;
		while (traversal->right != NULL)
			traversal = traversal->right;
		return traversal->val;
	}
	return NULL;
}
