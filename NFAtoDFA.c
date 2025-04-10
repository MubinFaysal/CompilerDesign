#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES 20
#define MAX_SYMBOLS 10
#define MAX_DFA_STATES 100

typedef struct {
    int nfa_states[MAX_STATES];
    int count;
} StateSet;

int nfa[MAX_STATES][MAX_SYMBOLS][MAX_STATES];
int nfa_num_states, nfa_num_symbols;
char symbols[MAX_SYMBOLS];
int nfa_start_state;
int nfa_accept_states[MAX_STATES], nfa_num_accepts;

StateSet dfa_states[MAX_DFA_STATES];
int dfa_transitions[MAX_DFA_STATES][MAX_SYMBOLS];
int dfa_num_states = 0;
int dfa_accept_states[MAX_DFA_STATES], dfa_num_accepts = 0;

int is_state_in_set(StateSet *set, int state) {
    for (int i = 0; i < set->count; i++)
        if (set->nfa_states[i] == state) return 1;
    return 0;
}

void add_state(StateSet *set, int state) {
    if (!is_state_in_set(set, state)) {
        set->nfa_states[set->count++] = state;
    }
}

int sets_equal(StateSet *a, StateSet *b) {
    if (a->count != b->count) return 0;
    for (int i = 0; i < a->count; i++) {
        if (!is_state_in_set(b, a->nfa_states[i]))
            return 0;
    }
    return 1;
}

int find_state_set(StateSet *set_list, int size, StateSet *target) {
    for (int i = 0; i < size; i++) {
        if (sets_equal(&set_list[i], target))
            return i;
    }
    return -1;
}

void move(StateSet *src_set, int symbol_index, StateSet *result) {
    result->count = 0;
    for (int i = 0; i < src_set->count; i++) {
        int state = src_set->nfa_states[i];
        for (int j = 0; j < MAX_STATES; j++) {
            if (nfa[state][symbol_index][j]) {
                add_state(result, j);
            }
        }
    }
}

void print_set(StateSet *set) {
    printf("{");
    for (int i = 0; i < set->count; i++) {
        printf("%d", set->nfa_states[i]);
        if (i < set->count - 1) printf(",");
    }
    printf("}");
}

void convert_nfa_to_dfa() {
    StateSet start_set = {.count = 0};
    add_state(&start_set, nfa_start_state);
    dfa_states[dfa_num_states++] = start_set;

    for (int i = 0; i < dfa_num_states; i++) {
        for (int sym = 0; sym < nfa_num_symbols; sym++) {
            StateSet result = {.count = 0};
            move(&dfa_states[i], sym, &result);
            if (result.count == 0) continue;
            int existing = find_state_set(dfa_states, dfa_num_states, &result);
            if (existing == -1) {
                dfa_states[dfa_num_states] = result;
                dfa_transitions[i][sym] = dfa_num_states;
                dfa_num_states++;
            } else {
                dfa_transitions[i][sym] = existing;
            }
        }
    }

    for (int i = 0; i < dfa_num_states; i++) {
        for (int j = 0; j < dfa_states[i].count; j++) {
            for (int k = 0; k < nfa_num_accepts; k++) {
                if (dfa_states[i].nfa_states[j] == nfa_accept_states[k]) {
                    dfa_accept_states[dfa_num_accepts++] = i;
                    goto next;
                }
            }
        }
        next:;
    }
}

void print_dfa() {
    printf("\nDFA States & Transitions:\n");
    for (int i = 0; i < dfa_num_states; i++) {
        printf("DFA state %d ", i);
        print_set(&dfa_states[i]);
        printf(":\n");
        for (int j = 0; j < nfa_num_symbols; j++) {
            int target = dfa_transitions[i][j];
            if (target || i == 0)
                printf("  on '%c' -> %d\n", symbols[j], target);
        }
    }

    printf("\nDFA Accept States: ");
    for (int i = 0; i < dfa_num_accepts; i++) {
        printf("%d ", dfa_accept_states[i]);
    }
    printf("\n");
}

int main() {
    printf("Enter number of NFA states: ");
    scanf("%d", &nfa_num_states);

    printf("Enter number of input symbols: ");
    scanf("%d", &nfa_num_symbols);
    printf("Enter symbols (space-separated): ");
    for (int i = 0; i < nfa_num_symbols; i++) {
        scanf(" %c", &symbols[i]);
    }

    // Initialize transitions
    memset(nfa, 0, sizeof(nfa));

    printf("Enter transitions (format: from symbol to), -1 to end:\n");
    while (1) {
        int from, to;
        char sym;
        scanf("%d %c %d", &from, &sym, &to);
        if (from == -1 || to == -1) break;

        int sym_index = -1;
        for (int i = 0; i < nfa_num_symbols; i++) {
            if (symbols[i] == sym) {
                sym_index = i;
                break;
            }
        }

        if (sym_index != -1) {
            nfa[from][sym_index][to] = 1;
        } else {
            printf("Invalid symbol '%c'\n", sym);
        }
    }

    printf("Enter start state: ");
    scanf("%d", &nfa_start_state);

    printf("Enter number of accept states: ");
    scanf("%d", &nfa_num_accepts);
    printf("Enter accept states: ");
    for (int i = 0; i < nfa_num_accepts; i++) {
        scanf("%d", &nfa_accept_states[i]);
    }

    convert_nfa_to_dfa();
    print_dfa();

 return 0;
}
