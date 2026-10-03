#!/bin/bash

# تنظيف الشاشة وترتيب المظهر
clear
echo "=========================================="
echo "    مستودع sifrtemertesf - مشغل ومُنظم الأكواد"
echo "=========================================="

# 1. ترتيب وتنظيف ملفات المشروع (إنشاء مجلد للمخرجات إن لم يكن موجوداً)
OUTPUT_DIR="./build_outputs"
if [ ! -d "$OUTPUT_DIR" ]; then
    echo "[+] إنشاء مجلد لتنظيم مخرجات التشغيل..."
    mkdir "$OUTPUT_DIR"
fi

# 2. تجميع وترجمة ملفات لغة C تلقائياً (بما أن المشروع غني بها)
echo -e "\n[*] جاري فحص وترجمة ملفات C المتوفرة..."
for c_file in *.c; do
    # التأكد من وجود ملفات C فعلياً لتجنب الأخطاء
    [ -e "$c_file" ] || continue
    
    filename=$(basename -- "$c_file")
    extension="${filename##*.}"
    filename="${filename%.*}"
    
    echo " -> جاري ترجمة: $c_file"
    gcc "$c_file" -o "$OUTPUT_DIR/$filename" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "    [✔] تم الترتيب بنجاح والمخرج في: $OUTPUT_DIR/$filename"
    else
        echo "    [❌] فشلت ترجمة $c_file (تحقق من المكتبات أو الأكواد)"
    fi
done

# 3. تشغيل ملفات لغة C الأساسية المترجمة بنجاح (مثل ram_refresher أو advanced_reboot)
echo -e "\n[*] جاري تشغيل الخدمات البرمجية الأساسية (C)..."
if [ -f "$OUTPUT_DIR/ram_refresher" ]; then
    echo " -> تشغيل ram_refresher في الخلفية..."
    "$OUTPUT_DIR/ram_refresher" &
fi

# 4. تشغيل ملفات JavaScript الأساسية (Node.js) إذا كانت تعتمد على بيئة تشغيل سيرفر
echo -e "\n[*] جاري فحص وتشغيل ملفات الويب والـ JS..."
if [ -f "package.json" ] && [ -f "index.js" ]; then
    echo " -> تم العثور على مشروع Node.js. جاري تثبيت الاعتماديات وتشغيل index.js..."
    npm install --quiet
    node index.js &
elif [ -f "index.js" ]; then
    echo " -> تشغيل ملف index.js مباشرة عبر Node..."
    node index.js &
fi

echo -e "\n=========================================="
echo "    [✔] تم تنظيم وتشغيل كافة الملفات المتاحة!"
echo "=========================================="
