#include <iostream>
#include <limits>
#include <queue>
#include <algorithm> // for max
using namespace std;

/**
 * Your average node structure. 
 * Remember that depth == The height from the top of the tree
*/
struct Node {
	Node *left, *right, *parent;
	int data, depth;

	Node(Node *parent, int depth, int data) :
		left{nullptr}, right{nullptr}, parent{parent},
		data{data}, depth{depth} { }
};

// O(tree height) (and the tree height is O(n))

/**
 * Helps us insert the BST
*/
Node* insert_helper(Node **pnode, int x, Node *parent, int depth) {
	//If the node we end up at is a nullptr, that's the perfect time to insert a node!
	//So, let's insert the node!
	if(*pnode == nullptr) {
		return *pnode = new Node(parent, depth, x);
	}
	if((*pnode)->data == x) {		//If the item already exists, return. Pause. Stop. Do not collect $200.
		return nullptr;
	}
	if(x < (*pnode)->data) {		//Else, recursively insert the item
		return insert_helper(&(*pnode)->left, x, *pnode, depth + 1);
	} else {
		return insert_helper(&(*pnode)->right, x, *pnode, depth + 1);
	}
}

/**
 * Now, actually insert the tree
 * We use a pointer to the 
*/
Node* insert(Node **pnode, int x) {
	return insert_helper(pnode, x, nullptr, 0);
}

// Θ(n)
void display_sideways(Node *node) {
	//Recursively prints the right subtree
	if(node != nullptr) {
		display_sideways(node->right);
		if(node->depth == 0) {				//if it's the root, denote it
			cout << "-> ";
		} else {
			cout << "   ";
		}
		for(int i = 0; i < node->depth; ++i) {
			cout << "   ";						//Spacing issues
		}
		cout << node->data << endl;
		display_sideways(node->left);
	}
}

// O(tree height) (and the tree height is O(n))
Node* find(Node *node, int x) {
	//If we get to a null pointer, my search failed
	if(node == nullptr) {
		return nullptr;
	}
	if(node->data == x) {	//If we find it, POG. Return it.
		return node;
	}
	if(x < node->data) {	//Now, we recursively search.
		return find(node->left, x);
	} else {
		return find(node->right, x);
	}
}

// Sum all integers in the BST
// Θ(n)
int sum(Node *node) {
	if(node == nullptr) {
		return 0;
	} else {
		return node->data + sum(node->left) + sum(node->right);
	}
}

// Θ(n)
void decrement_depths(Node *node) {
	if(node != nullptr) {
		node->depth--;
		decrement_depths(node->left);
		decrement_depths(node->right);
	}
}

// Deletes and returns as result the minimum node in the non-empty tree.
// Since the tree is non-empty, such minimum node must exist.
// This code is similar to findMin except that we do not have to test
// the Null case since we know that the tree is non-empty.
// O(tree height) (and the tree height is O(n))
Node* delete_min(Node **pnode) {
	if((*pnode)->left == nullptr) {
		// current node is the minimum node, remove it from the tree and return it
		Node *min = *pnode;
		*pnode = min->right;
		decrement_depths(min->right);
		return min;
	} else {
		// Keep moving down-left
		return delete_min(&(*pnode)->left);
	}
}

// O(tree height) (and the tree height is O(n))
bool delete_node(Node **pnode, int x) {
	if(*pnode == nullptr) {
		// Cannot find x, deletion fails
		return false;
	}
	if((*pnode)->data == x) {
		// Case 1: the node is a leaf
		// Note: this case is in fact a special case of the next one (case 2-a
		// or case 2-b) so this code could be merged with the next case.
		// We keep this case separate here for clarity.
		if((*pnode)->left == nullptr && (*pnode)->right == nullptr) {
			delete *pnode;
			*pnode = nullptr;
			return true;
		}
		// Case 2-a: the node has one left child
		if((*pnode)->left != nullptr && (*pnode)->right == nullptr) {
			Node *tmp = *pnode;
			*pnode = tmp->left;
			decrement_depths(tmp->left);
			delete tmp;
			return true;
		}
		// Case 2-b: the node has one right child
		if((*pnode)->left == nullptr && (*pnode)->right != nullptr) {
			Node *tmp = *pnode;
			*pnode = tmp->right;
			decrement_depths(tmp->right);
			delete tmp;
			return true;
		}
		// Case 3: the node has two children
		// We replace the node with the minimum node in the right subtree
		// (The maximum node in the left subtree would work too.)
		if((*pnode)->left != nullptr && (*pnode)->right != nullptr) {
			Node *tmp = *pnode;
			// Find the smallest node in the right subtree:
			Node *min = delete_min(&tmp->right);
			// Replace the node with the min:
			min->parent = tmp->parent;
			min->depth = tmp->depth;
			min->left = tmp->left;
			min->right = tmp->right;
			*pnode = min;
			delete tmp;
			return true;
		}
	}
	if(x < (*pnode)->data) {
		return delete_node(&(*pnode)->left, x);
	} else {
		return delete_node(&(*pnode)->right, x);
	}
}

// Θ(n)
void delete_bst(Node *node) {
	if(node != nullptr) {
		delete_bst(node->left);
		delete_bst(node->right);
		delete node;
	}
}

// Create a balanced BST from a sorted array.
// Θ(n)
Node* create_bst_helper(int data[], int low, int high, Node *parent, int depth) {
	if(low > high) {
		return nullptr;
	}
	int mid = low + (high - low) / 2;
	Node *new_node = new Node(parent, depth, data[mid]);
	new_node->left = create_bst_helper(data, low, mid - 1, new_node, depth + 1);
	new_node->right = create_bst_helper(data, mid + 1, high, new_node, depth + 1);
	return new_node;
}

Node* create_bst(int data[], int length) {
	return create_bst_helper(data, 0, length - 1, nullptr, 0);
}

int main(int argc, const char *argv[]) {
	Node *bst1 = nullptr;
	insert(&bst1, 3);
	insert(&bst1, 5);
	insert(&bst1, 2);
	display_sideways(bst1);
	cout << endl;
	cout << "finding " << find(bst1, 5)->data << endl;
	cout << "sum is " << sum(bst1) << endl;
	cout << "deleting 3:" << endl << endl;
	delete_node(&bst1, 3);
	display_sideways(bst1);
	cout << endl;
	cout << "deleting 5:" << endl << endl;
	delete_node(&bst1, 5);
	display_sideways(bst1);
	cout << endl;
	delete_bst(bst1);
	bst1 = nullptr;

	int data[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	Node *bst2 = create_bst(data, 15);
	display_sideways(bst2);

	delete_bst(bst2);
	return 0;
}
