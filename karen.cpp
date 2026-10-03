#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <sys/sysinfo.h>

class SystemMonitor {
private:
    const int memory_threshold_percent;
    
    // دالة خاصة لإرسال الإشعار لسطح المكتب في نظام Linux
    void send_alert_notification(double mem_usage) const {
        uid_t uid = getuid();
        char command[512];

        // صياغة أمر إرسال الإشعار لـ Linux الرسومي
        std::snprintf(command, sizeof(command),
                 "DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%d/bus "
                 "notify-send -i dialog-warning 'تنبيه النظام (Karen) ⚠️' "
                 "'استهلاك الذاكرة العشوائية مرتفع جداً! وصل إلى %.2f%%. يرجى إغلاق بعض التطبيقات.' "
                 "--urgency=critical", uid, mem_usage);

        // تنفيذ أمر النظام
        std::system(command);
    }

public:
    // المشيد لتحديد حد الاستهلاك (الافتراضي 85%)
    SystemMonitor(int threshold = 85) : memory_threshold_percent(threshold) {}

    // الدالة الأساسية لبدء المراقبة
    void start() {
        struct sysinfo info;

        std::cout << "[+] تم تشغيل أداة المراقبة (Karen) بلغة C++ بنجاح في الخلفية...\n";
        std::cout << "[+] سيتم تنبيهك فوراً إذا تجاوز استهلاك الذاكرة " << memory_threshold_percent << "%.\n";

        // تحويل البرنامج ليعمل كـ Daemon في الخلفية بهدوء
        if (daemon(1, 0) == -1) {
            std::perror("[-] فشل تشغيل الأداة في الخلفية");
            return;
        }

        // حلقة المراقبة المستمرة بنمط C++
        while (true) {
            if (sysinfo(&info) == 0) {
                unsigned long total_ram = info.totalram;
                unsigned long free_ram = info.freeram;
                unsigned long used_ram = total_ram - free_ram;
                
                double mem_usage_percent = (static_cast<double>(used_ram) / total_ram) * 100.0;

                // التحقق من تجاوز الحد المسموح
                if (mem_usage_percent >= memory_threshold_percent) {
                    send_alert_notification(mem_usage_percent);
                    
                    // النوم لمدة 5 دقائق (300 ثانية) لتفادي الإشعارات المكررة والمزعجة
                    std::this_thread::sleep_for(std::chrono::minutes(5));
                }
            }

            // فحص حالة النظام كل 10 ثوانٍ لتوفير موارد المعالج
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
};

int main() {
    // إنشاء كائن المراقبة وبدء التشغيل
    SystemMonitor monitor(85);
    monitor.start();

    return 0;
}
