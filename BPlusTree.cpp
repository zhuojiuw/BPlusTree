#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0
#define MAX_NUMBER 100

//����Ҷ�ڵ���ڲ��ڵ�  ��������ռ���˷�
typedef struct BPlusTreeNode {
	int isRoot, isLeaf;  //��¼
	int keyNumbers;  //Ԫ����Ŀ
	int key[MAX_NUMBER];  //key
	int pos[MAX_NUMBER];  //value
	struct BPlusTreeNode* child[MAX_NUMBER];  //����ָ��
	struct BPlusTreeNode* father;  //��ָ��
	struct BPlusTreeNode* next;  //Ҷ�ڵ��е�ǰ��ڵ�ָ��
	struct BPlusTreeNode* last;
} BPlusTreeNode;

struct BPlusTreeNode* Root;
int MaxNumber = 5;
int TotalNodes;
int Number;

// �½�һ���ڵ�
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

// �ڵ��ڲ�����
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
//���һ��(key,pointer)���ڲ��ڵ�
void InsertToInner(BPlusTreeNode* Pointer, int key, BPlusTreeNode* value) {
    //��key���뵽��ȷλ��
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
	if (Pointer->keyNumbers == MaxNumber) // ���� ����
		Split(Pointer);
}

//����Ҷ�ڵ�
void Split(BPlusTreeNode* Pointer) {
	// ���Pointer�ڵ������ݵ�Temp
	BPlusTreeNode* Temp = New_BPlusTreeNode();
	BPlusTreeNode* ch;
	int Mid = MaxNumber >> 1;
	Temp->isLeaf = Pointer->isLeaf; // ȷ�����ڵ����һ��
	Temp->keyNumbers = MaxNumber - Mid;  //�½ڵ���Ԫ������
	for (int i = Mid; i < MaxNumber; i++) {
		Temp->child[i - Mid] = Pointer->child[i];
		Temp->key[i - Mid] = Pointer->key[i];
		if (Temp->isLeaf) {  //�����Ҷ�ڵ� ��������
			Temp->pos[i - Mid] = Pointer->pos[i];
		} else {  //������� ����Ҫ��¼����ָ��
			ch = (BPlusTreeNode*)Temp->child[i - Mid];
			ch->father = Temp;
		}
	}
	// �޸�PointerԪ������
	Pointer->keyNumbers = Mid;
	//������ѵ��Ǹ��ڵ� ����Ҫһ���µĸ��ڵ�
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
		// ��temp��ӵ�Pointer�ĸ��ڵ���
		Temp->father = Pointer->father;
		InsertToInner(Pointer->father, Pointer->key[Mid], Temp);
	}
}

//���һ��kv��Ҷ�ڵ���
void Insert(BPlusTreeNode* Pointer, int key, int pos) {
    //��key���뵽��ȷλ��
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
	if (Pointer->keyNumbers == MaxNumber) // ���� ����
		Split(Pointer);
}

//����Χ�ڵ������
void Resort(BPlusTreeNode* Left, BPlusTreeNode* Right) {
	int total = Left->keyNumbers + Right->keyNumbers;
	BPlusTreeNode* temp;
	if (Left->keyNumbers < Right->keyNumbers) { //��ڵ������� ��Ҫ������
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
// ���·���Pointer
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
	if (my_index + 1 < Father->keyNumbers) { //�������������ķ�֧
		succChild = Father->child[my_index + 1];
		if ((succChild->keyNumbers - 1) * 2 >= MaxNumber) {
			Resort(Pointer, succChild); // ���Ժ��ҽڵ�����
			Father->key[my_index + 1] = succChild->key[0];
			return;
		}
	}
	if (my_index - 1 >= 0) {
		prevChild = Father->child[my_index - 1];
		if ((prevChild->keyNumbers - 1) * 2 >= MaxNumber) {
			Resort(prevChild, Pointer); // ��ڵ�
			Father->key[my_index] = Pointer->key[0];
			return;
		}
	}
	if (my_index + 1 < Father->keyNumbers) { // ���ҽڵ�ϲ�
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
		Delete(Father, succChild->key[0]); // ɾ���ҽڵ�
		return;
	}
	if (my_index - 1 >= 0) { // ����ڵ�ϲ�
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

// ɾ��key
void Delete(BPlusTreeNode* Pointer, int key) {
	int i, del = Binary_Search(Pointer, key);
	void* delChild = Pointer->child[del];
	for (i = del; i < Pointer->keyNumbers - 1; i++) {  //�ƶ�
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
	if (del == 0 && !Pointer->isRoot) {   //��Ҫ�޸ĸ��ڵ�����
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
	if (Pointer->keyNumbers * 2 < MaxNumber)  //��Ҫ���Ľڵ㲼��
		Redistribute(Pointer);
}

//�ҵ�Ҷ�ӽڵ�
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

//����
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

//�������������
int BPlusTree_Insert(int key, int pos) {
	BPlusTreeNode* Leaf = Find(key);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] == key) return false;  //�Ѿ�����
	Insert(Leaf, key, pos);  //���
	return true;
}
//��Ψһ�����еĲ�ѯ
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
//��Χ��ѯ
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

//��ѯ��������
int BPlusTree_Find(int key) {
	BPlusTreeNode* Leaf = Find(key);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return -1;
	return Leaf->pos[i];
}

//ɾ������
void BPlusTree_Delete(int key) {
	BPlusTreeNode* Leaf = Find(key);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return;
   	Delete(Leaf, key);
}

//������
void BPlusTree_Destroy() {
	if (Root == NULL) return;
	Destroy(Root);
	Root = NULL;
}

//��ʼ��һ����
void BPlusTree_Init() {
	BPlusTree_Destroy();
	Root = New_BPlusTreeNode();
	Root->isRoot = true;
	Root->isLeaf = true;
	TotalNodes = 0;
}

//����Ϊ M-way
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
