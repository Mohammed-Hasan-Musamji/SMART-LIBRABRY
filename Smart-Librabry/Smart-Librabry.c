/*
  smart_library_full.c
  SmartLibrary - No arrays, No <string.h>, All linked structures (Books, Queue, Stack, IssuedRecords)
  Features:
   - Book catalog (singly linked list) using pointer typedef (Book)
   - Issue request queue (linked queue)
   - Activity history stack (linked stack)
   - Issued students record (linked list) to display students who took books
   - Manual string helpers (no string.h)
*/

#include <stdio.h>
#include <stdlib.h>

/* ---------------------- Manual string helpers (no <string.h>) ---------------------- */

/* Remove trailing newline added by fgets */
void remove_newline(char *s)
{
    if (!s)
        return;
    for (int i = 0; s[i] != '\0'; ++i)
    {
        if (s[i] == '\n')
        {
            s[i] = '\0';
            return;
        }
    }
}

/* Read a line into a dynamically allocated buffer (max bytes).
   Caller must free the returned pointer when no longer needed. */
char *read_string(int max_bytes)
{
    if (max_bytes <= 0)
        return NULL;
    char *buf = (char *)malloc((size_t)max_bytes);
    if (!buf)
        return NULL;
    if (fgets(buf, max_bytes, stdin) == NULL)
    {
        buf[0] = '\0';
    }
    else
    {
        remove_newline(buf);
    }
    return buf;
}

/* ---------------------- BOOK CATALOG (LINKED LIST) ---------------------- */

typedef struct BookStruct
{
    int book_id;
    char *title; /* dynamically allocated */
    int available_copies;
    struct BookStruct *next;
} *Book;

// typedef BookStruct *Book;

Book head = NULL;

void insert_book(int id, char *title, int copies)
{
    Book b = malloc(sizeof(struct BookStruct));
    if (!b)
    {
        printf("Memory allocation failed for Book.\n");
        if (title)
            free(title);
        return;
    }
    b->book_id = id;
    b->title = title; /* title expected to be malloc'd by caller */
    b->available_copies = copies;
    b->next = NULL;

    if (!head)
        head = b;
    else
    {
        Book cur = head;
        while (cur->next)
            cur = cur->next;
        cur->next = b;
    }
}

