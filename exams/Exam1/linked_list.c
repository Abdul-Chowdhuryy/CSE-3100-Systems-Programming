#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"





node *create_node(int v) {
    node *p = malloc(sizeof(node)); // Allocating Memory
    assert(p != NULL); 
    
  
    p->v = v;
    p->next = NULL;
    return p; 
}


void insert(node **head, node *newnode) {
    if (*head == NULL || (*head)->v >= newnode->v) {
        newnode->next = *head;
        *head = newnode;
        return;
    }

    node *p = *head;
    while (p->next != NULL && p->next->v < newnode->v) {
        p = p->next;
    }

    newnode->next = p->next;
    p->next = newnode;
}


void insert_last(node **head, node **tail, node *newnode) {
    if (*head == NULL) {
        *head = *tail = newnode;
    } else {
        (*tail)->next = newnode;
        *tail = newnode;
    }
}


node *remove_first(node **head) {
    node *p = (*head);
    if (*head != NULL) {
        *head = (*head)->next;
    }
    return p;
}


void print_list(node *head) {
    node *p = (head;
    while (p != NULL) {
        printf("%d ", p->v);
        p = p->next;
    }
    printf("\n");
}


node *merge(node **p1, node **p2) {
    node *head = NULL, *tail = NULL;
    node *q1 = *p1, *q2 = *p2;

    while (q1 != NULL && q2 != NULL) {
        node *temp;
        if (q1->v <= q2->v) {
            temp = q1;
            q1 = q1->next;
        } else {
            temp = q2;
            q2 = q2->next;
        }

        if (head == NULL) {
            head = tail = temp;
        } else {
            tail->next = temp;
            tail = temp;
        }
    }

    while (q1 != NULL) {
        if (head == NULL) {
            head = tail = q1;
        } else {
            tail->next = q1;
            tail = q1;
        }
        q1 = q1->next;
    }

    while (q2 != NULL) {
        if (head == NULL) {
            head = tail = q2;
        } else {
            tail->next = q2;
            tail = q2;
        }
        q2 = q2->next;
    }

    *p1 = NULL;
    *p2 = NULL;

    return head;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int n = atoi(argv);

    int a[n];
    int b[n];

    for (int i = 0; i < n; i++) {
        a[i] = i;
        b[i] = 2 * i;
    }

    node *p1 = NULL, *p2 = NULL;

    for (int i = 0; i < n; i++) {
        node *p = create_node(a[i]);
        insert(&p1, p);
    }
    print_list(p1);

    for (int i = 0; i < n; i++) {
        node *p = create_node(b[i]);
        insert(&p2, p);
    }
    print_list(p2);

    node *p = merge(&p1, &p2);
    print_list(p);
    printf("%p\n", p1);
    printf("%p\n", p2);

    return 0;
}
