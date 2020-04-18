#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0
#define MAX_NUMBER 100

//复用叶节点和内部节点  但会产生空间的浪费
typedef struct BPlusTreeNode {
	int isRoot, isLeaf;  //记录
	int keyNumbers;  //元素数目
	int key[MAX_NUMBER];  //key
	int pos[MAX_NUMBER];  //value
	struct BPlusTreeNode* child[MAX_NUMBER];  //孩子指针
	struct BPlusTreeNode* father;  //父指针
	struct BPlusTreeNode* next;  //叶节点中的前后节点指针
	struct BPlusTreeNode* last;
} BPlusTreeNode;

struct BPlusTreeNode* Root;
int MaxNumber = 5;
int TotalNodes;
int Number;

// 新建一个节点
BPlusTreeNode* New_BPlusTreeNode() {
	struct BPlusTreeNode* p = (struct BPlusTreeNode*)malloc(sizeof(struct BPlusTreeNode));
	p->isRoot = p->isLeaf = false;
	p->keyNumbers = 0;
	p->child[0] = NULL;
	p->father = NULL;
	p->next = NULL;
	p->last = NULL;
	TotalNodes++;
	return p;
}

// 节点内部搜索
inline int Binary_Search(BPlusTreeNode* Pointer, int key) {
	int l = 0, r = Pointer->keyNumbers;
	if (key < Pointer->key[l]) return l;
	if (Pointer->key[r - 1] <= key) return r - 1;
	while (l < r - 1) {
		int mid = (l + r) >> 1;
		if (Pointer->key[mid] > key)
			r = mid;
		else
			l = mid;
	}
	return l;
}

void Split(BPlusTreeNode* Pointer);
//添加一个(key,pointer)到内部节点
void InsertToInner(BPlusTreeNode* Pointer, int key, BPlusTreeNode* value) {
    //把key插入到正确位置
	int i, ins;
	if (key < Pointer->key[0]) {
        ins = 0;
    } else {
        ins = Binary_Search(Pointer, key) + 1;
    }
	for (i = Pointer->keyNumbers; i > ins; i--) {
		Pointer->key[i] = Pointer->key[i - 1];
		Pointer->child[i] = Pointer->child[i - 1];
	}
	Pointer->keyNumbers++;
	Pointer->key[ins] = key;
	Pointer->child[i] = value;
	if (Pointer->keyNumbers == MaxNumber) // 已满 分裂
		Split(Pointer);
}

//分裂叶节点
void Split(BPlusTreeNode* Pointer) {
	// 拆分Pointer节点内数据到Temp
	BPlusTreeNode* Temp = New_BPlusTreeNode();
	BPlusTreeNode* ch;
	int Mid = MaxNumber >> 1;
	Temp->isLeaf = Pointer->isLeaf; // 确保两节点深度一致
	Temp->keyNumbers = MaxNumber - Mid;  //新节点内元素数量
	for (int i = Mid; i < MaxNumber; i++) {
		Temp->child[i - Mid] = Pointer->child[i];
		Temp->key[i - Mid] = Pointer->key[i];
		if (Temp->isLeaf) {  //如果是叶节点 则存放数据
			Temp->pos[i - Mid] = Pointer->pos[i];
		} else {  //如果不是 则需要记录孩子指针
			ch = (BPlusTreeNode*)Temp->child[i - Mid];
			ch->father = Temp;
		}
	}
	// 修改Pointer元素数量
	Pointer->keyNumbers = Mid;
	//如果分裂的是根节点 则需要一个新的根节点
	if (Pointer->isRoot) {
		Root = New_BPlusTreeNode();
		Root->keyNumbers = 2;
		Root->isRoot = true;
		Root->key[0] = Pointer->key[0];
		Root->child[0] = Pointer;
		Root->key[1] = Temp->key[0];
		Root->child[1] = Temp;
		Pointer->father = Temp->father = Root;
		Pointer->isRoot = false;
		if (Pointer->isLeaf) {
			Pointer->next = Temp;
			Temp->last = Pointer;
		}
	} else {
		// 把temp添加到Pointer的父节点中
		Temp->father = Pointer->father;
		InsertToInner(Pointer->father, Pointer->key[Mid], Temp);
	}
}

//添加一个kv到叶节点中
void Insert(BPlusTreeNode* Pointer, int key, int pos) {
    //把key插入到正确位置
	int i, ins;
	if (key < Pointer->key[0]) {
        ins = 0;
    } else {
        ins = Binary_Search(Pointer, key) + 1;
    }
	for (i = Pointer->keyNumbers; i > ins; i--) {
		Pointer->key[i] = Pointer->key[i - 1];
		Pointer->pos[i] = Pointer->pos[i - 1];
	}
	Pointer->keyNumbers++;
	Pointer->key[ins] = key;
	Pointer->pos[ins] = pos;
	if (Pointer->keyNumbers == MaxNumber) // 已满 分裂
		Split(Pointer);
}

