#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pq.h"

/**
 * @brief Descrive un task letto dal file CSV.
 */
typedef struct {
    int id;
    int start;
    int length;
    int priority;
} Task;

/**
 * @brief Confronta due task secondo la priorita' di scheduling.
 *
 * A priorita' piu' alta corrisponde un valore maggiore; in caso di parita'
 * si preferisce il task arrivato prima e poi quello con ID minore.
 *
 * @param a Primo task.
 * @param b Secondo task.
 * @return Valore positivo, nullo o negativo come da convenzione `compare`.
 */
static int compare_task_priority(const void* a, const void* b) {
    const Task* ta = a;
    const Task* tb = b;

    if (ta->priority != tb->priority) return ta->priority - tb->priority;
    if (ta->start != tb->start) return tb->start - ta->start;
    return tb->id - ta->id;
}

/**
 * @brief Produce una hash del puntatore al task.
 *
 * @param a Puntatore al task.
 * @return Valore hash associato al puntatore.
 */
static unsigned long hash_task(const void* a) {
    return (unsigned long)(uintptr_t)a;
}

/**
 * @brief Espande il vettore dei task quando non c'e' piu' spazio disponibile.
 *
 * @param tasks Puntatore al vettore dinamico.
 * @param capacity Capacita' corrente del vettore.
 * @param count Numero di elementi gia' memorizzati.
 * @return `0` se l'operazione ha successo, `-1` in caso di errore.
 */
static int ensure_capacity(Task** tasks, size_t* capacity, size_t count) {
    Task* resized_tasks;
    size_t new_capacity;

    if (count < *capacity) {
        return 0;
    }

    new_capacity = (*capacity == 0) ? 16 : (*capacity * 2);
    resized_tasks = realloc(*tasks, new_capacity * sizeof(**tasks));
    if (!resized_tasks) {
        return -1;
    }

    *tasks = resized_tasks;
    *capacity = new_capacity;
    return 0;
}

/**
 * @brief Legge i task dal file CSV validando il formato di input.
 *
 * Il file deve contenere righe nel formato `ID,Start,Length,Priority`
 * gia' ordinate per `Start`.
 *
 * @param input File CSV di input.
 * @param tasks Vettore dinamico dei task letto dal file.
 * @param task_count Numero di task letti.
 * @return `0` se la lettura ha successo, `-1` in caso di errore.
 */
static int read_tasks(FILE* input, Task** tasks, size_t* task_count) {
    char line[256];
    size_t capacity = 0;
    size_t count = 0;
    int last_start = -1;

    *tasks = NULL;
    *task_count = 0;

    while (fgets(line, sizeof(line), input)) {
        Task task;

        if (line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        if (sscanf(line, "%d,%d,%d,%d", &task.id, &task.start, &task.length, &task.priority) != 4) {
            fprintf(stderr, "Invalid CSV row: %s", line);
            free(*tasks);
            *tasks = NULL;
            return -1;
        }

        if (task.start < last_start) {
            fprintf(stderr, "Input file is not sorted by Start\n");
            free(*tasks);
            *tasks = NULL;
            return -1;
        }

        if (task.length < 0) {
            fprintf(stderr, "Invalid task length for ID %d\n", task.id);
            free(*tasks);
            *tasks = NULL;
            return -1;
        }

        if (ensure_capacity(tasks, &capacity, count) != 0) {
            free(*tasks);
            *tasks = NULL;
            return -1;
        }

        (*tasks)[count++] = task;
        last_start = task.start;
    }

    *task_count = count;
    return 0;
}

/**
 * @brief Esegue la simulazione di scheduling e scrive il CSV di output.
 *
 * @param output File su cui scrivere i task eseguiti.
 * @param tasks Vettore dei task ordinati per tempo di arrivo.
 * @param task_count Numero di task presenti nel vettore.
 * @return `0` se la simulazione termina correttamente, `-1` in caso di errore.
 */
static int write_schedule(FILE* output, Task* tasks, size_t task_count) {
    PriorityQueue* queue;
    size_t next_task = 0;
    size_t completed = 0;
    int current_time = 0;

    queue = priority_queue_create(compare_task_priority, hash_task);
    if (!queue) {
        return -1;
    }

    while (completed < task_count) {
        while (next_task < task_count && tasks[next_task].start <= current_time) {
            if (priority_queue_push(queue, &tasks[next_task]) != 0) {
                priority_queue_free(queue);
                return -1;
            }
            next_task++;
        }

        if (priority_queue_size(queue) == 0) {
            current_time = tasks[next_task].start;
            continue;
        }

        {
            Task* task = priority_queue_top(queue);
            int start_time = current_time;
            int end_time = current_time + task->length;

            priority_queue_pop(queue);
            if (fprintf(output, "%d,%d,%d\n", task->id, start_time, end_time) < 0) {
                priority_queue_free(queue);
                return -1;
            }

            current_time = end_time;
            completed++;
        }
    }

    priority_queue_free(queue);
    return 0;
}

int main(int argc, char** argv) {
    Task* tasks = NULL;
    size_t n = 0;
    FILE* input = NULL;
    FILE* output = NULL;
    int exit_code = 1;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    input = fopen(argv[1], "r");
    output = fopen(argv[2], "w");
    if (!input || !output) {
        perror("fopen");
        if (input) fclose(input);
        if (output) fclose(output);
        return 1;
    }

    if (read_tasks(input, &tasks, &n) != 0) {
        fprintf(stderr, "Failed to read input tasks\n");
        goto cleanup;
    }

    if (write_schedule(output, tasks, n) != 0) {
        fprintf(stderr, "Failed to write output schedule\n");
        goto cleanup;
    }

    exit_code = 0;

cleanup:
    free(tasks);
    fclose(input);
    fclose(output);
    return exit_code;
}
