#!/usr/bin/env node

const { spawn } = require('child_process');
const path = require('path');

// تحديد مسار ملف البايثون في نفس المجلد
const scriptPath = path.join(__dirname, 'code.py');

// تشغيل ملف البايثون وتمرير أي مدخلات أو وسائط (Arguments)
const pythonProcess = spawn('python3', [scriptPath, ...process.argv.slice(2)], {
    stdio: 'inherit' // يضمن أن واجهة البايثون والألوان تظهر وتتفاعل في ترمكس بشكل طبيعي
});

pythonProcess.on('error', (err) => {
    console.error('حدث خطأ أثناء تشغيل بايثون. تأكد من تثبيت python3 في ترمكس.');
});
