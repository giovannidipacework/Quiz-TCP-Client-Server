#include "list.h"
#include "config.h"

struct theme
{
    char title[MAX_THEME_NAME_SIZE];
    int theme_id;
    struct node *questions;
};

struct theme *create_theme(char *title, int theme_id);
struct node *load_question(struct theme *tm);
struct question *get_question(struct theme *tm, int question_id);