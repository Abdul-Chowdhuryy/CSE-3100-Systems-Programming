// Linked list demo program in CSE 3100
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

// Error messages
void error_message(enum ErrorNumber errno) {
  char *messages[] = {
      "OK",
      "Memory allocation failed.",
      "Deleting a node is not supported.",
      "The number is not on the list.",
      "Sorting is not supported.",
      "Reversing is not supported.",
      "Token is too long.",
      "A number should be specified after character d, a, or p.",
      "Token is not recognized.",
      "Invalid error number."};

  if (errno < 0 || errno > ERR_END)
    errno = ERR_END;
  printf("linkedlist: %s\n", messages[errno]);
}

// Create a new node
node *new_node(int v) {
  node *p = malloc(sizeof(node));
  if (p == NULL) {
    error_message(ERR_NOMEM);
    exit(-1);
  }
  p->v = v;
  p->next = NULL;
  return p;
}

// Prepend a node
node *prepend(node *head, node *newnode) {
  newnode->next = head;
  return newnode;
}

// Find a node by value
node *find_node(node *head, int v) {
  while (head != NULL) {
    if (head->v == v)
      return head;
    head = head->next;
  }
  return head;
}

// Find the last node
node *find_last(node *head) {
  if (head != NULL) {
    while (head->next != NULL)
      head = head->next;
  }
  return head;
}

// Append a node
node *append(node *head, node *newnode) {
  node *p = find_last(head);
  newnode->next = NULL;
  if (p == NULL)
    return newnode;
  p->next = newnode;
  return head;
}

// Delete the entire list
node *delete_list(node *head) {
  while (head != NULL) {
    node *p = head->next;
    free(head);
    head = p;
  }
  return head;
}

// Print the list
void print_list(node *head) {
  printf("[");
  while (head) {
    printf("%d, ", head->v);
    head = head->next;
  }
  printf("]\n");
}

// Print a single node
void print_node(node *p) {
  printf("%p: v=%-5d next=%p\n", p, p->v, p->next);
}

// Print detailed list information
void print_list_details(node *head) {
  while (head) {
    print_node(head);
    head = head->next;
  }
}

// Delete a node by value
node *delete_node(node *head, int v) {
  node *current = head;
  node *previous = NULL;

  while (current != NULL) {
    if (current->v == v) {
      if (previous == NULL) {
        head = current->next;
      } else {
        previous->next = current->next;
      }
      free(current);
      return head;
    }
    previous = current;
    current = current->next;
  }
  error_message(ERR_NOTFOUND);
  return head;
}

// Reverse the list
node *reverse_list(node *head) {
  node *prev = NULL;
  node *current = head;
  node *next = NULL;

  while (current != NULL) {
    next = current->next;
    current->next = prev;
    prev = current;
    current = next;
  }
  return prev;
}
