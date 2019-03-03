#include "Tree.h"

void Tree::getCollisions(){
	collisions.clear();
	if(nodes.count == 1){
		return;
	}
	Stack<ushort> stack(256);
	stack.push(root);
	stack.push(root);
	while(!stack.empty()){
		ushort indexA = stack.pop();
		ushort indexB = stack.pop();
		TreeNode& a = nodes[indexA];
		TreeNode& b = nodes[indexB];
		if(indexA == indexB){
			TreeNode& left = nodes[a.left];
			TreeNode& right = nodes[a.right];
			if(left.isLeaf()){
				if(right.isLeaf()){
					if(areColliding(left.aabb, right.aabb)){
						collisions.push(Pair<int>(left.data, right.data));
					}
				}else{
					TreeNode& rightLeft = nodes[right.left];
					TreeNode& rightRight = nodes[right.right];
					if(areColliding(left.aabb, rightLeft.aabb)){
						collisions.push(Pair<int>(left.data, rightLeft.data));
					}
					if(areColliding(left.aabb, rightRight.aabb)){
						collisions.push(Pair<int>(left.data, rightRight.data));
					}
					if(areColliding(rightLeft.aabb, rightRight.aabb)){
						collisions.push(Pair<int>(rightLeft.data, rightRight.data));
					}
				}
			}else{
				if(right.isLeaf()){
					TreeNode& leftLeft = nodes[left.left];
					TreeNode& leftRight = nodes[left.right];
					if(areColliding(leftLeft.aabb, right.aabb)){
						collisions.push(Pair<int>(leftLeft.data, right.data));
					}
					if(areColliding(leftRight.aabb, right.aabb)){
						collisions.push(Pair<int>(leftRight.data, right.data));
					}
					if(areColliding(leftLeft.aabb, leftRight.aabb)){
						collisions.push(Pair<int>(leftLeft.data, leftRight.data));
					}
				}else{
					stack.push(a.left);
					stack.push(a.left);
					stack.push(a.right);
					stack.push(a.right);
					if(areColliding(left.aabb, right.aabb)){
						stack.push(a.left);
						stack.push(a.right);
					}
				}
			}
		}else{
			if(a.isLeaf()){
				TreeNode& bLeft = nodes[b.left];
				TreeNode& bRight = nodes[b.right];
				if(areColliding(a.aabb, bLeft.aabb)){
					if(bLeft.isLeaf()){
						collisions.push(Pair<int>(a.data, bLeft.data));
					}else{
						stack.push(indexA);
						stack.push(b.left);
					}
				}
				if(areColliding(a.aabb, bRight.aabb)){
					if(bRight.isLeaf()){
						collisions.push(Pair<int>(a.data, bRight.data));
					}else{
						stack.push(indexA);
						stack.push(b.right);
					}
				}
			}else if(b.isLeaf()){
				TreeNode& aLeft = nodes[a.left];
				TreeNode& aRight = nodes[a.right];
				if(areColliding(aLeft.aabb, b.aabb)){
					if(aLeft.isLeaf()){
						collisions.push(Pair<int>(aLeft.data, b.data));
					}else{
						stack.push(a.left);
						stack.push(indexB);
					}
				}
				if(areColliding(aRight.aabb, b.aabb)){
					if(aRight.isLeaf()){
						collisions.push(Pair<int>(aRight.data, b.data));
					}else{
						stack.push(a.right);
						stack.push(indexB);
					}
				}
			}else{
				TreeNode& aLeft = nodes[a.left];
				TreeNode& aRight = nodes[a.right];
				TreeNode& bLeft = nodes[b.left];
				TreeNode& bRight = nodes[b.right];
				if(areColliding(aLeft.aabb, bLeft.aabb)){
					if(aLeft.isLeaf() && bLeft.isLeaf()){
						collisions.push(Pair<int>(aLeft.data, bLeft.data));
					}else{
						stack.push(a.left);
						stack.push(b.left);
					}
				}
				if(areColliding(aLeft.aabb, bRight.aabb)){
					if(aLeft.isLeaf() && bRight.isLeaf()){
						collisions.push(Pair<int>(aLeft.data, bRight.data));
					}else{
						stack.push(a.left);
						stack.push(b.right);
					}
				}
				if(areColliding(aRight.aabb, bLeft.aabb)){
					if(aRight.isLeaf() && bLeft.isLeaf()){
						collisions.push(Pair<int>(aRight.data, bLeft.data));
					}else{
						stack.push(a.right);
						stack.push(b.left);
					}
				}
				if(areColliding(aRight.aabb, bRight.aabb)){
					if(aRight.isLeaf() && bRight.isLeaf()){
						collisions.push(Pair<int>(aRight.data, bRight.data));
					}else{
						stack.push(a.right);
						stack.push(b.right);
					}
				}
			}
		}
	}
}

