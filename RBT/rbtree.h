/*******************************************************************************
 * Name        : rbtree.h
 * Author      : Zakariyya Scavotto & Justin Chen
 * Version     : 1.0
 * Date        : 11/11/2023
 * Description : Implementation of red-black tree.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#ifndef RBTREE_H_
#define RBTREE_H_

#include "node.h"
#include "tree.h"
#include "treeprinter.h"
#include <iostream>
#include <cstdlib>
#include <exception>
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>

// Forward declaration
template <typename K, typename V>
class RedBlackTree;

/**
 * tree_exception class
 * Demonstrates how you can write your own custom exceptions in C++.
 */
class tree_exception: public std::exception {
public:
    tree_exception(const std::string &message) : message_{message} { }

    ~tree_exception() throw() { }

    virtual const char* what() const throw() {
        return message_.c_str();
    }

private:
    std::string message_;
};

template <typename K, typename V>
class RedBlackTreeIterator {
public:
    /**
     * Constructor
     */
    RedBlackTreeIterator() : node_ptr{nullptr}, tree{nullptr} { }

    /**
     * Equality operator. Compares node pointers.
     */
    bool operator==(const RedBlackTreeIterator &rhs) const {
        return node_ptr == rhs.node_ptr;
    }

    /**
     * Inequality operator. Compares node pointers.
     */
    bool operator!=(const RedBlackTreeIterator &rhs) const {
        return node_ptr != rhs.node_ptr;
    }

    /**
     * Dereference operator. Returns a reference to the Node pointed to
     * by node_ptr.
     */
    Node<K, V>& operator*() const {
        return *node_ptr;
    }

    /**
     * Dereference operator. Returns a pointer to the Node pointed to
     * by node_ptr.
     */
    Node<K, V>* operator->() const {
        return node_ptr;
    }

    /**
     * Preincrement operator. Moves forward to next larger value.
     */
    RedBlackTreeIterator& operator++() {
        Node<K, V> *p;

        if (node_ptr == nullptr) {
            // ++ from end(). Get the root of the tree.
            node_ptr = tree->root_;

            // Error, ++ requested for an empty tree.
            if (node_ptr == nullptr)
                throw tree_exception(
                        "RedBlackTreeIterator operator++(): tree empty");

            // Move to the smallest value in the tree, which is the first node
            // in an inorder traversal.
            while (node_ptr->left != nullptr) {
                node_ptr = node_ptr->left;
            }
        } else {
            if (node_ptr->right != nullptr) {
                // Successor is the leftmost node of right subtree.
                node_ptr = node_ptr->right;

                while (node_ptr->left != nullptr) {
                    node_ptr = node_ptr->left;
                }
            } else {
                // Have already processed the left subtree, and
                // there is no right subtree. Move up the tree,
                // looking for a parent for which node_ptr is a left child,
                // stopping if the parent becomes nullptr (or in this case,
                // root_parent_. A non-null parent is the successor. If parent
                // is nullptr, the original node was the last node inorder, and
                // its successor is the end of the list.
                p = node_ptr->parent;
                while (p != nullptr && node_ptr == p->right) {
                    node_ptr = p;
                    p = p->parent;
                }

                // If we were previously at the rightmost node in
                // the tree, node_ptr = nullptr, and the iterator specifies
                // the end of the list.
                node_ptr = p;
            }
        }

        return *this;
    }

    /**
     * Postincrement operator. Moves forward to next larger value.
     */
    RedBlackTreeIterator operator++(int) {
        RedBlackTreeIterator tmp(*this);
        operator++(); // prefix-increment this instance
        return tmp;   // return value before increment
    }

private:
    // node_ptr is the current location in the tree. We can move
    // freely about the tree using left, right, and parent.
    // tree is the address of the RedBlackTree object associated
    // with this iterator. It is used only to access the
    // root pointer, which is needed for ++ and --
    // when the iterator value is end().
    Node<K, V> *node_ptr;
    RedBlackTree<K, V> *tree;
    friend class RedBlackTree<K, V>;

    /**
     * Constructor used to construct an iterator return value from a tree
     * pointer.
     */
    RedBlackTreeIterator(Node<K, V> *p, RedBlackTree<K, V> *t) :
        node_ptr(p), tree(t) { }
};

template<typename K, typename V>
class RedBlackTree : public Tree {
public:
    typedef RedBlackTreeIterator<K, V> iterator;

    /**
     * Constructor to create an empty red-black tree.
     */
    explicit RedBlackTree() : root_{nullptr}, size_{0} { }

