#include "rbtree.h"
#include <stdlib.h>

// RB트리생성
rbtree *new_rbtree(void)
{
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    // empty상태엔 nil 하나 있음 => 그 nil도 자식을 가지고 있다.
    node_t *NIL = (node_t *)calloc(1, sizeof(node_t));
    p->nil = NIL;
    p->root = NIL;
    NIL->color = RBTREE_BLACK;
    return p;
}

// delete_rbtree(후위순회 하면서 하나씩 free)
void rb_postorder_delete(node_t *root, rbtree *t)
{
    if (root == t->nil)
        return;
    rb_postorder_delete(root->left, t);
    rb_postorder_delete(root->right, t);
    free(root);
}

void delete_rbtree(rbtree *t)
{
    if (t == NULL)
        return;
    rb_postorder_delete(t->root, t);
    free(t->nil);
    free(t);
}

// 왼쪽 회전함수
void left_rotate(rbtree *t, node_t *x)
{
    // y는 x의 오른쪽 자식
    node_t *y = x->right;
    // ß 옮기기 - y의 왼쪽 자식(ß)이 x의 오른쪽 자식이됨
    x->right = y->left;
    // ß의 부모는 x다
    if (y->left != t->nil)
        y->left->parent = x;
    // y의 부모는 x의 전부모다
    y->parent = x->parent;
    // x의 부모가 nil이면 Tree는 빈상태이므로 y를 트리의 루트노드에 넣는다
    if (x->parent == t->nil)
        t->root = y;
    // x가 부모의 왼쪽 자식이었으면 y도 부모의 왼쪽자식(x자리에 y넣기)
    else if (x == x->parent->left)
        x->parent->left = y;
    // x가 부모의 오른쪽 자식이었으면 y도 부모의 오른쪽자식(x자리에 y넣기)
    else
        x->parent->right = y;
    // y의 왼쪽 자식은 x다
    y->left = x;
    // x의 부모는 y다
    x->parent = y;
}

// 오른쪽 회전함수
void right_rotate(rbtree *t, node_t *x)
{
    // y는 x의 오른쪽 자식
    node_t *y = x->left;
    // ß 옮기기 - y의 왼쪽 자식(ß)이 x의 오른쪽 자식이됨
    x->left = y->right;
    // ß의 부모는 x다
    if (y->right != t->nil)
        y->right->parent = x;
    // y의 부모는 x의 전부모다
    y->parent = x->parent;
    // x의 부모가 nil이면 Tree는 빈상태이므로 y를 트리의 루트노드에 넣는다
    if (x->parent == t->nil)
        t->root = y;
    // x가 부모의 왼쪽 자식이었으면 y도 부모의 왼쪽자식(x자리에 y넣기)
    else if (x == x->parent->right)
        x->parent->right = y;
    // x가 부모의 오른쪽 자식이었으면 y도 부모의 오른쪽자식(x자리에 y넣기)
    else
        x->parent->left = y;
    // y의 왼쪽 자식은 x다
    y->right = x;
    // x의 부모는 y다
    x->parent = y;
}

void insert_fixup(rbtree *t, node_t *z)
{
    // 
    while (z->parent->color == RBTREE_RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            node_t *y = z->parent->parent->right;
            // 필기자료 3번 경우
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t, z->parent->parent);
            }
        }
        else
        {
            node_t *y = z->parent->parent->left;
            // 필기자료 3번 경우의 대칭
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}

