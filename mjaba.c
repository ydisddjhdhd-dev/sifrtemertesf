#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// تعريف أنواع الشبكات المدعومة في الجزائر
typedef enum {
    SIM_OOREDOO,
    SIM_DJEZZY,
    SIM_MOBILIS,
    SIM_UNKNOWN
} SimType;

// دالة لتنظيف الرقم من المسافات أو الرموز الزائدة
void clean_phone_number(const char *input, char *output) {
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i])) {
            output[j++] = input[i];
        } else if (input[i] == '+' && j == 0) {
            output[j++] = input[i]; // الاحتفاظ بـ + في البداية فقط إذا وجد
        }
    }
    output[j] = '\0';
