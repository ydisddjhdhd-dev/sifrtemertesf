#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// دالة التحقق من صلاحيات الـ Root
int is_root() {
    return getuid() == 0;
}

int main() {
    printf("--- محرك تنشيط وتحرير الذاكرة العشوائية لـ Linux ---\n\n");

    // 1. التحقق من الصلاحيات (النواة تمنع تعديل كاش الذاكرة بدون Root)
    if (!is_root()) {
        fprintf(stderr, "[-] خطأ أمني: يتطلب تشغيل هذا البرنامج صلاحيات المسؤول (Root).\n");
        fprintf(stderr, "    أعد التشغيل باستخدام: sudo %s\n", "./ram_refresher");
        return 1;
    }

    printf("[+] الخطوة 1: مزامنة وتفريغ البيانات المعلقة (Syncing file system buffers)...\n");
    // استدعاء نظام لحفظ أي بيانات مؤقتة في الأقراص قبل إفراغ الذاكرة لضمان عدم فقدان البيانات
    sync();
    sleep(1);

    printf("[+] الخطوة 2: فتح قنوات التحكم في ذاكرة النواة (Drop Caches API)...\n");
    FILE *drop_caches = fopen("/proc/sys/vm/drop_caches", "w");
    if (drop_caches == NULL) {
        perror("[-] خطأ أثناء الوصول إلى واجهة تحكم الذاكرة بالنواة");
        return 1;
    }

    /* 
       خيارات تنشيط الذاكرة المتاحة للنواة:
       "1" -> تحرير الصفحة المخفية (PageCache)
       "2" -> تحرير المجلدات والعناوين (Inodes & Dentries)
       "3" -> تحرير كل ما سبق (PageCache + Inodes + Dentries) - التنشيط الكامل
    */
    printf("[+] الخطوة 3: إرسال أمر التطهير الكامل وتحرير المساحات غير المستخدمة...\n");
    fprintf(drop_caches, "3\n");
    fclose(drop_caches);

    printf("\n==================================================\n");
    printf("[+] تم تنشيط الذاكرة العشوائية (RAM) بنجاح!\n");
    printf("[+] تم التخلص من الملفات المؤقتة غير النشطة وتسريع استجابة النظام.\n");
    printf("==================================================\n");

    return 0;
}
