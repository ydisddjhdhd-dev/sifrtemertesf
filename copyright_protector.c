#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIGNATURE_MAGIC "CPR_PROT"
#define KEY_SIZE 16

// بنية هيدر الحقوق الذي سيتم حقنه في نهاية الملف
typedef struct {
    char magic[8];      6289227   // معرف الحماية الخاص بالبرنامج
    char owner[64];    mohmmed    // اسم مالك الحقوق
    char protected_at[32]; 2024// تاريخ الحماية
    unsigned long original_size;4m // الحجم الأصلي للملف
    unsigned int checksum;  5678// بصمة التحقق من التلاعب
} CopyrightHeader;

// 1. منظومة توليد بصمة التحقق من نزاهة البيانات من الصفر
unsigned int calculate_checksum(const unsigned char *data, size_t size) {
    unsigned int checksum = 0x811C9DC5; // FNV-1a initial offset basis
    for (size_t i = 0; i < size; i++) {
        checksum ^= data[i];
        checksum *= 0x01000193; // FNV-1a prime
    }
    return checksum;
}

// 2. محرك التشفير وفك التشفير باستخدام مفتاح ديناميكي ممتد
void crypt_stream(unsigned char *data, size_t size, const char *key) {
    size_t key_len = strlen(key);
    for (size_t i = 0; i < size; i++) {
        // تشفير تشابكي ديناميكي يعتمد على موقع البايت والمفتاح
        data[i] ^= (key[i % key_len] + (i * 13)) & 0xFF;
    }
}

// 3. دالة الحصول على الوقت الحالي وتنسيقه
void get_current_timestamp(char *buffer, size_t max_size) {
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(buffer, max_size, "%Y-%m-%d %H:%M:%S", time_info);
}

// 4. نظام حماية الملف وحقن الحقوق
int protect_file(const char *input_path, const char *output_path, const char *owner, const char *key) {
    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        printf("[-] خطأ: لا يمكن فتح الملف المصدر: %s\n", input_path);
        return 0;
    }

    // معرفة حجم الملف
    fseek(in_file, 0, SEEK_END);
    long file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    if (file_size <= 0) {
        printf("[-] خطأ: الملف المصدر فارغ أو تالف.\n");
        fclose(in_file);
        return 0;
    }

    // حجز ذاكرة للملف
    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (!buffer) {
        printf("[-] خطأ: فشل في حجز الذاكرة.\n");
        fclose(in_file);
        return 0;
    }

    // قراءة محتوى الملف
    fread(buffer, 1, file_size, in_file);
    fclose(in_file);

    printf("[+] جاري فحص ملفك البرمجي وحساب بصمة النزاهة...\n");
    unsigned int data_checksum = calculate_checksum(buffer, file_size);

    printf("[+] جاري تشفير البيانات وتأمينها لمسح أي قراءة مباشرة...\n");
    crypt_stream(buffer, file_size, key);

    // تجهيز هيدر الحقوق والملكية
    CopyrightHeader header;
    memset(&header, 0, sizeof(CopyrightHeader));
    memcpy(header.magic, SIGNATURE_MAGIC, 8);
    strncpy(header.owner, owner, sizeof(header.owner) - 1);
    get_current_timestamp(header.protected_at, sizeof(header.protected_at));
    header.original_size = file_size;
    header.checksum = data_checksum;

    // كتابة الملف المحمي الجديد
    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        printf("[-] خطأ: لا يمكن إنشاء ملف الخرج: %s\n", output_path);
        free(buffer);
        return 0;
    }

    // كتابة البيانات المشفرة أولاً ثم إلحاق الحقوق في النهاية
    fwrite(buffer, 1, file_size, out_file);
    fwrite(&header, sizeof(CopyrightHeader), 1, out_file);

    fclose(out_file);
    free(buffer);

    printf("\n==================================================\n");
    printf("[+] تم حماية الملف بنجاح وحقن حقوق الملكية!\n");
    printf("[+] المالك المسجل: %s\n", header.owner);
    printf("[+] تاريخ التوقيع: %s\n", header.protected_at);
    printf("[+] الحجم الإجمالي: %ld بايت\n", file_size + sizeof(CopyrightHeader));
    printf("==================================================\n");

    return 1;
}

