struct result{
    struct user *u;
    struct theme *tm;
    int points;
    int completed;
};

struct result* create_result(struct user *u, struct theme *tm, int points, int completed);
void insert_ordered_result(struct node *list, struct result *r);