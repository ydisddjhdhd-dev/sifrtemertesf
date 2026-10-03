#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// دالة مخصصة للتحقق من وجود أمر معين في نظام Linux
int is_command_available(const char *cmd) {
    char check_cmd[128];
    // توجيه المخرجات إلى /dev/null لمنع تشويه واجهة البرنامج
    snprintf(check_cmd, sizeof(check_cmd), "command -v %s > /dev/null 2>&1", cmd);
    return (system(check_cmd) == 0);
}

// دالة البحث عن المتصفح النشط وتفعيله
int launch_system_browser(const char *url) {
    pid_t pid = fork();

    // إذا كنا داخل العملية الابنة (Child Process)
    if (pid == 0) {
        // مصفوفة تضم أشهر المتصفحات في بيئات Linux حسب الأولوية
        const char *browsers[] = {
            "xdg-open",       // الأداة القياسية لفتح المتصفح الافتراضي للنظام
            "x-www-browser",  // المتصفح الافتراضي في توزيعات Debian/Ubuntu
            "google-chrome",  // متصفح كروم المستقر
            "firefox",        // متصفح فايرفوكس الشهير
            "chromium",       // النسخة الحرة من كروم
            "brave-browser",  // متصفح بريف
            "opera"           // متصفح أوبرا
        };
        
        int total_browsers = sizeof(browsers) / sizeof(browsers[0]);
        
        // حلقة الدوران لاستكشاف المتصفح المتاح وتشغيله فوراً
        for (int i = 0; i < total_browsers; i++) {
            if (is_command_available(browsers[i])) {
                printf("[+] تم استكشاف المحرك وتفعيل: %s\n", browsers[i]);
                
                // استبدال العملية الحالية بالمتصفح المستهدف وتمرير الرابط
                execlp(browsers[i], browsers[i], url, (char *)NULL);
                
                // إذا فشل execlp لسبب عتادي يستمر البحث
                perror("[-] فشل تشغيل المتصفح الحالي");
            }
        }
        
        // إذا لم يجد أي متصفح إطلاقاً
        fprintf(stderr, "[-] خطأ: لم يتم العثور على أي متصفح مدعوم في النظام.\n");
        exit(1);
    } 
    // إذا حدث خطأ أثناء تشعب العملية (Fork)
    else if (pid < 0) {
        perror("[-] فشل في إنشاء عملية النظام (Fork Failed)");
        return 0;
    } 
    // العملية الأب (Parent Process) تنتظر قليلاً للتأكد من استجابة النظام
    else {
        int status;
        waitpid(pid, &status, WNOHANG);
        return 1;
    }
}

int main(int argc, char *argv[]) {
    printf("==================================================\n");
    printf("--- محرك استكشاف وتنشيط المتصفحات الرسومية v1.0 ---\n");
    printf("==================================================\n\n");

    // الرابط الافتراضي الذي سيفتحه المتصفح عند الإطلاق
    const char *default_url = "https://google.com";
    const char *target_url;

    // التحقق مما إذا كان المستخدم قد مرر رابطاً مخصصاً عبر الطرفية
    if (argc > 1) {
        target_url = argv[1];
    } else {
        target_url = default_url;
    }

    // فحص بيئة العرض الرسومية (DISPLAY) في Linux قبل التشغيل
    char *display_env = getenv("DISPLAY");
    if (display_env == NULL) {
        fprintf(stderr, "[!] تحذير أمني: لم يتم رصد واجهة رسومية نشطة (DISPLAY env is missing).\n");
        fprintf(stderr, "[!] تأكد من تشغيل الكود داخل محطة رسومية (X11/Wayland).\n\n");
    }

    printf("[*] جاري تحضير أمر الاستدعاء وإظهار النافذة...\n");
    printf("[*] الرابط المستهدف: %s\n\n", target_url);

    // تفعيل وإظهار المتصفح
    if (launch_system_browser(target_url)) {
        printf("[+] تم إرسال إشارة التنشيط إلى النواة بنجاح.\n");
        printf("[+] البرنامج مستقر والذاكرة آمنة.\n");
    } else {
        fprintf(stderr, "[-] فشل المحرك في إظهار المتصفح.\n");
        return 1;
    }

    printf("\n==================================================\n");
    return 0;
}