Book search_book(int id)
{
    Book cur = head;
    while (cur)
    {
        if (cur->book_id == id)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void display_books()
{
    printf("\n===================== BOOK CATALOG =====================\n");

    if (!head)
    {
        printf("No books available.\n");
        return;
    }

    printf("+---------+-------------------------------+----------+\n");
    printf("| Book ID | Title                         | Copies   |\n");
    printf("+---------+-------------------------------+----------+\n");

    Book cur = head;
    while (cur)
    {
        printf("| %-7d | %-29s | %-8d |\n",
               cur->book_id,
               cur->title ? cur->title : "(no title)",
               cur->available_copies);
        cur = cur->next;
    }

    printf("+---------+-------------------------------+----------+\n");
}

/* ---------------------- ISSUE REQUEST QUEUE (LINKED) ---------------------- */

typedef struct ReqNode
{
    char *student_name; /* malloc'd */
    int book_id;
    struct ReqNode *next;
} ReqNode;

ReqNode *q_front = NULL;
ReqNode *q_rear = NULL;

void enqueue_request(char *name, int book_id)
{
    ReqNode *node = (ReqNode *)malloc(sizeof(ReqNode));
    if (!node)
    {
        printf("Memory allocation failed for request. Dropping request.\n");
        if (name)
            free(name);
        return;
    }
    node->student_name = name; /* name is malloc'd by caller */
    node->book_id = book_id;
    node->next = NULL;

    if (!q_front)
    {
        q_front = q_rear = node;
    }
    else
    {
        q_rear->next = node;
        q_rear = node;
    }
}

ReqNode *dequeue_request_node()
{
    if (!q_front)
        return NULL;
    ReqNode *tmp = q_front;
    q_front = q_front->next;
    if (!q_front)
        q_rear = NULL;
    tmp->next = NULL; /* detach */
    return tmp;       /* caller responsible to free node (and possibly student_name) */
}

void display_queue()
{
    printf("\n================= ISSUE REQUEST QUEUE =================\n");

    if (!q_front)
    {
        printf("No pending requests.\n");
        return;
    }

    printf("+--------+---------------------------+---------+\n");
    printf("|  No.   | Student Name              | Book ID |\n");
    printf("+--------+---------------------------+---------+\n");

    ReqNode *cur = q_front;
    int i = 1;
    while (cur)
    {
        printf("| %-6d | %-25s | %-7d |\n",
               i,
               cur->student_name ? cur->student_name : "(no name)",
               cur->book_id);
        cur = cur->next;
        i++;
    }

    printf("+--------+---------------------------+---------+\n");
}

/* ---------------------- ACTIVITY STACK (LINKED) ---------------------- */

typedef struct StackNode
{
    char *action; /* malloc'd string like "ADD_BOOK 101" */
    struct StackNode *next;
} StackNode;

StackNode *stack_top = NULL;

void push_action(char *act)
{
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    if (!node)
    {
        printf("Memory allocation failed for action. Dropping action.\n");
        if (act)
            free(act);
        return;
    }
    node->action = act; /* act is malloc'd by caller */
    node->next = stack_top;
    stack_top = node;
}

StackNode *pop_action_node()
{
    if (!stack_top)
        return NULL;
    StackNode *tmp = stack_top;
    stack_top = stack_top->next;
    tmp->next = NULL;
    return tmp; /* caller should free node and action string */
}

void display_stack()
{
    printf("\n==================== ACTIVITY STACK ====================\n");

    if (!stack_top)
    {
        printf("No actions logged.\n");
        return;
    }

    printf("+--------+-------------------------------+\n");
    printf("| Index  | Action                        |\n");
    printf("+--------+-------------------------------+\n");

    StackNode *cur = stack_top;
    int i = 1;

    while (cur)
    {
        printf("| %-6d | %-29s |\n",
               i,
               cur->action ? cur->action : "(no action)");
        cur = cur->next;
        i++;
    }

    printf("+--------+-------------------------------+\n");
}

/* Pop K actions and print them (undo view only) */
void undo_k_actions(int k)
{
    if (k <= 0)
    {
        printf("K must be > 0.\n");
        return;
    }
    printf("\nPopping %d action(s):\n", k);
    for (int i = 0; i < k; ++i)
    {
        StackNode *p = pop_action_node();
        if (!p)
        {
            printf("No more actions to pop.\n");
            return;
        }
        printf("Popped: %s\n", (p->action ? p->action : "(no action)"));
        if (p->action)
            free(p->action);
        free(p);
    }
}

/* ---------------------- ISSUED RECORDS (LINKED) ---------------------- */

typedef struct IssueRecord
{
    char *student_name; /* malloc'd pointer reused from request (do NOT free again) */
    int book_id;
    struct IssueRecord *next;
} IssueRecord;

IssueRecord *issued_head = NULL;

void add_issue_record(char *student_name, int book_id)
{
    IssueRecord *node = (IssueRecord *)malloc(sizeof(IssueRecord));
    if (!node)
    {
        printf("Memory allocation failed for issue record.\n");
        return;
    }
    node->student_name = student_name; /* take ownership of student_name pointer */
    node->book_id = book_id;
    node->next = issued_head;
    issued_head = node;
}

void display_issued_students()
{
    printf("\n============= STUDENTS WHO TOOK BOOKS =============\n");

    if (!issued_head)
    {
        printf("No books issued yet.\n");
        return;
    }

    printf("+--------+---------------------------+---------+\n");
    printf("|  No.   | Student Name              | Book ID |\n");
    printf("+--------+---------------------------+---------+\n");

    IssueRecord *cur = issued_head;
    int i = 1;
    while (cur)
    {
        printf("| %-6d | %-25s | %-7d |\n",
               i,
               cur->student_name ? cur->student_name : "(no name)",
               cur->book_id);
        cur = cur->next;
        i++;
    }

    printf("+--------+---------------------------+---------+\n");
}

/* ---------------------- MENU OPERATIONS ---------------------- */

void menu_add_book()
{
    int id = 0, copies = 0;
    printf("Enter Book ID (integer): ");
    if (scanf("%d", &id) != 1)
    {
        printf("Invalid input.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; /* flush newline */

    printf("Enter Title: ");
    char *title = read_string(256); /* malloc'd */

    printf("Enter available copies (integer): ");
    if (scanf("%d", &copies) != 1)
    {
        printf("Invalid input.\n");
        if (title)
            free(title);
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ;

    insert_book(id, title, copies);

    /* log action */
    char *act = (char *)malloc(64);
    if (act)
    {
        sprintf(act, "ADD_BOOK %d", id);
        push_action(act);
    }
    else
    {
        printf("Warning: cannot log action due to memory.\n");
    }

    printf("Book added: ID=%d Title=%s Copies=%d\n", id, (title ? title : "(no title)"), copies);
}

void menu_search_book()
{
    int id = 0;
    printf("Enter Book ID to search: ");
    if (scanf("%d", &id) != 1)
    {
        printf("Invalid input.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ;

    Book b = search_book(id);
    if (b)
    {
        printf("Book Found -> ID=%d Title=%s Copies=%d\n", b->book_id, (b->title ? b->title : "(no title)"), b->available_copies);
    }
    else
    {
        printf("Book with ID %d not found.\n", id);
    }
}

void menu_request_issue()
{
    /* read student name */
    while (getchar() != '\n')
        ; /* ensure input buffer clear before fgets */
    printf("Enter Student Name: ");
    char *name = read_string(128); /* malloc'd */

    printf("Enter Book ID to request: ");
    int id = 0;
    if (scanf("%d", &id) != 1)
    {
        printf("Invalid input.\n");
        if (name)
            free(name);
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ;

    enqueue_request(name, id);

    /* log action */
    char *act = (char *)malloc(64);
    if (act)
    {
        sprintf(act, "ISSUE_REQUEST %d", id);
        push_action(act);
    }

    printf("Request enqueued: Student=%s BookID=%d\n", (name ? name : "(no name)"), id);
}

void menu_process_next_request()
{
    ReqNode *req = dequeue_request_node();
    if (!req)
    {
        printf("No pending requests to process.\n");
        return;
    }

    printf("Processing request: Student=%s BookID=%d\n", (req->student_name ? req->student_name : "(no name)"), req->book_id);
    Book b = search_book(req->book_id);

    if (b && b->available_copies > 0)
    {
        /* Issue success */
        b->available_copies -= 1;

        /* log action */
        char *act = (char *)malloc(64);
        if (act)
        {
            sprintf(act, "ISSUE_SUCCESS %d", req->book_id);
            push_action(act);
        }

        /* record issued student: take ownership of student_name pointer (do NOT free it here) */
        add_issue_record(req->student_name, req->book_id);

        printf("Issued book '%s' (ID %d) to %s. Remaining copies: %d\n",
               (b->title ? b->title : "(no title)"),
               b->book_id,
               (req->student_name ? req->student_name : "(no name)"),
               b->available_copies);

        /* free the request node structure only (student_name moved to issued records) */
        free(req);
    }
    else
    {
        /* Issue failed */
        char *act = (char *)malloc(64);
        if (act)
        {
            sprintf(act, "ISSUE_FAIL %d", req->book_id);
            push_action(act);
        }

        if (!b)
        {
            printf("Not Available: Book ID %d does not exist.\n", req->book_id);
        }
        else
        {
            printf("Not Available: Book ID %d has no available copies.\n", req->book_id);
        }

        /* Since request failed, free student_name and the request node to avoid leak */
        if (req->student_name)
            free(req->student_name);
        free(req);
    }
}

void menu_view_all()
{
    display_books();
    display_queue();
    display_stack();
    display_issued_students();

    printf("\nDo you want to undo (pop) actions from the activity stack? (y/n): ");
    while (getchar() != '\n')
        ; /* flush newline */
    int c = getchar();
    if (c == 'y' || c == 'Y')
    {
        int k = 0;
        printf("\nEnter K (number of actions to pop): ");
        if (scanf("%d", &k) != 1)
        {
            printf("Invalid input.\n");
            while (getchar() != '\n')
                ;
            return;
        }
        while (getchar() != '\n')
            ;
        undo_k_actions(k);
    }
    else
    {
        /* consume rest of line if any */
        while (getchar() != '\n' && c != '\n')
            ;
    }
}

/* ---------------------- MAIN ---------------------- */

int main(void)
{
    printf("=== SmartLibrary (No arrays, all linked lists) ===\n");
    int choice = -1;
    for (;;)
    {
        printf("\n--- Menu ---\n");
        printf("1. Add Book\n");
        printf("2. Search Book\n");
        printf("3. Request Issue\n");
        printf("4. Process Next Request\n");
        printf("5. View All Data Structures\n");
        printf("6. Display Students Who Took Books\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Exiting.\n");
            break;
        }
        switch (choice)
        {
        case 1:
            menu_add_book();
            break;
        case 2:
            menu_search_book();
            break;
        case 3:
            menu_request_issue();
            break;
        case 4:
            menu_process_next_request();
            break;
        case 5:
            menu_view_all();
            break;
        case 6:
            display_issued_students();
            break;
        case 0:
            printf("Exiting SmartLibrary. Goodbye.\n");
            /* NOTE: program exits without freeing all memory for brevity.
               In production, you'd free books, stack, queue nodes, and issued records. */
            return 0;
        default:
            printf("Invalid choice. Try again.\n");
            break;
        }
    }
    return 0;
}