    /**
     * Constructor to create a red-black tree with the elements from the
     * vector.
     */
    explicit RedBlackTree(std::vector< std::pair<K, V> > &elements) :
                                            root_(nullptr), size_(0) {
        insert_elements(elements);
    }

    /**
     * Destructor.
     */
    ~RedBlackTree() {
        delete_tree(root_);
    }

    /**
     * Inserts elements from the vector into the red-black tree.
     * Duplicate elements are not inserted. Instead, we throw an error.
     * However, due to string concatenation being weird, there are instances where we write directly to cout instead of trying to
     * string concatenate on objects we don't know the type of (because C++ can't do dynamic typing)
     */
    void insert_elements(std::vector< std::pair<K, V> > &elements) {
        for (size_t i = 0, len = elements.size(); i < len; ++i) {
            try {
                insert(elements[i].first, elements[i].second);
            } catch (const tree_exception &te) {
                std::cerr << "Warning: " << te.what() << std::endl;
            }
        }
    }

    /**
     * Inserts a key-value pair into the red black tree.
     * const iterator &it indicates where to start the search for the place to
     * insert the node. If it == end(), the search starts at the root.
     * Must throw a tree_exception if attempting to insert a key that is
     * already present in the tree.
     */
    void insert(const iterator &it, const std::pair<K, V> &key_value) {
        const K& key = key_value.first;
        Node<K, V> *x, *y;
        if (it != end()) {
            x = it.node_ptr;
            y = x->parent;
        } else {
            x = root_;
            y = nullptr;
        }

        // Prepare a node to insert into the RBT
        Node<K, V> *z = new Node<K, V>(key, key_value.second);
        
        // If we already have the key in the tree (aka we haven't gone through all the nodes and haven't seen it),
        // don't add it in
        if (find(key) != end()) {

            // Use an ostringstream to create our error message string
            std::ostringstream temp;
            temp << "Attempt to insert duplicate key '" << key << "'.";
            // delete the node that can't be inserted before throwing the exception
            delete z;
            throw tree_exception(temp.str());
            //throw tree_exception("Attempt to insert duplicate key '" << key << "'.");
            
            return;
		}

        // The following code is purely based off of CLRS ch. 13's pseudo-code for insertion
        // This performs a "BST-like" insertion operation.
        while (x != nullptr){
            y = x;
            if (key < x->value()){
                x = x->left;
            }
            else{
                x = x->right;
            }
        }
        z->parent = y;
        if (y == nullptr){
            root_ = z;
        }
        else if(key < y->value()){
            y->left = z;
        }
        else{
            y->right = z;
        }
        z->left = nullptr;
        z->right = nullptr;

        // Newly inserted nodes are red
        z->color = RED;
        size_++;        // Increment size so we have it as a variable

        // After we are done, fix up the RBT to preserve RBT properties and balance it out
        insert_fixup(z);
    }

    /**
     * Inserts a key-value pair into the red-black tree.
     */
    void insert(const K &key, const V &value) {
        iterator e = end();
        insert(e, std::pair<K, V>(key, value));
    }

    /**
     * Returns an ASCII representation of the red-black tree.
     */
    std::string to_ascii_drawing() {
        BinaryTreePrinter<K, V> printer(root_);
        return printer.to_string();
    }

    /**
     * Returns the height of the red-black tree.
     */
    int height() const {
        return height(root_);
    }

    /**
     * Returns the number of nodes in the red-black tree.
     */
    size_t size() const {
        return size_;
    }

    /**
     * Returns the leaf count of the red-black tree.
     */
    size_t leaf_count() const {
        return leaf_count(root_);
    }

    /**
     * Returns the internal node count of the red-black tree.
     */
    size_t internal_node_count() const {
        return internal_node_count(root_);
    }

    /**
     * Returns the diameter of the red-black tree rooted at node.
     * Diameter is defined as the length of the longest path between
     * two (non-null) leaves in the tree. The path does not necessarily have to
     * pass through the root.
     */
    size_t diameter() const {
        // TODO
        return diameter(root_);
    }

    /**
     * Returns the max width of the red-black tree, i.e. the largest number of nodes on any level.
     * @return The size of the maximum width of the RBT
     */
    size_t max_width() const {
        size_t max_width = 0;

        // Iterate through each "height level" of the tree and find the maximum width
        // Height needs to be incremented by 1 because we started counting at -1 (for null tree)
        for (int i = 0, h = height(root_) + 1; i < h; i++) {
            size_t w = width(root_, i);
            if (w > max_width) {
                max_width = w;
            }
        }
        return max_width;
    }

