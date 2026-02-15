#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG 0

// Three outcomes each turn (like left/middle/right)
double prob_trap     = 0.30;  // lose HP
double prob_treasure = 0.35;  // gain gold
double prob_monster  = 0.35;  // fight monster (lose HP, gain more gold)
double prob_min = 1e-6;

int steps = 6;        // 3^6 = 729 leaves by default
int start_hp = 10;
int max_hp = 10;
int start_gold = 0;
int win_gold = 10;

void init(int argc, char *argv[]);

// State carried along each path
double prob = 1.0;
int step = 0;
int hp = 10;
int gold = 0;

// Actions
void step_trap(void) {
    prob *= prob_trap;
    step += 1;
    hp -= 2;
}

void step_treasure(void) {
    prob *= prob_treasure;
    step += 1;
    gold += 3;
    if (hp < max_hp) hp += 1; // small heal
}

void step_monster(void) {
    prob *= prob_monster;
    step += 1;
    hp -= 3;
    gold += 5; // loot if you survive
}

int main(int argc, char *argv[]) {
    // avoid duplicate buffered prints due to fork
    setvbuf(stdout, NULL, _IONBF, 0);

    init(argc, argv);

    // set start state from args
    hp = start_hp;
    gold = start_gold;

    // CSV header
    printf("prob,step,hp,gold,result,pid\n");

    pid_t trap_pid = -1;     int trap_status = 0;
    pid_t treasure_pid = -1; int treasure_status = 0;
    pid_t monster_pid = -1;  int monster_status = 0;

    while (step < steps && hp > 0 && gold < win_gold) {

        // Each iteration: fork up to 3 children. Parent waits then exits,
        // children continue deeper (same principle as professor code).
        if (prob * prob_trap > prob_min && (trap_pid = fork()) == 0) {
            step_trap();
        }
        else if (prob * prob_treasure > prob_min && (treasure_pid = fork()) == 0) {
            step_treasure();
        }
        else if (prob * prob_monster > prob_min && (monster_pid = fork()) == 0) {
            step_monster();
        }
        else {
            // Parent process (or pruned process) waits for any children it spawned
            if (trap_pid > 0) waitpid(trap_pid, &trap_status, 0);
            if (treasure_pid > 0) waitpid(treasure_pid, &treasure_status, 0);
            if (monster_pid > 0) waitpid(monster_pid, &monster_status, 0);

#if DEBUG
            fprintf(stderr, "Debug parent pid=%d waited: trap=%d treasure=%d monster=%d\n",
                    getpid(), trap_status, treasure_status, monster_status);
#endif
            return 0;
        }
    }

    const char *result;
    if (hp <= 0) result = "DEAD";
    else if (gold >= win_gold) result = "WIN";
    else result = "TIME";

    printf("%0.10g,%d,%d,%d,%s,%d\n", prob, step, hp, gold, result, getpid());
    return 0;
}

void init(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        {
            const char *arg = "--prob-trap=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { prob_trap = atof(argv[i] + len); continue; }
        }
        {
            const char *arg = "--prob-treasure=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { prob_treasure = atof(argv[i] + len); continue; }
        }
        {
            const char *arg = "--prob-monster=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { prob_monster = atof(argv[i] + len); continue; }
        }
        {
            const char *arg = "--prob-min=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { prob_min = atof(argv[i] + len); continue; }
        }
        {
            const char *arg = "--steps=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { steps = atoi(argv[i] + len); continue; }
        }
        {
            const char *arg = "--hp=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { start_hp = atoi(argv[i] + len); continue; }
        }
        {
            const char *arg = "--max-hp=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { max_hp = atoi(argv[i] + len); continue; }
        }
        {
            const char *arg = "--gold=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { start_gold = atoi(argv[i] + len); continue; }
        }
        {
            const char *arg = "--win-gold=";
            size_t len = strlen(arg);
            if (strncmp(argv[i], arg, len) == 0) { win_gold = atoi(argv[i] + len); continue; }
        }

        fprintf(stderr, "Unknown argument: %s\n", argv[i]);
        exit(EXIT_FAILURE);
    }

    // Normalize probabilities to sum to 1.0
    double sum = prob_trap + prob_treasure + prob_monster;
    if (sum <= 0.0) {
        fprintf(stderr, "Probabilities must sum to > 0\n");
        exit(EXIT_FAILURE);
    }
    prob_trap     /= sum;
    prob_treasure /= sum;
    prob_monster  /= sum;

    printf("%s --prob-trap=%g --prob-treasure=%g --prob-monster=%g --steps=%d --prob-min=%g --hp=%d --win-gold=%d\n",
           argv[0], prob_trap, prob_treasure, prob_monster, steps, prob_min, start_hp, win_gold);
}
