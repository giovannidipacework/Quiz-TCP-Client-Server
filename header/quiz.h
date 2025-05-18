#include <stdbool.h>
#include <pthread.h>

#include "theme.h"
#include "user.h"

struct quiz
{
    struct node *themes;
    struct node *users;
    struct node *results;

    pthread_mutex_t mutex_user;
    pthread_mutex_t mutex_score;
};

struct quiz *create_quiz();
struct node *load_themes(struct quiz *qz);
struct node *load_users(struct quiz *qz);
struct node *load_scores(struct quiz *qz);

int record_user(struct quiz *qz, char nickname[]);
int score_user(struct quiz *qz, struct user *u, struct theme *tm);
void record_score(struct quiz *qz, struct user *u, struct theme *tm, int points, int completed);
void remove_score(struct quiz *qz, struct user *u, struct theme *tm);
void show_score(struct quiz *qz, char *buffer);
int number_of_themes(struct quiz *qz);

struct user *get_user(struct quiz *qz, char nickname[]);
struct theme *get_theme(struct quiz *qz, int theme_id);
struct question *get_question_from_theme(struct theme *tm, int qs);

void save_file(struct quiz *qz);