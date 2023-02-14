#ifndef ___SKIP_LIST_HPP
#define ___SKIP_LIST_HPP

#include <cmath> // for log2
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

/**
 * flipCoin -- NOTE: Only read if you are interested in how the
 * coin flipping works.
 * 
 * @brief Uses the bitwise representation of the key to simulate
 * the flipping of a deterministic coin.
 * 
 * This function looks at the bitwise representation to determine
 * how many layers it occupies in the skip list. It takes the bitwise
 * XOR of each byte in a 32-bit unsigned number and uses the index `previousFlips`
 * to determine the truth value. It's best to look at an example and
 * to start with values that fit into one byte.
 * 
 * Let's start with the function call `flipCoin(0, 0)`. This call is
 * asking whether or not we should insert the key `0` into layer `1`
 * (because `previousFlips` represents the number of previous flips).
 * 
 * This function will get the bitwise representation of 0: 
 * 
 *        00000000 00000000 00000000 00000000
 * 
 * All bytes are XOR'd together: 
 * 
 * 
 *  c = 0 ^ 0 ^ 0 ^ 0
 * 
 * Then the index `previousFlips` is used to obtain the bit in the `previousFlips % 8` position.
 * 
 * So the return value is `0 & 1 = 0`, since the value in the zeroth position 
 * (obtained by bitmasking the value (1 << 0) at the bottom of the function
 * is 0.
 * Thus, this value `0` should never get added beyond the bottom-most layer.
 * 
 * Before:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after 0 is inserted
 * 
 * S_1: -inf --------> inf
 * S_0: -inf --> 0 --> inf
 * 
 * Let's look at something more interesting, like the call `flipCoin(5, 0)`.
 * Remember the binary representation for 5 is 00000101.
 * 
 * c = 0 ^ 0 ^ 0 ^ 00000101 = 00000101
 * 
 * Now we get the bit at 0th position (from `previousFlips`).
 * 
 * 00000101 & 1 = 1. Thus, `5` DOES get inserted into the next layer,
 * layer 1.
 * 
 * So the skip list before this insertion might look like:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after it would look like:
 * 
 * S_2: -inf --------> inf
 * S_1: -inf --> 5 --> inf
 * S_0: -inf --> 5 --> inf
 * 
 * Observe that a new layer had to get created, because there should always
 * be an empty layer at the top.
 * 
 * Since 5 got inserted into the next layer, we need to flip again to see if
 * it should get propagated once more.
 * 
 * `flipCoin(5, 1)` this will produce a result of `0`, since 00000101 & (1 << 1) = 0.
 * 
 * Thus, `5` will not be pushed up to the next layer.
 * 
 * @param key The integer key which will be added to the skip list
 * @param previousFlips The number of previous flips for this key
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(unsigned key, unsigned previousFlips)
{
	char c;
	unsigned first8Bits = (key & 0xFF000000) / 0x01000000 ; 
	unsigned next8Bits =  (key & 0x00FF0000) / 0x00010000;
	unsigned andThen =	 (key & 0x0000FF00) / 0x00000100;
	unsigned lastBits =   (key & 0x000000FF);
	c = first8Bits ^ next8Bits ^ andThen ^ lastBits;
	previousFlips = previousFlips % 8;
	return ( c & (1 << previousFlips) ) != 0;
}

/**
 * @brief Works the same as the integer version above, except
 * it XORs chars in a string instead of the first four
 * bytes of an integer.
 * 
 * @param key key that will be inserted into the skip list
 * @param previousFlips number of previous flips for this key 
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(std::string key, unsigned previousFlips)
{
	char c = key[0];
	for(unsigned j = 1; j < key.length(); j++)
	{
		c = c ^ key[j];
	}
	previousFlips = previousFlips % 8;
	return ( c & (1 << previousFlips) ) != 0;	
}

template<typename Key, typename Value>
class SkipList
{
	struct Node{
		Key key;
		Value val;
		Node *next;
		Node *prev;
		Node *up;
		Node *down;
		
		Node(Key k, Value v, Node *n, Node *p, Node *u, Node *d){
			key = k;
			val = v;
			next = n;
			prev = p;
			up = u;
			down = d;
		}
	};

private:
	// private variables go here.
	Node *head;
	Key negKey;
	Value negVal;

public:

	SkipList();

	// You DO NOT need to implement a copy constructor or an assignment operator.

	~SkipList();

	// How many distinct keys are in the skip list?
	size_t size() const noexcept;

	// Does the Skip List contain zero keys?
	bool isEmpty() const noexcept;

	// How many layers are in the skip list?
	// Note that an empty Skip List has two layers by default,
	// the "base" layer S_0 and the top layer S_1.
	//
	// [S_1: Top layer]    -inf ------> inf
	// [S_0: Bottom layer] -inf ------> inf
	//
	// This "empty" Skip List has two layers and a height of one.
	unsigned numLayers() const noexcept;

	// What is the height of this key, assuming the "base" layer S_0
	// contains keys with a height of 1?
	// For example, "0" has a height of 1 in the following skip list.
	//
	// [S_1]  -inf --------> inf
	// [S_0]  -inf --> 0 --> inf
	//
	// Throw an exception if this key is not in the Skip List.
	unsigned height(const Key & k) const;

	// If this key is in the SkipList and there is a next largest key
	// return the next largest key.
	// This function should throw a RuntimeException if either the key doesn't exist
	// or there is no subsequent key. 
	// A consequence of this is that this function will
	// throw a RuntimeException if *k* is the *largest* key in the Skip List.
	Key nextKey(const Key & k) const;

	// If this key is in the SkipList and a next smallest key exists,
	// return the next smallest key.
	// This function should throw a RuntimeException if either the key doesn't exist
	// or there is no previous key. 
	// A consequence of this is that this function will
	// throw a RuntimeException if *k* is the *smallest* key in the Skip List.
	Key previousKey(const Key & k) const;

	// These return the value associated with the given key.
	// Throw a RuntimeException if the key does not exist.
	Value & find(const Key & k);
	const Value & find(Key k) const;

	// Return true if this key/value pair is successfully inserted, false otherwise.
	// See the project write-up for conditions under which the key should be "bubbled up"
	// to the next layer.
	// If the key already exists, do not insert one -- return false.
	bool insert(const Key & k, const Value & v);

	// Return a vector containing all inserted keys in increasing order.
	std::vector<Key> allKeysInOrder() const;

	// Is this the smallest key in the SkipList? Throw a RuntimeException
	// if the key *k* does not exist in the Skip List. 
	bool isSmallestKey(const Key & k) const;

	// Is this the largest key in the SkipList? Throw a RuntimeException
	// if the key *k* does not exist in the Skip List. 
	bool isLargestKey(const Key & k) const;

	// I am not requiring you to implement remove.

	
};

template<typename Key, typename Value>
SkipList<Key, Value>::SkipList() {
	this->head = new Node(negKey, negVal, nullptr, nullptr, nullptr, nullptr);
	this->head->up = new Node(negKey, negVal, nullptr, nullptr, nullptr, this->head);


}

template<typename Key, typename Value>
SkipList<Key, Value>::~SkipList() {

	while(this->head->down != nullptr){
		this->head = this->head->down;
	}
	while(this->head != nullptr){
		if(head != nullptr){
		}
		Node *temp = this->head->next;
		while(this->head != nullptr){
			Node *up = this->head->up;
			delete this->head;
			this->head = up;
		}
		this->head = temp;
	}
}


template<typename Key, typename Value>
size_t SkipList<Key, Value>::size() const noexcept {

	Node *temp = this->head;
	int size = 0;
	while(temp->down != nullptr){
		temp = temp->down;
	}
	while(temp->next != nullptr){
		size++;
		temp = temp->next;
	}
	return size;
}


template<typename Key, typename Value>
bool SkipList<Key, Value>::isEmpty() const noexcept {

	if(this->size() == 0){
		return true;
	}
	return false;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::numLayers() const noexcept {

	int layer = 1;
	Node *temp = this->head;
	while(temp->down != nullptr){
		temp = temp->down;
	}
	while(temp->up != nullptr){
		temp = temp->up;
		layer++;
	}
	return layer;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::height(const Key & k) const {

	unsigned height = 0;
	Node *temp = this->head->next;
	while(temp != nullptr){
		if(temp->key == k){
			break;
		}

		else if(this->isLargestKey(temp->key)){
			throw RuntimeException("key doesn't exist");
		}

		else if(temp->next->key > k){
			if(temp->down != nullptr){
				temp = temp->down;
			}
		}

		else{
			temp = temp->next;
		}
	}

	while(temp->down != nullptr){
		temp = temp->down;
	}

	while(temp != nullptr){
		temp = temp->up;
		height++;
	}
	return height;    
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::nextKey(const Key & k) const {

	Key nk = 0;
	Node *temp = this->head->next;
	while(temp != nullptr){
		if(temp->key == k){
			break;
		}

		else if(this->isLargestKey(temp->key)){
			throw RuntimeException("key doesn't exist");
		}

		else if(temp->next->key > k){
			if(temp->down != nullptr){
				temp = temp->down;
			}
		}

		else{
			temp = temp->next;
		}
	}

	while(temp->down != nullptr){
		temp = temp->down;
	}

	if(temp->next == nullptr){
		throw RuntimeException("no next key");
	}
	while(temp != nullptr){
		if(temp->key > k){
			nk = temp->key;
			break;
		}
		temp = temp->next;
	}
	return nk;
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::previousKey(const Key & k) const {
	Node *temp = this->head->next;
	if(temp->key == k){
		throw RuntimeException("no previous key");
	}
	while(temp != nullptr){
		if(temp->key == k){
			break;
		}
		
		else if(this->isLargestKey(temp->key)){
			throw RuntimeException("key doesn't exist");
		}
		
		else if(temp->next->key > k){
			if(temp->down != nullptr){
				temp = temp->down;
			}
		}

		else{
			temp = temp->next;
		}
	}

	while(temp->down != nullptr){
		temp = temp->down;
	}

	return temp->prev->key;
}

template<typename Key, typename Value>
const Value & SkipList<Key, Value>::find(Key k) const {
	Node *temp = this->head->next;
	while(temp != nullptr){
		if(temp->key == k){
			break;
		}
		else if(this->isLargestKey(temp->key)){
			throw RuntimeException("key doesn't exist");
		}
		else if(temp->next->key > k){
			if(temp->down != nullptr){
				temp = temp->down;
			}
		}

		else{
			temp = temp->next;
		}
	}

	while(temp->down != nullptr){
		temp = temp->down;
	}

	const Value &v = temp->val;
	return v;
}

template<typename Key, typename Value>
Value & SkipList<Key, Value>::find(const Key & k) {
	Node *temp = this->head->next;
	while(temp != nullptr){
		if(temp->key == k){
			break;
		}

		else if(this->isLargestKey(temp->key)){
			throw RuntimeException("key doesn't exist");
		}

		else if(temp->next->key > k){
			if(temp->down != nullptr){
				temp = temp->down;
			}
		}

		else{
			temp = temp->next;
		}
	}

	while(temp->down != nullptr){
		temp = temp->down;
	}

	return temp->val;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key & k, const Value & v) {
	Node *temp = this->head;
	Node *temp_lay = this->head;//infinity node
	while(temp_lay->up != nullptr){
		temp_lay = temp_lay->up;
	}

	
	while(temp->next != nullptr){
		if(temp->next->key == k){
			return false; // key already exists
		}

		else if(temp->next->key > k && temp->down == nullptr){
			break;
		}

		else if(temp->next->key > k){
			if(temp->down != nullptr){
				temp = temp->down;
			}
		}

		else{
			temp = temp->next;
		}
	}


	Node *ins = new Node(k, v, temp->next, temp, nullptr, nullptr); // node being inserted

	temp->next = ins;


	unsigned long size = this->size();
	int height = 1;
	if(size <= 16){
		for(int i = 0; i < 11; i++){
			if(flipCoin(k, i)){
				Node *layer = new Node(k, v, nullptr, nullptr, nullptr, ins);
				height++;
				while(temp->up == nullptr){
					temp = temp->prev;

				}
				temp->up->next = layer;
				layer->prev = temp->up;
				temp = temp->up;

				ins->up = layer; 
				ins = ins->up;
				if(height == this->numLayers()){//adding the infinity height
					Node *up = new Node(negKey, negVal, nullptr, nullptr, nullptr, temp_lay);
					temp_lay->next = layer;
					temp_lay->up = up;
					temp_lay = temp_lay->up;


				}
			}
			else{
				break;
			}
		}
		return true;

	}
	else{
		size = 3 * ceil(log2(size));
		for(int i = 0; i < size-1; i++){
			if(flipCoin(k, i)){
				Node *layer = new Node(k, v, nullptr, nullptr, nullptr, ins);
				height++;
				while(temp->up == nullptr){
					temp = temp->prev;

				}
				temp->up->next = layer;
				layer->prev = temp->up;
				temp = temp->up;

				ins->up = layer; 
				ins = ins->up;
				if(height == this->numLayers()){//adding the infinity height
					Node *up = new Node(negKey, negVal, nullptr, nullptr, nullptr, temp_lay);
					temp_lay->up = up;
					temp_lay->next = layer;
					temp_lay = temp_lay->up;//this pointer is trippin after it goes up once it doesnt go up again, pointer lost?

				}
			}
			else{
				break;
			}
		}
		return true;
}
}
template<typename Key, typename Value>
std::vector<Key> SkipList<Key, Value>::allKeysInOrder() const {

	std::vector<Key> k;
	Node *temp = this->head->next;
	while(temp->down != nullptr){
		temp = temp->down;
	}

	while(temp != nullptr){
		k.push_back(temp->key);
		temp = temp->next;
	}
    return k;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isSmallestKey(const Key & k) const {
	std::vector<Key> vk = this->allKeysInOrder();
	bool in = false;
	for(auto &v: vk){
		if(v==k){
			in = true;
		}
	}
	if(in == false){
		throw RuntimeException("key not found");
	}
	if(vk.at(1) == k){
		in = true;
	}
    return in;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isLargestKey(const Key & k) const {
	std::vector<Key> vk = this->allKeysInOrder();
	bool in = false;
	for(auto &v: vk){
		if(v==k){
			in = true;
		}
	}

	if(in == false){
		throw RuntimeException("key not found");
	}
	
	if(vk.at(vk.size()-1) == k){
		return true;
	}
    return false;
}



#endif
