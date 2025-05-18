#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../header/theme.h"
#include "../header/result.h"

// inserisce un nodo in testa alla lista
struct node *insert_head(struct node *head, void *data, size_t data_size)
{
    struct node *new_node;
    new_node = (struct node *)malloc(sizeof(struct node));

    new_node->data = malloc(data_size);
    new_node->next = head;

    memcpy(new_node->data, data, data_size);

    return new_node;
}

// inserisce un nodo in coda alla lista
struct node *insert_tail(struct node *head, void *data, size_t data_size)
{
    struct node *new_node;
    new_node = (struct node *)malloc(sizeof(struct node));

    new_node->data = malloc(data_size);
    new_node->next = NULL;

    memcpy(new_node->data, data, data_size);

    if (head == NULL)
    {
        return new_node;
    }

    struct node *current = head;

    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = new_node;

    return head;
}

// inserisco dopo il nodo node
void insert_node(struct node **node, void *data, size_t data_size)
{
    struct node *new_node;
    new_node = (struct node *)malloc(sizeof(struct node));

    new_node->data = malloc(data_size);
    new_node->next = (*node)->next;

    memcpy(new_node->data, data, data_size);

    (*node)->next = new_node;
}

// inserisce un nodo all'indice index
void *remove_node(struct node **head, int index)
{
    int i = 0;
    void *ret = NULL;
    struct node *current = *head;
    struct node *prec = NULL;

    if (index == 0)
    {
        if (*head == NULL)
        {
            return NULL;
        }

        *head = current->next;
        ret = current->data;
        free(current);
        return ret;
    }

    while (current != NULL && i != index)
    {
        prec = current;
        current = current->next;
        i++;
    }

    if (current == NULL)
    {
        return NULL;
    }

    prec->next = current->next;
    ret = current->data;
    free(current);

    return ret;
}