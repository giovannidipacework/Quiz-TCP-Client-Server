struct node
{
    void *data;
    struct node *next;
};

struct node *insert_head(struct node *head, void *data, size_t data_size);
struct node *insert_tail(struct node *head, void *data, size_t data_size);
void insert_node(struct node **node, void *data, size_t data_size);
void *remove_head(struct node *head);
void *remove_tail(struct node *head);
void *remove_node(struct node **head, int index);