// 삽입
node_t *rbtree_insert(rbtree *t, const key_t key)
{
    node_t *y = t->nil;
    node_t *x = t->root;
    while (x != t->nil)
    {
        y = x;
        if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    // 새로들어갈 노드(new_node) 할당
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->key = key;
    new_node->parent = y;
    if (y == t->nil)
        t->root = new_node;
    else if (new_node->key < y->key)
        y->left = new_node;
    else
        y->right = new_node;
    // new_node초기화
    new_node->left = t->nil;
    new_node->right = t->nil;
    new_node->color = RBTREE_RED;
    insert_fixup(t, new_node);
    return t->root;
}

// 값찾기
node_t *rbtree_find(const rbtree *t, const key_t key)
{
    node_t *r = t->root;
    while (r != t->nil)
    {
        if (r->key == key)
            return r;
        else if (r->key < key)
            r = r->right;
        else
            r = r->left;
    }
    return NULL;
}

// 최소값 찾기
node_t *rbtree_min(const rbtree *t)
{
    // TODO: implement find
    node_t *r = t->root;
    if (r == t->nil)
        return r;
    while (r->left != t->nil)
    {
        r = r->left;
    }
    return r;
}

// 최대값 찾기
node_t *rbtree_max(const rbtree *t)
{
    node_t *r = t->root;
    if (r == t->nil)
        return r;
    while (r->right != t->nil)
    {
        r = r->right;
    }
    return r;
}

// u자리에 v옮기기
void transplant(rbtree *t, node_t *u, node_t *v)
{
    // 루트없는 트리일때
    if (u->parent == t->nil)
        t->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

void delete_fixup(rbtree *t, node_t *extra_black)
{
    // extra_black이 루트 노드인경우는 while문을 돌 필요가없다.
    while (extra_black != t->root && extra_black->color == RBTREE_BLACK)
    {
        // x가 왼쪽자식(형제는 오른쪽)
        if (extra_black == extra_black->parent->left)
        {
            node_t *bro = extra_black->parent->right;
            // case 1 => 형제가 red인 경우
            if (bro->color == RBTREE_RED)
            {
                // 형제를 black으로 만들고
                bro->color = RBTREE_BLACK;
                // 부모를 red로 만들고
                extra_black->parent->color = RBTREE_RED;
                // 왼쪽 회전
                left_rotate(t, extra_black->parent);
                // 형제노드 바꾸기 => x는 그대로 extra black => case 2,3,4 로만들어서 해결한다.
                bro = extra_black->parent->right;
            }
            // case 2 => 형제의 자녀모두 black 인경우
            if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK)
            {
                // 형제를 red로 만들고
                bro->color = RBTREE_RED;
                // extra black을 부모로만든다(부모노드를 기준으로 case를 다신 나눈다) => case 1,2,3,4 만들어서 해결한다.
                extra_black = extra_black->parent;
            }
            else
            {
                // case 3 => 형제의 왼쪽 자녀는 red, 오른쪽 자녀는 black
                if (bro->right->color == RBTREE_BLACK)
                {
                    // 형제의 왼쪽 자녀는 black
                    bro->left->color = RBTREE_BLACK;
                    // 형제는 red
                    bro->color = RBTREE_RED;
                    // 형제 기준 오른쪽 회전
                    right_rotate(t, bro);
                    // 형제노드 바꾸기
                    bro = extra_black->parent->right;
                    // 이과정을 거치면 case 4번이 만들어진다.
                }
                // case 4 => 오른쪽 형제가 black, 형제의 오른쪽 자녀가 red
                // 오른쪽 형제는 부모의색으로
                bro->color = extra_black->parent->color;
                // 부모는 black
                extra_black->parent->color = RBTREE_BLACK;
                // 형제의 오른쪽 자녀는 black
                bro->right->color = RBTREE_BLACK;
                // 왼쪽 회전
                left_rotate(t, extra_black->parent);
                // case 4를 거치면 RB트리가 완성된다 반복문 탈출!
                break;
            }
        }
        // extra_black이 오른쪽 자식(형제는 왼쪽) => 왼쪽 자식일때으 소스코드에서 left를 right로 right를 left로
        else
        {
            node_t *bro = extra_black->parent->left;
            if (bro->color == RBTREE_RED)
            {
                bro->color = RBTREE_BLACK;
                extra_black->parent->color = RBTREE_RED;
                right_rotate(t, extra_black->parent);
                bro = extra_black->parent->left;
            }
            if (bro->right->color == RBTREE_BLACK && bro->left->color == RBTREE_BLACK)
            {
                bro->color = RBTREE_RED;
                extra_black = extra_black->parent;
            }
            else
            {
                if (bro->left->color == RBTREE_BLACK)
                {
                    bro->right->color = RBTREE_BLACK;
                    bro->color = RBTREE_RED;
                    left_rotate(t, bro);
                    bro = extra_black->parent->left;
                }
                bro->color = extra_black->parent->color;
                extra_black->parent->color = RBTREE_BLACK;
                bro->left->color = RBTREE_BLACK;
                right_rotate(t, extra_black->parent);
                break;
            }
        }
    }
    // extra_black은 root 노드이므로 black
    extra_black->color = RBTREE_BLACK;
}

node_t *tree_minimum(const rbtree *t, node_t *z_right)
{
    node_t *r = z_right;
    if (r == t->nil)
        return r;
    while (r->left != t->nil)
    {
        r = r->left;
    }
    return r;
}

// 노드삭제하기(transplant, tree-minimum, fixup)
int rbtree_erase(rbtree *t, node_t *z)
{
    node_t *successor = z;
    node_t *x;
    color_t successor_original_color = successor->color;
    // z의 오른쪽에만 자녀가 있는경우
    if (z->left == t->nil)
    {
        x = z->right;
        transplant(t, z, z->right);
    }
    // z의 왼쪽에만 자녀가 있는경우
    else if (z->right == t->nil)
    {
        x = z->left;
        transplant(t, z, z->left);
    }
    // z의 양쪽에 자녀가 있는경우
    else
    {
        // successor는 z의 다음번째수 => successor보다 큰것들중 가장 작은수
        successor = tree_minimum(t, z->right);
        successor_original_color = successor->color;
        // x는 successor의 오른쪽 자녀
        x = successor->right;
        // successor가 삭제하려는 노드의 자식이면
        if (successor->parent == z)
        {
            x->parent = successor;
        }
        // 
        else
        {
            // successor가 이동하기전에 successor의 오른쪽을 successor로 옮겨야함
            transplant(t, successor, successor->right);
            successor->right = z->right;
            successor->right->parent = successor;
        }
        // successor를 z로 옮긴다.
        transplant(t, z, successor);
        successor->left = z->left;
        successor->left->parent = successor;
        successor->color = z->color;
    }
    // successor_original_color가 블랙이면 2, 4, 5조건을 위반할 수 있음
    if (successor_original_color == RBTREE_BLACK)
        delete_fixup(t, x);
    // successor_original_color가 RED면 그냥 삭제 해도됨
    free(z);
    return 0;
}

int rb_inorder(node_t *root, key_t *res, const rbtree *t, int i, const size_t n)
{
    if (root == t->nil || i >= n)
        return i;
    i = rb_inorder(root->left, res, t, i, n);
    res[i] = root->key;
    i++;
    i = rb_inorder(root->right, res, t, i, n);
    return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    if (t->root == t->nil)
        return -1;
    rb_inorder(t->root, arr, t, 0, n);
    return 0;
}