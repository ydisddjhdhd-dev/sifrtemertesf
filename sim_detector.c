#include <stdio.h>
#include <string.h>
#include <ctype.h>

// دالة لمعالجة وتحليل الشريحة
void detect_algerian_sim(const char *raw_number) {
    char num[30] = {0};
    int idx = 0;

    // تنظيف المدخلات والاحتفاظ بالأرقام فقط
    for (int i = 0; raw_number[i] != '\0'; i++) {
        if (isdigit(raw_number[i])) {
            num[idx++] = raw_number[i];
        }
    }
    num[idx] = '\0';

    char *final_ptr = num;

    // التعامل مع الرمز الدولي الجزائري 213 أو 00213
    if (strncmp(final_ptr, "00213", 5) == 0) {
        final_ptr += 5;
    } else if (strncmp(final_ptr, "213", 3) == 0) {
        final_ptr += 3;
    }

    // التحقق من طول الرقم الجزائري القياسي بعد إزالة المفتاح الدولي (يجب أن يكون 9 أرقام يبدأ بـ 5 أو 6 أو 7)
    // أو 10 أرقام إذا كان يبدأ بـ 05 أو 06 أو 07
    if (strlen(final_ptr) == 9) {
        // تحويله إلى صيغة 10 أرقام القياسية لتسهيل الفحص
        char temp[11] = "0";
        strcat(temp, final_ptr);
        strcpy(num, temp);
        final_ptr = num;
    }

    printf("=======================================\n");
    printf("[*] جاري تحليل الرقم: %s\n", raw_number);

    if (strlen(final_ptr) != 10 || final_ptr[0] != '0') {
        printf("[-] النتيجة: رقم غير صالح أو لا يطابق معايير الهواتف في الجزائر.\n");
        return;
    }

    // استكشاف وتفعيل نوع الشبكة بناءً على المقدمة
    if (final_ptr[1] == '5') {
        printf("[+] نوع الشريحة: أوريدو (Ooredoo الجزائر) 🔴\n");
        printf("[+] حالة الحزمة: مدعومة بنجاح.\n");
    } 
    else if (final_ptr[1] == '7') {
        printf("[+] نوع الشريحة: جيزي (Djezzy الجزائر) 🔴\n");
        printf("[+] حالة الحزمة: مدعومة بنجاح.\n");
    } 
    else if (final_ptr[1] == '6') {
        printf("[+] نوع الشريحة: موبيليس (Mobilis الجزائر) 🟢\n");
        printf("[+] حالة الحزمة: مدعومة بنجاح.\n");
    } 
    else {
        printf("[-] النتيجة: شبكة جزائرية مجهولة أو خط أرضي.\n");
    }
}

int main() {
    printf("--- محرك دعم وتدقيق شرائح الاتصال الجزائرية ---\n\n");

    // أمثلة لاختبار دعم الأرقام بصيغ مختلفة
    detect_algerian_sim("0550 12 34 56");     // أوريدو محلي
    detect_algerian_sim("+213 770 99 88 77"); // جيزي دولي
    detect_algerian_sim("213661223344");      // موبيليس دولي بدون زائد
    detect_algerian_sim("031445566");         // رقم هاتف ثابت (قسنطينة مثلاً) لبيان الرفض

    return 0;
}
