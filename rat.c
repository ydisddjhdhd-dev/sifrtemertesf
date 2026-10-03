#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// دالة لجلب معرف المستخدم الحالي لضمان إرسال الإشعار للواجهة الرسومية الصحيحة
void send_desktop_notification() {
    char command[512];
    uid_t uid = getuid();

    // صياغة أمر إرسال الإشعار مع ضبط البيئة الرسومية لـ Linux
    snprintf(command, sizeof(command),
             "DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%d/bus "
             "notify-send -i timed-out 'تذكير بالنوم 😴' "
             "'الساعة الآن 10 مساءً. حان وقت إغلاق الجهاز والاستعداد للنوم لتستيقظ بنشاط!' "
             "--urgency=critical", uid);

    // تنفيذ أمر الإشعار
    system(command);

    // تشغيل صوت تنبيه للنظام (إذا كان متوفراً في التوزيعة)
    snprintf(command, sizeof(command),
             "DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%d/bus "
             "paplay /usr/share/sounds/freedesktop/stereo/complete.oga 2>/dev/null", uid);
    system(command);
}

int main() {
    printf("[+] تم تشغيل برنامج التذكير بالنوم في الخلفية بنجاح...\n");
    printf("[+] سيتم تنبيهك يومياً الساعة 10:00 مساءً.\n");

    // تحويل البرنامج ليعمل في الخلفية (Daemon) بشكل مستمر
    // المعامل 1 يعني الاحتفاظ بمسار المجلد الحالي، و 0 يعني إخفاء المخرجات النصية في الطرفية
    if (daemon(1, 0) == -1) {
        perror("[-] فشل تشغيل البرنامج في الخلفية");
        return 1;
    }

    // حلقة فحص الوقت اللانهائية
    while (1) {
        time_t now = time(NULL);
        struct tm *current_time = localtime(&now);

        // التحقق مما إذا كانت الساعة 22 (10 مساءً) والدقيقة 00
        if (current_time->tm_hour == 22 && current_time->tm_min == 0) {
            send_desktop_notification();
            
            // النوم لمدة 61 ثانية لمنع تكرار الإشعار في نفس الدقيقة
            sleep(61); 
        }

        // فحص الوقت كل 30 ثانية لتوفير استهلاك المعالج (CPU)
        sleep(30);
    }

    return 0;
}