    /**
     * Returns the successful search cost, i.e. the average number of nodes
     * visited to find a key that is present.
     */
    double successful_search_cost() const {
        return size_ == 0 ? 0 : 1 + (double)sum_levels() / size_;
    }

    /**
     * Returns the unsuccessful search cost, i.e. the average number of nodes
     * visited to find a key that is not present.
     */
    double unsuccessful_search_cost() const {
        return (double)sum_null_levels() / null_count();
    }

    /**
     * Searches for item. If found, returns an iterator pointing
     * at it in the tree; otherwise, returns end().
     */
    iterator find(const K &key) {
        Node<K, V> *x = root_;
        while (x != nullptr) {
            const K& current_key = x->key();
            if (key == current_key) {
                break; // Found!
            } else if (key < current_key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return iterator(x, this);
    }

    /**
     * Returns an iterator pointing to the first item in order.
     */
    iterator begin() {
        Node<K, V> *curr = root_;

        // if the tree is not empty, the first node
        // in order is the farthest node left from root
        if (curr != nullptr) {
            while (curr->left != nullptr) {
                curr = curr->left;
            }
        }

        // build return value using private constructor
        return iterator(curr, this);
    }

    /**
     * Returns an iterator pointing just past the end of the tree data.
     */
    iterator end() {
        return iterator(nullptr, this);
    }

private:
    Node<K, V> *root_;
    size_t size_;
    friend class RedBlackTreeIterator<K, V>;

    /**
     * Deletes all nodes from the red-black tree.
     * Note: if a node is created that is not added to the BST, you'll need to deallocate them seperately to prevent valgrind
     * @param n For users, this should be the root node. Otherwise, it's the current node we're recursively deleting.
     */
    void delete_tree(Node<K, V> *n) {

        // Based off of our BST implementation - this recursively deletes the nodes
        if (n != nullptr) {
            delete_tree(n->left);
            delete_tree(n->right);
            delete n;
        }
    }

    /**
     * Helper function to get the color of the currentNode's parent
     * @param current_node Pointer to the current node
     * @return The parent's color
    */
    int get_parent_color(Node<K, V> *current_node) {

        if (current_node->parent != nullptr)
        {
            return current_node->parent->color;
        }

        return BLACK;           // If there is no parent, the node is a "black" node
    }

    /**
     * Helper function to get the color of the currentNode's grandparent
     * @param current_node Pointer to the current node
     * @return The grandparent's color
    */
    int get_grandparent_color(Node<K, V> *current_node) {

        if (current_node->parent != nullptr && current_node->parent->parent != nullptr)
        {
            return current_node->parent->color;
        }

        return BLACK;       // If there is no grandparent, the node is a "black" node
    }

    /**
     * Helper function to get the color of the currentNode's grandparent
     * @param uncle Pointer to the uncle node
     * @return The uncle's color
    */
    int get_uncle_color(Node<K, V> *uncle) {

        if (uncle != nullptr)
        {
            return uncle->color;
        }

        return BLACK;       // If there is no uncle, the node is a "black" node

    }

    /**
     * Fixup method described on p. 316 of CLRS.
     * This takes the node we currently inserted and recursively fixes the red black tree up
     * @param z The most recently added node
     */
    void insert_fixup(Node<K, V> *z) {

        // Change the root color to black
        root_->color = BLACK;

        Node<K,V> *current_node = z;      // Rename *z to currentNode because it'll be much easier to debug later

        // Create a temporary pointer for y to represent the uncle node we're on
        Node<K,V> *uncle;

        // While we're not at the root node or the root node's children (they will always be RBT'd properly)
        while (current_node->parent != nullptr && current_node->parent->parent != nullptr && get_parent_color(current_node) == RED) {

            // If we're on the left side of the tree
            if (current_node->parent == current_node->parent->parent->left) 
            {
                uncle = current_node->parent->parent->right;

                // Case 1: The parent and uncle are both red. Note we already checked if the parent is red
                if (uncle != nullptr && get_uncle_color(uncle) == RED) {

                    // Set both to black
                    current_node->parent->color = BLACK;
                    uncle->color = BLACK;

                    // Repaint grandparent to red
                    current_node->parent->parent->color = RED;

                    // Set current node to one above
                    current_node = current_node->parent->parent;
                } 
                else {

                    if (current_node == current_node->parent->right) 
                    {
                        // Case 2: The uncle is black (or there is no uncle) and the child is on the right
                        //From Meunier's class notes, this converts into case 3
                        current_node = current_node->parent;
                        left_rotate(current_node);
                    }       

                    // If code reaches here, that means the uncle doesn't exist, the parent is red, and the child is on the left
                    // This is case 3
                    current_node->parent->color = BLACK;
                    current_node->parent->parent->color = RED;
                    right_rotate(current_node->parent->parent);
                }
            }
            else            // Do the exact same, but for the right
            {
                uncle = current_node->parent->parent->left;

                // Case 1: The parent and uncle are both red
                if (uncle != nullptr && get_uncle_color(uncle) == RED) {
                    current_node->parent->color = BLACK;
                    uncle->color = BLACK;
                    current_node->parent->parent->color = RED;
                    current_node = current_node->parent->parent;
                }
                else {
                    // Case 2: The uncle is black (or there is no uncle) and the child is on the left
                    // Converts it to case 3
                    if (current_node == current_node->parent->left) {
                        current_node = current_node->parent;
                        right_rotate(current_node);
                    }

                    // If code reaches here, that means the uncle doesn't exist, the parent is red, and the child is on the left
                    // This is case 3

                    current_node->parent->color = BLACK;
                    current_node->parent->parent->color = RED;
                    left_rotate(current_node->parent->parent);
                }
            }
        }
        
        // Change the root color to black
        root_->color = BLACK;
    }

    /**
     * Performs a left-rotate on the provided BST node
     * Left-rotate method described on p. 313 of CLRS.
     * If we rotate to a new root, update it
     * @param x The node to left rotate
     */
    void left_rotate(Node<K, V> *x) {
        // Based on textbook pseudocode
        Node<K,V> *y = x->right; 

        // Set the right node (pointer) to the left node of the child (pointer).
        // Edit the parent pointer too, if possible
        x->right = y->left;
        if (y->left != nullptr){
            y->left->parent = x;
        }

        // Fix the "child node's" parent (pointer) to x's pointer
        // Edit the parent pointer too to check for null pointer (since that would be the root)
        y->parent = x->parent;

        if (x->parent == nullptr){
            root_ = y;                  // If x was the root, then set it to root
        }
        else if (x == x->parent->left){
            x->parent->left = y;        // If x was on the left side of the parent, then the parent's left to y
        }
        else if (x == x->parent->right){
            x->parent->right = y;       // Repeat for the right
        }

        // Since it's a left rotate, set y's left node to x
        y->left = x;
        x->parent = y;  // Set x's parent
    }

    /**
     * Preforms a right-rotate method as described on p. 313 of CLRS.
     * If we rotate to a new root, update it
     * @param x The node to perform a right rotate on
     */
    void right_rotate(Node<K, V> *x) {
        // Since we're doing a right rotate, set the left node (pointer) to the right node of the child (pointer)
        Node<K,V> *y = x->left; 
        x->left = y->right;
        if (y->right != nullptr){
            y->right->parent = x;
        }

        // Fix the "child node's" parent (pointer) to x's pointer
        // Edit the parent pointer too to check for null pointer (since that would be the root)
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root_ = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        
        // Since it's a right rotate, set y's right node to x
        y->right = x;
        x->parent = y;      // Set x's parent
    }

    /**
     * Recursively calculates the height of the red-black tree starting at node.
     * A null node starts at height -1.
     * @param node The node to calculate the height of
     */
    int height(Node<K, V> *node) const {
        // TODO
        if (node == nullptr){
            return -1;
        }

        // The height will be the max height of either subtree (for the height of our current node)
        // Then, we increment by 1 to take into account the current level
        return std::max(height(node->left), height(node->right)) + 1;
    }

    /**
     * Recursively calculates and returns the count of leaves in the red-black tree starting at node.
     * For this method, a leaf is a non-null node that has no children.
     * @param node The node to find the number of leaf nodes
     * @return The number of leaf nodes
     */
    size_t leaf_count(Node<K, V> *node) const {
        size_t toReturn = 0;

        // A null item cannot have leaves
		if (node == nullptr){
			return 0;
        }

        // Looks at the left and right child for leaf nodes. If a leaf node is spotted, add it to toReturn
		if (node->right != nullptr){
			toReturn += leaf_count(node->right);
        }
		if ((node->left != nullptr)){
			toReturn += leaf_count(node->left);
        }

        // Base case for recursion: if the node has no left and right child, it is a leaf node itself
		if ((node->left == nullptr) && (node->right == nullptr)){
			toReturn++;
        }

        return toReturn;
    }

    /**
     * Recursively calculates the count of internal nodes in the red-black tree starting a node.
     * An internal node has at least one child.
     * @param node The node to calculate the internal node from
     * @return The number of internal nodes, starting from parent $node
     */
    size_t internal_node_count(Node<K, V> *node) const {
        // If a node is null, it has no internal node
        if (node == nullptr){
			return 0;
        }
		else if (node->left != nullptr || node->right != nullptr){
            // If a node has children in left/right, it's an internal node.
            // Find internal node of its children, and add itself
			return internal_node_count(node->left) + internal_node_count(node->right) + 1;
        } 
		else {
            // Doesn't really do much since both notes are null pointer here
            // However, our base case is the nullptr so we traverse that recursively
			return internal_node_count(node->left) + internal_node_count(node->right);
        }
    }

    /**
     * Helper method to assist in the computation of tree diameter.
     * @param node Find the diameter of the tree with this specified parent
     * @return An integer representing the tree diameter
     */
    int diameter(Node<K, V> *node) const {
        
        // If the tree does not exist, its diameter is literally 0
        if (node == nullptr) {
            return 0;
        }

        // Get the heights and diameters of the subtrees to compare to see which is greater
        int leftHeight = height(node->left);
        int rightHeight = height(node->right);

        int leftDiam = diameter(node->left);
        int rightDiam = diameter(node->right);

        // Set our largest diameter found between the two subtrees
        int largestDiameter = std::max(leftDiam, rightDiam);

        // Now, compare the sum of the heights (+1 for each of our current nodes) to the biggest diameter we already
        return std::max(leftHeight+rightHeight + 2, largestDiameter);
    }

    /**
     * Recursively calculates and returns the width of the red-black tree at the designated level.
     * Width is defined as the number of nodes residing at a level.
     * @param node The node to start from to calculate the width
     * @param level The level we are currently on
     * @return The width of the current node
     */
    size_t width(Node<K, V> *node, size_t level) const {
        // A null node has no width
        if (node == nullptr) {
            return 0;
        }
        else if (level == 0) {       // If we reach the desired level, that is 1.
            return 1;
        }
        else {          // For any other node, the width we want is a combination of it's left and right node
            return width(node->left, level-1) + width(node->right, level-1);
        }
    }

    /**
     * @return Null count init
    */
    size_t null_count() const {
        return null_count(root_);
    }

    /**
     * Returns the count of null pointers in the red-black tree starting at node.
     * @param node The node to start counting at
     * @return The number of null pointers
     */
    size_t null_count(Node<K, V> *node) const {
        size_t toReturn = 0;    // Tally

        // A null node has itself as a null pointer
        if (node == nullptr){
            return 1;
        }

        // If left is not null, get the null_count of the left subtree
        if (node->left != nullptr){
            toReturn += null_count(node->left);
        }
        else{ // Otherwise we found another null
            toReturn += 1;
        }

        // Repeat for the right node
        if (node->right != nullptr){
            toReturn += null_count(node->right);
        }
        else{
            toReturn += 1;
        }

        return toReturn;
    }

    /**
     * @return Sum level init
    */
    size_t sum_levels() const {
        return sum_levels(root_, 0);
    }

    /**
     * Returns the sum of the levels of each non-null node in the red-black
     * tree starting at node.
     * For example, the tree
     *   5 <- level 0
     *  / \
     * 2   8 <- level 1
     *      \
     *       10 <- level 2
     * has sum 1*0 + 2*1 + 1*2 = 4.
     */
    size_t sum_levels(Node<K, V> *node, size_t level) const {
        // If a root is not null, we can sum the levels recursively by traversing down
        if (node!= nullptr) {
            return level + sum_levels(node->left, level+1) + sum_levels(node->right, level+1);
        }
        else {
            return 0;   // If we reach the bottom, that has no levels. So, we add 0
        }
    }

    /**
     * Init for sum null levels
    */
    size_t sum_null_levels() const {
        return sum_null_levels(root_, 0);
    }

    /**
     * Returns the sum of the levels of each null node in the red-black tree
     * starting at node.
     * For example, the tree
     *    5 <- level 0
     *   / \
     *  2   8 <- level 1
     * / \ / \
     * * * * 10 <- level 2
     *       / \
     *       * * <- level 3
     * has sum 3(2) + 2(3) = 12.
     */
    size_t sum_null_levels(Node<K, V> *node, size_t level) const {
        
        // If a node is not null, recursively add the number of null levels below it
        if (node!= nullptr){
            return sum_null_levels(node->left, level+1) + sum_null_levels(node->right, level+1);
        }
        else {
            return level;   // If it's null, POGCHAMP! That's what we want. Now return that bozo
        }
    }
};

#endif /* RBTREE_H_ */
