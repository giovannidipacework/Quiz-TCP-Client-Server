#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../header/user.h"
#include "../header/theme.h"
#include "../header/result.h"

struct result *create_result(struct user *u, struct theme *tm, int points, int completed)
{
    struct result *r = (struct result *)malloc(sizeof(struct result));

    r->u = u;
    r->tm = tm;
    r->points = points;
    r->completed = completed;

    return r;
};

// inserisco i punteggi in ordine, prima di tema e poi di punteggio
void insert_ordered_result(struct node *list, struct result *r)
{
    struct node *node_r = list;
    struct node *punt = list;
    struct node *punt_prec = NULL;

    while (punt != NULL)
    {
        struct result *temp = (struct result *)punt->data;
        if (temp->tm->theme_id == r->tm->theme_id)
        {
            if (temp->points < r->points)
            {
                if (punt_prec == NULL)
                    // se sono nello stesso tema con punti maggiori e sono in testa inserisco in testa
                    node_r = insert_head(node_r, r, sizeof(struct result));
                else
                    // se sono nello stesso tema con punti maggiori e non sono in testa inserisco prima
                    insert_node(&punt_prec, r, sizeof(struct result));
                break;
            }
            else if (punt->next != NULL)
            {
                struct result *next_temp = (struct result *)punt->next->data;
                if (next_temp->tm->theme_id != r->tm->theme_id)
                {
                    // se sono nello stesso tema con punti minori ma il prossimo non `e del tema inserisco dopo
                    insert_node(&punt, r, sizeof(struct result));
                    break;
                }
            }
        }
        else if (punt->next == NULL)
        {
            // se sono a fine lista inserisco
            insert_node(&punt, r, sizeof(struct result));
            break;
        }

        punt_prec = punt;
        punt = punt->next;
    }
}