//向周围节点借数据
void Resort(BPlusTreeNode* Left, BPlusTreeNode* Right) {
	int total = Left->keyNumbers + Right->keyNumbers;
	BPlusTreeNode* temp;
	if (Left->keyNumbers < Right->keyNumbers) { //左节点数据少 需要借数据
		int leftSize = total >> 1;
		int i = 0, j = 0;
		while (Left->keyNumbers < leftSize) {
			Left->key[Left->keyNumbers] = Right->key[i];
			Left->child[Left->keyNumbers] = Right->child[i];
			if (Left->isLeaf) {
				Left->pos[Left->keyNumbers] = Right->pos[i];
			} else {
				temp = (BPlusTreeNode*)(Right->child[i]);
				temp->father = Left;
			}
			Left->keyNumbers++;
			i++;
		}
		while (i < Right->keyNumbers) {
			Right->key[j] = Right->key[i];
			Right->child[j] = Right->child[i];
			if (Right->isLeaf) Right->pos[j] = Right->pos[i];
			i++;
			j++;
		}
		Right->keyNumbers = j;
	} else {
		int leftSize = total >> 1;
		int i, move = Left->keyNumbers - leftSize, j = 0;
		for (i = Right->keyNumbers - 1; i >= 0; i--) {
			Right->key[i + move] = Right->key[i];
			Right->child[i + move] = Right->child[i];
			if (Right->isLeaf) Right->pos[i + move] = Right->pos[i];
		}
		for (i = leftSize; i < Left->keyNumbers; i++) {
			Right->key[j] = Left->key[i];
			Right->child[j] = Left->child[i];
			if (Right->isLeaf) {
				Right->pos[j] = Left->pos[i];
			} else {
				temp = (BPlusTreeNode*)Left->child[i];
				temp->father = Right;
			}
			j++;
		}
		Left->keyNumbers = leftSize;
		Right->keyNumbers = total - leftSize;
	}
}

void Delete(BPlusTreeNode* Pointer, int key);
// 重新分配Pointer
void Redistribute(BPlusTreeNode* Pointer) {
	if (Pointer->isRoot) {
		if (Pointer->keyNumbers == 1 && !Pointer->isLeaf) {
			Root = Pointer->child[0];
			Root->isRoot = true;
			free(Pointer);
		}
		return;
	}
	BPlusTreeNode* Father = Pointer->father;
	BPlusTreeNode* prevChild;
	BPlusTreeNode* succChild;
	BPlusTreeNode* temp;
	int my_index = Binary_Search(Father, Pointer->key[0]);
	if (my_index + 1 < Father->keyNumbers) { //如果不是最右面的分支
		succChild = Father->child[my_index + 1];
		if ((succChild->keyNumbers - 1) * 2 >= MaxNumber) {
			Resort(Pointer, succChild); // 可以和右节点整合
			Father->key[my_index + 1] = succChild->key[0];
			return;
		}
	}
	if (my_index - 1 >= 0) {
		prevChild = Father->child[my_index - 1];
		if ((prevChild->keyNumbers - 1) * 2 >= MaxNumber) {
			Resort(prevChild, Pointer); // 左节点
			Father->key[my_index] = Pointer->key[0];
			return;
		}
	}
	if (my_index + 1 < Father->keyNumbers) { // 和右节点合并
		int i = 0;
		while (i < succChild->keyNumbers) {
			Pointer->key[Pointer->keyNumbers] = succChild->key[i];
			Pointer->child[Pointer->keyNumbers] = succChild->child[i];
			if (Pointer->isLeaf) {
				Pointer->pos[Pointer->keyNumbers] = succChild->pos[i];
			} else {
				temp = (BPlusTreeNode*)(succChild->child[i]);
				temp->father = Pointer;
			}
			Pointer->keyNumbers++;
			i++;
		}
		Delete(Father, succChild->key[0]); // 删除右节点
		return;
	}
	if (my_index - 1 >= 0) { // 和左节点合并
		int i = 0;
		while (i < Pointer->keyNumbers) {
			prevChild->key[prevChild->keyNumbers] = Pointer->key[i];
			prevChild->child[prevChild->keyNumbers] = Pointer->child[i];
			if (Pointer->isLeaf) {
				prevChild->pos[prevChild->keyNumbers] = Pointer->pos[i];
			} else {
				temp = (BPlusTreeNode*)(Pointer->child[i]);
				temp->father = prevChild;
			}
			prevChild->keyNumbers++;
			i++;
		}
		Delete(Father, Pointer->key[0]); // delete left child
		return;
	}
}