// 5. نظام التحقق من الحقوق وفك حماية الملف
int verify_and_unprotect(const char *input_path, const char *output_path, const char *key) {
    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        printf("[-] خطأ: لا يمكن فتح الملف المحمي: %s\n", input_path);
        return 0;
    }

    // معرفة حجم الملف الكلي
    fseek(in_file, 0, SEEK_END);
    long total_size = ftell(in_file);
    
    if (total_size <= (long)sizeof(CopyrightHeader)) {
        printf("[-] خطأ: الملف ليس ملفاً محمياً بحقوق أو أنه تالف جداً.\n");
        fclose(in_file);
        return 0;
    }

    // قراءة هيدر الحقوق من نهاية الملف
    CopyrightHeader header;
    fseek(in_file, total_size - sizeof(CopyrightHeader), SEEK_SET);
    fread(&header, sizeof(CopyrightHeader), 1, in_file);

    // التحقق من معرف الحماية الخاص بنا
    if (memcmp(header.magic, SIGNATURE_MAGIC, 8) != 0) {
        printf("[-] خطأ عتادي: هذا الملف لا يحتوي على بصمة حقوق رقمية تابعة للنظام.\n");
        fclose(in_file);
        return 0;
    }

    // قراءة البيانات المشفرة
    long encrypted_size = header.original_size;
    unsigned char *buffer = (unsigned char *)malloc(encrypted_size);
    if (!buffer) {
        printf("[-] خطأ: فشل في حجز ذاكرة القراءة.\n");
        fclose(in_file);
        return 0;
    }

    fseek(in_file, 0, SEEK_SET);
    fread(buffer, 1, encrypted_size, in_file);
    fclose(in_file);

    printf("\n==================================================\n");
    printf("[+] تم استكشاف بصمة الحقوق بنجاح:\n");
    printf("[+] المالك الشرعي للملف: %s\n", header.owner);
    printf("[+] تاريخ وتوقيع الحماية: %s\n", header.protected_at);
    printf("==================================================\n");

    printf("[+] جاري فك تشفير البيانات المكتوبة...\n");
    crypt_stream(buffer, encrypted_size, key);

    printf("[+] جاري فحص مطابقة بصمة النزاهة للحماية من التلاعب...\n");
    unsigned int current_checksum = calculate_checksum(buffer, encrypted_size);

    if (current_checksum != header.checksum) {
        printf("\n[!!!] تحذير أمني صارم: تم كشف تلاعب في محتويات الملف! [!!!]\n");
        printf("[!] البصمة الأصلية: %X | البصمة الحالية: %X\n", header.checksum, current_checksum);
        printf("[!] سيتم إلغاء العملية لحماية سلامة النظام.\n");
        free(buffer);
        return 0;
    }

    printf("[+] فحص النزاهة ممتاز (100%%). الملف آمن ولم يتم التلاعب به.\n");

    // كتابة الملف المسترجع الأصلي بدون هيدر الحقوق
    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        printf("[-] خطأ: لا يمكن إنشاء الملف المسترجع: %s\n", output_path);
        free(buffer);
        return 0;
    }

    fwrite(buffer, 1, encrypted_size, out_file);
    fclose(out_file);
    free(buffer);

    printf("[+] تم فك الحماية بنجاح واسترجاع الملف الأصلي في: %s\n", output_path);
    return 1;
}

// 6. واجهة التحكم الرئيسية ومدخل البرنامج
int main(int argc, char *argv[]) {
    printf("\n--- نظام حماية الملفات وحقن الحقوق الرقمية v1.0 ---\n");

    if (argc < 4) {
        printf("\nطريقة الاستخدام الشاملة:\n");
        printf("  لحماية ملف وحقن الحقوق فيه:\n");
        printf("    %s -p [الملف_الأصلي] [الملف_المحمي_الجديد] \"[اسم المالك]\"\n", argv[0]);
        printf("  لفحص الحقوق واسترجاع الملف الأصلي:\n");
        printf("    %s -v [الملف_المحمي] [الملف_المسترجع_الجديد]\n", argv[0]);
        return 1;
    }

    // مفتاح تعمية ثابت داخلي (يمكنك تغييره لحماية ملفاتك)
    const char *secret_key = "ProtX99_SecureKey";
    char *mode = argv[1];

    if (strcmp(mode, "-p") == 0) {
        if (argc < 5) {
            printf("[-] خطأ: يرجى كتابة اسم مالك الحقوق بين علامتي تنصيص.\n");
            return 1;
        }
        protect_file(argv[2], argv[3], argv[4], secret_key);
    } 
    else if (strcmp(mode, "-v") == 0) {
        verify_and_unprotect(argv[2], argv[3], secret_key);
    } 
    else {
        printf("[-] خطأ: وضع التشغيل غير معروف استخدم (-p) للحماية أو (-v) للتحقق المستمر.\n");
        return 1;
    }

    return 0;
}
