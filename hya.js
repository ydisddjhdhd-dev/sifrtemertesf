const fs = require('fs');
const path = require('path');

// تحديد المجلد الحالي (أو يمكنك كتابة مسار المجلد هنا)
const directoryPath = __dirname; 
const outputFile = path.join(directoryPath, 'combined.js');

try {
    // 1. قراءة جميع الملفات في المجلد
    const files = fs.readdirSync(directoryPath);

    // 2. تصفية الملفات للحصول على ملفات الـ JS فقط (واستبعاد ملف المخرجات نفسه إذا كان موجوداً)
    const jsFiles = files.filter(file => 
        path.extname(file).toLowerCase() === '.js' && file !== 'combined.js'
    );

    // 3. دمج محتوى الملفات
    let combinedContent = '';
    jsFiles.forEach(file => {
        const filePath = path.join(directoryPath, file);
        const fileContent = fs.readFileSync(filePath, 'utf8');
        
        // إضافة تعليق يوضح اسم الملف الأصلي لترتيب الكود، ثم إضافة المحتوى
        combinedContent += `\n/* --- Start of File: ${file} --- */\n`;
        combinedContent += fileContent;
        combinedContent += `\n/* --- End of File: ${file} --- */\n`;
    });

    // 4. كتابة المحتوى المدمج في الملف الجديد
    fs.writeFileSync(outputFile, combinedContent, 'utf8');
    console.log(`✅ تم دمج ${jsFiles.length} ملفات بنجاح في ملف: combined.js`);

} catch (err) {
    console.error('❌ حدث خطأ أثناء دمج الملفات:', err);
}
