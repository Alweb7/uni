#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

#define BUFFER_SIZE 100


typedef struct contact Contact, *ContactPtr;

struct contact {
    char* name;
    char* surname;
    char* mobile;
    char* url;
};

char* copyString(const char* str) {
    if (!str) return NULL;

    char* copy = malloc(strlen(str) + 1);
    if (!copy) return NULL;

    strcpy(copy, str);
    return copy;
}

ContactPtr mkContact(const char* name, const char* surname, const char* mobile, const char* url) {
    if ((name == NULL) || (surname == NULL)) return NULL; 

    ContactPtr ptr = malloc(sizeof(Contact)); 
    if (!ptr) return NULL; 

    ptr->name = copyString(name);
    ptr->surname = copyString(surname);
    ptr->mobile = copyString(mobile);
    ptr->url = copyString(url);

    if (!ptr->name || !ptr->surname || !ptr->mobile || !ptr->url) {
        free(ptr->name);
        free(ptr->surname);
        free(ptr->mobile);
        free(ptr->url);
        free(ptr);
        return NULL;
    }

    return ptr; 
}


void dsContact(ContactPtr* cntptr) {
    if (cntptr && *cntptr) {
        free((*cntptr)->name);
        free((*cntptr)->surname);
        free((*cntptr)->mobile);
        free((*cntptr)->url);
        free(*cntptr);
        *cntptr = NULL; 
    }
}


void updateMobile(ContactPtr cnt, const char* newMobile) {
    if (cnt) { 
        char* copy = copyString(newMobile);
        if (copy) {
            free(cnt->mobile);
            cnt->mobile = copy;
        }
    }
}

void updateUrl(ContactPtr cnt, const char* newUrl) {
    if (cnt) { 
        char* copy = copyString(newUrl);
        if (copy) {
            free(cnt->url);
            cnt->url = copy;
        }
    }
}


const char* getName(const ContactPtr cnt) {
    if (!cnt) return NULL; 
    return cnt->name; 
}


const char* getSurname(const ContactPtr cnt) {
    if (!cnt) return NULL; 
    return cnt->surname; 
}


const char* getMobile(const ContactPtr cnt) {
    if (!cnt) return NULL; 
    return cnt->mobile; 
}


const char* getUrl(const ContactPtr cnt) {
    if (!cnt) return NULL; 
    return cnt->url; 
}


_Bool _equalsContact(const ContactPtr cnt1, const ContactPtr cnt2) {
    if (!cnt1 || !cnt2) return 0; 
    
    return strcmp(cnt1->name, cnt2->name) == 0 && strcasecmp(cnt1->surname, cnt2->surname) == 0;
}


int cmpContact(const ContactPtr cnt1, const ContactPtr cnt2) {
    if (!cnt1 || !cnt2) return 0; 

    int comparison = strcasecmp(cnt1->surname, cnt2->surname); 

    if (comparison == 0) comparison = strcasecmp(cnt1->name, cnt2->name); 

  
    if (comparison < 0) comparison = -1;
    if (comparison > 0) comparison = 1;

    return comparison; 
}


void readLine(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}


void printContact(const ContactPtr cnt) {
    if (!cnt) {
        printf("Contatto non valido.\n");
        return;
    }

    printf("\n--- Contatto ---\n");
    printf("Nome: %s\n", getName(cnt));
    printf("Cognome: %s\n", getSurname(cnt));
    printf("Telefono: %s\n", getMobile(cnt));
    printf("URL: %s\n", getUrl(cnt));
}

void printComparison(const ContactPtr cnt1, const ContactPtr cnt2) {
    int comparison = cmpContact(cnt1, cnt2);

    printf("\nConfronto tra %s %s e %s %s:\n",
           getName(cnt1), getSurname(cnt1),
           getName(cnt2), getSurname(cnt2));

    if (_equalsContact(cnt1, cnt2)) {
        printf("I due contatti hanno stesso nome e cognome.\n");
    } else {
        printf("I due contatti sono diversi.\n");
    }

    if (comparison < 0) {
        printf("Il primo contatto viene prima in ordine alfabetico.\n");
    } else if (comparison > 0) {
        printf("Il secondo contatto viene prima in ordine alfabetico.\n");
    } else {
        printf("I contatti sono equivalenti per l'ordinamento.\n");
    }
}


int main(void) {
    char name[BUFFER_SIZE];
    char surname[BUFFER_SIZE];
    char mobile[BUFFER_SIZE];
    char url[BUFFER_SIZE];
    char newMobile[BUFFER_SIZE];
    char newUrl[BUFFER_SIZE];

    printf("=== Creazione primo contatto ===\n");
    printf("Inserisci nome: ");
    readLine(name, BUFFER_SIZE);

    printf("Inserisci cognome: ");
    readLine(surname, BUFFER_SIZE);

    printf("Inserisci telefono: ");
    readLine(mobile, BUFFER_SIZE);

    printf("Inserisci URL: ");
    readLine(url, BUFFER_SIZE);

    ContactPtr contact = mkContact(name, surname, mobile, url);
    if (!contact) {
        printf("Errore nella creazione del contatto.\n");
        return 1;
    }

    printf("\nContatto creato:");
    printContact(contact);

    printf("\n=== Aggiornamento primo contatto ===\n");
    printf("Inserisci nuovo telefono: ");
    readLine(newMobile, BUFFER_SIZE);
    updateMobile(contact, newMobile);

    printf("Inserisci nuovo URL: ");
    readLine(newUrl, BUFFER_SIZE);
    updateUrl(contact, newUrl);

    printContact(contact);

    printf("\n=== Creazione secondo contatto ===\n");
    printf("Inserisci nome: ");
    readLine(name, BUFFER_SIZE);

    printf("Inserisci cognome: ");
    readLine(surname, BUFFER_SIZE);

    printf("Inserisci telefono: ");
    readLine(mobile, BUFFER_SIZE);

    printf("Inserisci URL: ");
    readLine(url, BUFFER_SIZE);

    ContactPtr secondContact = mkContact(name, surname, mobile, url);
    if (!secondContact) {
        printf("Errore nella creazione del secondo contatto.\n");
        dsContact(&contact);
        return 1;
    }

    printContact(secondContact);
    printComparison(contact, secondContact);

    dsContact(&contact);
    dsContact(&secondContact);

    printf("\nPremi invio per terminare...\n");
    getchar();

    return 0;
}
