#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

// حد الاستهلاك المسموح به للذاكرة (مثلاً 85%) قبل إرسال التنبيه
#define MEMORY_THRESHOLD_PERCENT 85

// دالة جلب معرف المستخدم وإرسال إشعار للنظام الرسومي
void send_alert_notification(double mem_usage) {
    char command[512];
    uid_t uid = getuid();

    // صياغة أمر إرسال الإشعار لـ Linux (متوافق مع البيئات الرسومية)
    snprintf(command, sizeof(command),
             "DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%d/bus "
             "notify-send -i dialog-warning 'تنبيه النظام (Karen) ⚠️' "
             "'استهلاك الذاكرة العشوائية مرتفع جداً! وصل إلى %.2f%%. يرجى إغلاق بعض التطبيقات.' "
             "--urgency=critical", uid, mem_usage);

    system(command);
}

int main() {
    struct sysinfo info;

    printf("[+] تم تشغيل أداة المراقبة (Karen) بنجاح في الخلفية...\n");
    printf("[+] سيتم تنبيهك فوراً إذا تجاوز استهلاك الذاكرة %d%%.\n", MEMORY_THRESHOLD_PERCENT);

    // تحويل البرنامج ليعمل كـ Daemon في الخلفية بهدوء
    if (daemon(1, 0) == -1) {
        perror("[-] فشل تشغيل الأداة في الخلفية");
        return 1;
    }

    // حلقة المراقبة المستمرة
    while (1) {
        if (sysinfo(&info) == 0) {
            // حساب الذاكرة المستخدمة فعلياً
            unsigned long total_ram = info.totalram;
            unsigned long free_ram = info.freeram;
            unsigned long used_ram = total_ram - free_ram;
            
            double mem_usage_percent = ((double)used_ram / total_ram) * 100.0;

            // إذا تجاوز الاستهلاك الحد المسموح، يتم إرسال التنبيه
            if (mem_usage_percent >= MEMORY_THRESHOLD_PERCENT) {
                send_alert_notification(mem_usage_percent);
                // النوم لمدة 5 دقائق بعد التنبيه لتفادي تكرار الإشعارات المزعجة
                sleep(300);
            }
        }

        // فحص حالة النظام كل 10 ثوانٍ لتوفير موارد المعالج
        sleep(10);
    }

    return 0;
}