void Tree::addLeaf(int index){
	TreeNode* node = &nodes[index];
	node->left = null;
	node->right = null;
	node->height = 0;
	if(root == null){
		node->parent = null;
		root = index;
		return;
	}
	// find the best place to be located at
	AABB& nodeAABB = node->aabb;
	int siblingIndex = root;
	TreeNode* sibling = &nodes[siblingIndex];
	int height = 1;
	while(!sibling->isLeaf()){
		int left = sibling->left;
		int right = sibling->right;
		float newLeftArea = eat(nodes[left].aabb, nodeAABB).area();
		float newRightArea = eat(nodes[right].aabb, nodeAABB).area();
		if(sibling->height == 1){
			float nodeArea = sibling->aabb.area();
			if(nodeArea < newLeftArea && nodeArea < newRightArea){
				height = 2;
				break;
			}
		}
		float leftArea = nodes[left].aabb.area();
		float rightArea = nodes[right].aabb.area();
		if(newLeftArea-leftArea < newRightArea-rightArea){
			siblingIndex = sibling->left;
		}else{
			siblingIndex = sibling->right;
		}
		sibling = &nodes[siblingIndex];
	}
	const int newParentIndex = nodes.allocate();
	// update pointers since allocation might have moved the array
	node = &nodes[index];
	sibling = &nodes[siblingIndex];
	TreeNode* newParent = &nodes[newParentIndex];
	newParent->left = siblingIndex;
	newParent->right = index;
	newParent->height = height;
	newParent->aabb = eat(node->aabb, sibling->aabb);
	if(siblingIndex == root){
		newParent->parent = null;
		root = newParentIndex;
		node->parent = newParentIndex;
		sibling->parent = newParentIndex;
		return;
	}
	int oldParent = sibling->parent;
	newParent->parent = oldParent;
	if(siblingIndex == nodes[oldParent].left){
		nodes[oldParent].left = newParentIndex;
	}else{
		nodes[oldParent].right = newParentIndex;
	}
	node->parent = newParentIndex;
	sibling->parent = newParentIndex;
	// walk back up to the root, balance tree and correct AABBs
	for(int index = newParent->parent; index != null; index = nodes[index].parent){
		index = balance(index);
	}
}

void Tree::removeLeaf(int index){
	if(index == root){
		root = null;
		return;
	}
	int parentIndex = nodes[index].parent;
	TreeNode& parent = nodes[parentIndex];
	int siblingIndex;
	if(parent.left == index){
		siblingIndex = parent.right;
	}else{
		siblingIndex = parent.left;
	}
	if(parentIndex == root){
		nodes[siblingIndex].parent = null;
		root = siblingIndex;
		nodes.deallocate(parentIndex);
		return;
	}
	int grandParentIndex = parent.parent;
	nodes[siblingIndex].parent = grandParentIndex;
	if(nodes[grandParentIndex].left == parentIndex){
		nodes[grandParentIndex].left = siblingIndex;
	}else{
		nodes[grandParentIndex].right = siblingIndex;
	}
	nodes.deallocate(parentIndex);
	// walk back up to the root, balance tree and correct AABBs
	for(int index = grandParentIndex; index != null; index = nodes[index].parent){
		index = balance(index);
	}
}

int Tree::balance(int indexA){
	TreeNode& a = nodes[indexA];
	int indexB = a.left;
	TreeNode& b = nodes[indexB];
	int indexC = a.right;
	TreeNode& c = nodes[indexC];
	int balance = c.height - b.height;
	if(balance < -1){
		// save children of b
		int indexD = b.left;
		TreeNode& d = nodes[indexD];
		int indexE = b.right;
		TreeNode& e = nodes[indexE];
		// rotate right
		b.right = indexA;
		b.parent = a.parent;
		a.parent = indexB;
		if(indexA == root){
			root = indexB;
		}else{
			if(nodes[b.parent].left == indexA){
				nodes[b.parent].left = indexB;
			}else{
				nodes[b.parent].right = indexB;
			}
		}
		// longer child goes to b, because b is higher than a
		// shorter child goes to a, because a is lower than b
		if(d.height < e.height){
			b.left = indexE;
			a.left = indexD;
			d.parent = indexA;
			a.aabb = eat(d.aabb, c.aabb);
			b.aabb = eat(e.aabb, a.aabb);
			a.height = 1 + Math::max(d.height, c.height);
			b.height = 1 + Math::max(e.height, a.height);
		}else{
			a.left = indexE;
			e.parent = indexA;
			a.aabb = eat(e.aabb, c.aabb);
			b.aabb = eat(d.aabb, a.aabb);
			a.height = 1 + Math::max(e.height, c.height);
			b.height = 1 + Math::max(d.height, a.height);
		}
		return indexB;
	}
	if(1 < balance){
		// save children of c
		int indexD = c.left;
		TreeNode& d = nodes[indexD];
		int indexE = c.right;
		TreeNode& e = nodes[indexE];
		// rotate left
		c.left = indexA;
		c.parent = a.parent;
		a.parent = indexC;
		if(indexA == root){
			root = indexC;
		}else{
			if(nodes[c.parent].left == indexA){
				nodes[c.parent].left = indexC;
			}else{
				nodes[c.parent].right = indexC;
			}
		}
		// longer child goes to c, because it is higher
		// shorter child goes to a, because it is lower
		if(d.height > e.height){
			c.right = indexD;
			a.right = indexE;
			e.parent = indexA;
			a.aabb = eat(b.aabb, e.aabb);
			c.aabb = eat(a.aabb, d.aabb);
			a.height = 1 + Math::max(b.height, e.height);
			c.height = 1 + Math::max(a.height, d.height);
		}else{
			a.right = indexD;
			d.parent = indexA;
			a.aabb = eat(b.aabb, d.aabb);
			c.aabb = eat(a.aabb, e.aabb);
			a.height = 1 + Math::max(b.height, d.height);
			c.height = 1 + Math::max(a.height, e.height);
		}
		return indexC;
	}
	a.aabb = eat(b.aabb, c.aabb);
	a.height = 1 + Math::max(b.height, c.height);
	return indexA;
}
