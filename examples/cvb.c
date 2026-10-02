#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TASKS 10
#define MAX_LEN 50

typedef struct {
    char description[MAX_LEN];
    bool is_completed;
} Task;

void print_separator() {
    printf("-----------------------------------\n");
}

void show_menu() {
    print_separator();
    printf("        منظم المهام الذكي         \n");
    print_separator();
    printf("1. عرض قائمة المهام الحالية\n");
    printf("2. إضافة مهمة جديدة للقائمة\n");
    printf("3. تحديد مهمة كمكتملة\n");
    printf("4. خروج من البرنامج\n");
    printf("اختر الإجراء المناسب (1-4): ");
}

void list_tasks(Task tasks[], int count) {
    print_separator();
    if (count == 0) {
        printf("قائمة المهام فارغة حالياً.\n");
        return;
    }
    printf("قائمة المهام:\n");
    for (int i = 0; i < count; i++) {
        printf("[%d] %s (%s)\n", i + 1, tasks[i].description,
               tasks[i].is_completed ? "مكتملة" : "غير مكتملة");
    }
}

void add_task(Task tasks[], int *count) {
    if (*count >= MAX_TASKS) {
        printf("عذراً، القائمة ممتلئة تماماً!\n");
        return;
    }
    printf("أدخل وصف المهمة الجديدة: ");
    getchar(); 
    fgets(tasks[*count].description, MAX_LEN, stdin);
    tasks[*count].description[strcspn(tasks[*count].description, "\n")] = 0;
    tasks[*count].is_completed = false;
    (*count)++;
    printf("تمت إضافة المهمة بنجاح.\n");
}

void complete_task(Task tasks[], int count) {
    if (count == 0) {
        printf("لا توجد مهام لتعديلها.\n");
        return;
    }
    int index;
    printf("أدخل رقم المهمة المراد إكمالها: ");
    scanf("%d", &index);
    if (index < 1 || index > count) {
        printf("رقم المهمة غير صحيح!\n");
        return;
    }
    tasks[index - 1].is_completed = true;
    printf("تم تحديث حالة المهمة بنجاح.\n");
}

int main() {
    Task tasks[MAX_TASKS];
    int task_count = 0;
    int choice;
    bool running = true;

    while (running) {
        show_menu();
        if (scanf("%d", &choice) != 1) {
            printf("إدخال خاطئ! يرجى إدخال رقم.\n");
            getchar();
            continue;
        }

        switch (choice) {
            case 1:
                list_tasks(tasks, task_count);
                break;
            case 2:
                add_task(tasks, &task_count);
                break;
            case 3:
                complete_task(tasks, task_count);
                break;
            case 4:
                printf("شكراً لاستخدامك البرنامج. وداعاً!\n");
                running = false;
                break;
            default:
                printf("خيار غير صحيح، حاول مجدداً.\n");
        }
    }
    return 0;
}
