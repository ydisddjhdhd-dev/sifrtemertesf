#!/bin/bash

# تلوين المخرجات لجعل الواجهة تبدو احترافية
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # لا يوجد لون

echo -e "${BLUE}=== بدء إعداد وتفعيل بيئة العمل والمشروع ===${NC}"

# 1. تحديث مستودعات Termux وتثبيت الحزم الأساسية
echo -e "${YELLOW}[1/4] جاري تحديث الحزم وتثبيت الأدوات الأساسية...${NC}"
pkg update -y && pkg upgrade -y
pkg install -y clang golang nodejs x11-repo tur-repo tigervnc xfce4 xfce4-terminal

# 2. بناء وتجميع ملفات لغات البرمجة المتفرقة تلقائياً
echo -e "${YELLOW}[2/4] جاري تجميع وبناء ملفات البرمجة (C / Go / JS)...${NC}"

# تجميع ملفات C المتوفرة في المستودع
if [ -f "ram_refresher.c" ]; then
    clang ram_refresher.c -o ram_refresher
    echo -e "${GREEN}✓ تم تجميع ram_refresher بنجاح.${NC}"
fi

if [ -f "util.c" ]; then
    clang util.c -o util_tool
    echo -e "${GREEN}✓ تم تجميع util.c بنجاح.${NC}"
fi

# بناء ملف Go
if [ -f "jao.go" ]; then
    go build jao.go
    echo -e "${GREEN}✓ تم بناء ملف jao.go بنجاح.${NC}"
fi

# تثبيت حزم Node.js إذا كان ملف النظم متاحاً
if [ -f "package.json" ]; then
    npm install
    echo -e "${GREEN}✓ تم تثبيت اعتمادات Node.js بنجاح.${NC}"
fi

# 3. إعداد الواجهة الرسومية (GUI) عبر VNC
echo -e "${YELLOW}[3/4] جاري ضبط إعدادات الواجهة الرسومية (XFCE)...${NC}"
mkdir -p ~/.vnc
echo "#!/bin/sh" > ~/.vnc/xstartup
echo "xfce4-session &" >> ~/.vnc/xstartup
chmod +x ~/.vnc/xstartup

# 4. تشغيل خادم الواجهة الرسومية وتنبيه المستخدم
echo -e "${YELLOW}[4/4] جاري تشغيل الواجهة الرسومية...${NC}"
# إيقاف أي سيرفر قديم لتفادي الأخطاء
vncserver -kill :1 2>/dev/null
# تشغيل السيرفر على المنفذ الافتراضي
vncserver :1

echo -e "${GREEN}===========================================${NC}"
echo -e "${GREEN}✓ تم تشغيل وتفعيل كل شيء بنجاح!${NC}"
echo -e "${BLUE}ℹ يمكنك الآن فتح تطبيق VNC Viewer والاتصال بـ: localhost:1${NC}"
echo -e "${BLUE}ℹ ستجد واجهة XFCE الرسومية تعمل وبداخلها أدواتك الجاهزة.${NC}"
echo -e "${GREEN}===========================================${NC}"