// 删除key
void Delete(BPlusTreeNode* Pointer, int key) {
	int i, del = Binary_Search(Pointer, key);
	void* delChild = Pointer->child[del];
	for (i = del; i < Pointer->keyNumbers - 1; i++) {  //移动
		Pointer->key[i] = Pointer->key[i + 1];
		Pointer->child[i] = Pointer->child[i + 1];
		if (Pointer->isLeaf) Pointer->pos[i] = Pointer->pos[i + 1];
	}
	Pointer->keyNumbers--;
	if (Pointer->isLeaf == false) {
		BPlusTreeNode* firstChild = (BPlusTreeNode*)(Pointer->child[0]);
		if (firstChild->isLeaf == true) {
			BPlusTreeNode* temp = (BPlusTreeNode*)delChild;
			BPlusTreeNode* prevChild = temp->last;
			BPlusTreeNode* succChild = temp->next;
			if (prevChild != NULL) prevChild->next = succChild;
			if (succChild != NULL) succChild->last = prevChild;
		}
	}
	if (del == 0 && !Pointer->isRoot) {   //需要修改父节点数据
		BPlusTreeNode* temp = Pointer;
		while (!temp->isRoot && temp == temp->father->child[0]) {
			temp->father->key[0] = Pointer->key[0];
			temp = temp->father;
		}
		if (!temp->isRoot) {
			temp = temp->father;
			int i = Binary_Search(temp, key);
			temp->key[i] = Pointer->key[0];
		}
	}
	free(delChild);
	if (Pointer->keyNumbers * 2 < MaxNumber)  //需要更改节点布局
		Redistribute(Pointer);
}

//找到叶子节点
BPlusTreeNode* Find(int key) {
	BPlusTreeNode* Pointer = Root;
	while (1) {
		if (Pointer->isLeaf == true)
			break;
		int i = Binary_Search(Pointer, key);
		Pointer = Pointer->child[i];
	}
	return Pointer;
}

//销毁
void Destroy(BPlusTreeNode* Pointer) {
	if (Pointer->isLeaf == true) {
		int i;
		for (i = 0; i < Pointer->keyNumbers; i++)
			free(Pointer->child[i]);
	} else {
		int i;
		for (i = 0; i < Pointer->keyNumbers; i++)
			Destroy(Pointer->child[i]);
	}
	free(Pointer);
}

//在树中添加数据
int BPlusTree_Insert(int key, int pos) {
	BPlusTreeNode* Leaf = Find(key);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] == key) return false;  //已经存在
	Insert(Leaf, key, pos);  //添加
	return true;
}
//非唯一索引中的查询
void BPlusTree_Query_Key(int key) {
	BPlusTreeNode* Leaf = Find(key);
	Number = 0;
	int i;
	for (i = 0; i < Leaf->keyNumbers; i++) {
		if (Leaf->key[i] == key) {
			Number++;
			if (Number < 10) printf("[no.%d kv = (%d,%d)]\n", Number, Leaf->key[i], Leaf->pos[i]);
		}
	}
	printf("sum = %d\n", Number);
}
//范围查询
void BPlusTree_Query_Range(int l, int r) {
	BPlusTreeNode* Leaf = Find(l);
	Number = 0;
	int i;
	for (i = 0; i < Leaf->keyNumbers; i++) {
		if (Leaf->key[i] >= l) break;
	}
	int flag = false;
	while (!flag) {
		while (i < Leaf->keyNumbers) {
			if (Leaf->key[i] > r) {
				flag = true;
				break;
			}
			Number++;
			if (Number == 10) printf("...\n");
			if (Number < 10) printf("[no.%d kv = (%d, %d)]\n", Number, Leaf->key[i], Leaf->pos[i]);
			i++;
		}
		if (flag || Leaf->next == NULL) break;
		Leaf = Leaf->next;
		i = 0;
	}
	printf("sum = %d\n", Number);
}

//查询单个数据
int BPlusTree_Find(int key) {
	BPlusTreeNode* Leaf = Find(key);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return -1;
	return Leaf->pos[i];
}

//删除数据
void BPlusTree_Delete(int key) {
	BPlusTreeNode* Leaf = Find(key);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return;
   	Delete(Leaf, key);
}

//销毁树
void BPlusTree_Destroy() {
	if (Root == NULL) return;
	Destroy(Root);
	Root = NULL;
}

//初始化一棵树
void BPlusTree_Init() {
	BPlusTree_Destroy();
	Root = New_BPlusTreeNode();
	Root->isRoot = true;
	Root->isLeaf = true;
	TotalNodes = 0;
}

//设置为 M-way
void BPlusTree_SetMaxNumber(int number) {
	MaxNumber = number + 1;
}

int main(void){
    BPlusTree_Init();
    for(int i = 1; i <= 10; i++)
        BPlusTree_Insert(i,1);
    for(int i = 0; i < Root->keyNumbers; i++) {
        printf("key = %d\n", Root->key[i]);
    }
    return 0;
}
