#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NODE_HEIGHT(node) (((node) == NULL) ? (-1) : (node->height))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct avltree_node avltree_node;
typedef struct avltree_T {
	int (*compare)(const void*, const void*);
	struct avltree_node {
		void *val;
		int8_t height;
		avltree_node *left;
		avltree_node *right;
	} *root;
} avltree_T;

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

avltree_T *alloc_avltree(int comparator(const void*, const void*)) {
	avltree_T *this = malloc(sizeof *this);
	if (this == NULL) {
		fprintf(stderr, "**avltree memory allocation failure** : failed to allocate new avltree\n");
		exit(EXIT_FAILURE);
	}

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

void dealloc_avltree(avltree_T *this) {
	avltree_delete_subtree(this->root);
	free(this);
}

static void avltree_rotate_right(avltree_node **rootref) {
	int8_t left_height, right_height;
	
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

static void avltree_rotate_left(avltree_node **rootref) {
	int8_t left_height, right_height;
	
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

static void avltree_insert_helper(avltree_T *this, avltree_node **traversal, void *val) {
	int8_t left_height, right_height;
	
	/* base case */
	if (*traversal == NULL) {
		*traversal = alloc_avltree_node(val);
		if (*traversal == NULL) {
			fprintf(stderr, "**avltree memory allocation failure** : failed to allocate new node\n");
			dealloc_avltree(this);
			exit(EXIT_FAILURE);
		}
		return;
	}
	
	if (this->compare(val, (*traversal)->val) < 0) {
		avltree_insert_helper(this, &(*traversal)->left, val);

		if (NODE_HEIGHT((*traversal)->left) - NODE_HEIGHT((*traversal)->right) == 2) {
			if (this->compare(val, (*traversal)->left->val) < 0)	/* imbalance occurs due to '/'-shaped subtree */
				avltree_rotate_right(traversal);
			else													/* imbalance occurs due to '<' shaped subtree */
				avltree_rotate_leftright(traversal);
		}
		
	} else if (this->compare(val, (*traversal)->val) > 0) {
		avltree_insert_helper(this, &(*traversal)->right, val);

		if (NODE_HEIGHT((*traversal)->right) - NODE_HEIGHT((*traversal)->left) == 2) {
			if (this->compare(val, (*traversal)->right->val) > 0)	/* imbalance occurs due to '\'-shaped subtree */
				avltree_rotate_left(traversal);
			else													/* imbalance occurs due to '>'-shaped subtree */
				avltree_rotate_rightleft(traversal);
		}
		
	}
	
	/* recalculating heights */
	left_height = NODE_HEIGHT((*traversal)->left);
	right_height = NODE_HEIGHT((*traversal)->right);
	(*traversal)->height = 1 + MAX(left_height, right_height);
}

void avltree_insert(avltree_T *this, void *val) {
	avltree_insert_helper(this, &this->root, val);
}
