#include <stdio.h>
#include <stdlib.h>



#include "list.h"
#include "schedule_rrp.h"
#include "task.h"
#include "CPU.h"





// add a task to the list 
void addTask(struct node** head, struct task* newTask) {
	// Criação do novo nó
	struct node* newNode = (struct node*)malloc(sizeof(struct node));
	if (newNode == NULL) {
		printf("Erro: Falha ao alocar memória para o novo nó.\n");
		return;
	}

	newNode->task = newTask;
	newNode->next = NULL;

	// Verifica se a lista está vazia
	if (*head == NULL) {
		*head = newNode;
	}
	else {
		struct node* current = *head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newNode;
	}
}

// invoke the scheduler
void schedule(struct node* head, int timeSlice) {
    // Ordena a lista de tarefas com base nas prioridades
    sortTasksByPriority(head);

    struct node* current = head;
    struct node* previous = NULL;
    int quantum = timeSlice;

    while (current != NULL) {
        if (current->task->burst > 0) {
            if (quantum > current->task->burst) {
                run(current->task, current->task->burst);
                current->task->burst = 0;  // Tarefa executada completamente
            }
            else {
                run(current->task, quantum);
                current->task->burst -= quantum;  // Reduz o burst da tarefa pelo quantum
            }
        }

        // Verifica se a tarefa está concluída
        if (current->task->burst == 0) {
            // Remove o nó atual da lista
            if (previous != NULL) {
                previous->next = current->next;
                free(current);
                current = previous->next;
            }
            else {
                // Caso seja o primeiro nó, atualiza a cabeça da lista
                head = current->next;
                free(current);
                current = head;
            }
        }
        else {
            previous = current;
            current = current->next;  // Avança para a próxima tarefa
        }

        // Verifica se há tempo restante no time slice
        if (quantum > 0) {
            // Atualiza o quantum para o próximo ciclo do loop
            quantum = timeSlice;
        }
        else {
            // O time slice expirou, passa para a próxima tarefa e reinicia o quantum
            error(current->task, timeSlice);
            quantum = timeSlice;
            previous = NULL;
        }
    }
}


void sortTasksByPriority(struct node* head) {
    if (head == NULL || (head)->next == NULL)
        return;

    struct node* sorted = NULL;
    struct node* current = head;

    while (current != NULL) {
        struct node* nextNode = current->next;

        // Insere o nó corrente no início da lista ordenada
        if (sorted == NULL || current->task->priority < sorted->task->priority) {
            current->next = sorted;
            sorted = current;
        }
        else {
            struct node* temp = sorted;
            while (temp->next != NULL && current->task->priority >= temp->next->task->priority) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }

        current = nextNode;
    }

    head = sorted;
}

void swapNodes(struct node* a, struct node* b) {
    struct task* tempTask = a->task;
    a->task = b->task;
    b->task = tempTask;
}