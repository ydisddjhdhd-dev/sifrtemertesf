package main

import (
	"syscall"
	"unsafe"

	"golang.org/x/sys/windows"
)

const (
	IDC_BTN_DELETE = 101
	IDC_BTN_CREATE = 102
)

var (
	user32                   = windows.NewLazySystemDLL("user32.dll")
	procCreateWindowExW      = user32.NewProc("CreateWindowExW")
	procDefWindowProcW       = user32.NewProc("DefWindowProcW")
	procRegisterClassW       = user32.NewProc("RegisterClassW")
	procShowWindow           = user32.NewProc("ShowWindow")
	procGetMessageW          = user32.NewProc("GetMessageW")
	procTranslateMessage     = user32.NewProc("TranslateMessage")
	procDispatchMessageW     = user32.NewProc("DispatchMessageW")
	procPostQuitMessage      = user32.NewProc("PostQuitMessage")
	procMessageBoxW          = user32.NewProc("MessageBoxW")
	procLoadCursorW          = user32.NewProc("LoadCursorW")
	procGetWindowLongPtrW    = user32.NewProc("GetWindowLongPtrW")
)

const (
	WM_CREATE  = 0x0001
	WM_DESTROY = 0x0002
	WM_COMMAND = 0x0111

	WS_CHILD       = 0x40000000
	WS_VISIBLE     = 0x10000000
	WS_TABSTOP     = 0x00010000
	BS_DEFPUSHBUTTON = 0x00000001

	WS_OVERLAPPEDWINDOW = 0x00CF0000
	WS_MAXIMIZEBOX      = 0x00010000
	WS_THICKFRAME       = 0x00040000

	CW_USEDEFAULT = 0x80000000

	MB_OK              = 0x00000000
	MB_ICONINFORMATION = 0x00000040

	IDC_ARROW = 32512

	COLOR_WINDOW = 5

	GWLP_HINSTANCE = -6
)

type WNDCLASSW struct {
	Style         uint32
	LpfnWndProc   uintptr
	CbClsExtra    int32
	CbWndExtra    int32
	HInstance     windows.Handle
	HIcon         windows.Handle
	HCursor       windows.Handle
	HbrBackground windows.Handle
	LpszMenuName  *uint16
	LpszClassName *uint16
}

type MSG struct {
	Hwnd     windows.Handle
	Message  uint32
	WParam   uintptr
	LParam   uintptr
	Time     uint32
	Pt       struct{ X, Y int32 }
}

func loword(wParam uintptr) uint16 {
	return uint16(wParam & 0xffff)
}

func windowProc(hwnd windows.Handle, uMsg uint32, wParam uintptr, lParam uintptr) uintptr {
	switch uMsg {
	case WM_CREATE:
		btnText1, _ := syscall.UTF16PtrFromString("حذف تعريفات إنفيديا")
		btnClass1, _ := syscall.UTF16PtrFromString("BUTTON")
		
		hInstPtr, _, _ := procGetWindowLongPtrW.Call(uintptr(hwnd), uintptr(GWLP_HINSTANCE))

		procCreateWindowExW.Call(
			0,
			uintptr(unsafe.Pointer(btnClass1)),
			uintptr(unsafe.Pointer(btnText1)),
			WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
			50, 50, 200, 40,
			uintptr(hwnd),
			uintptr(IDC_BTN_DELETE),
			hInstPtr,
			0,
		)

		btnText2, _ := syscall.UTF16PtrFromString("إنشاء تعريفات جديدة إنفيديا")
		procCreateWindowExW.Call(
			0,
			uintptr(unsafe.Pointer(btnClass1)),
			uintptr(unsafe.Pointer(btnText2)),
			WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
			50, 110, 200, 40,
			uintptr(hwnd),
			uintptr(IDC_BTN_CREATE),
			hInstPtr,
			0,
		)

	case WM_COMMAND:
		if loword(wParam) == IDC_BTN_DELETE {
			text, _ := syscall.UTF16PtrFromString("تم إرسال أمر حذف تعريفات إنفيديا.")
			caption, _ := syscall.UTF16PtrFromString("تنبيه")
			procMessageBoxW.Call(uintptr(hwnd), uintptr(unsafe.Pointer(text)), uintptr(unsafe.Pointer(caption)), MB_OK|MB_ICONINFORMATION)
		} else if loword(wParam) == IDC_BTN_CREATE {
			text, _ := syscall.UTF16PtrFromString("تم إرسال أمر إنشاء تعريفات إنفيديا الجديدة.")
			caption, _ := syscall.UTF16PtrFromString("تنبيه")
			procMessageBoxW.Call(uintptr(hwnd), uintptr(unsafe.Pointer(text)), uintptr(unsafe.Pointer(caption)), MB_OK|MB_ICONINFORMATION)
		}

	case WM_DESTROY:
		procPostQuitMessage.Call(0)
		return 0

	default:
		ret, _, _ := procDefWindowProcW.Call(uintptr(hwnd), uintptr(uMsg), wParam, lParam)
		return ret
	}
	return 0
}

func main() {
	hInstance := windows.Handle(0)
	className, _ := syscall.UTF16PtrFromString("NVIDIA_Control_Class")

	cursor, _, _ := procLoadCursorW.Call(0, uintptr(IDC_ARROW))

	wc := WNDCLASSW{
		LpfnWndProc:   syscall.NewCallback(windowProc),
		HInstance:     hInstance,
		LpszClassName: className,
		HCursor:       windows.Handle(cursor),
		HbrBackground: windows.Handle(COLOR_WINDOW + 1),
	}

	procRegisterClassW.Call(uintptr(unsafe.Pointer(&wc)))

	windowName, _ := syscall.UTF16PtrFromString("إدارة تعريفات إنفيديا")
	hwnd, _, _ := procCreateWindowExW.Call(
		0,
		uintptr(unsafe.Pointer(className)),
		uintptr(unsafe.Pointer(windowName)),
		WS_OVERLAPPEDWINDOW & ^uint32(WS_MAXIMIZEBOX) & ^uint32(WS_THICKFRAME),
		uintptr(CW_USEDEFAULT), uintptr(CW_USEDEFAULT), 310, 210,
		0, 0, uintptr(hInstance), 0,
	)

	if hwnd == 0 {
		return
	}

	procShowWindow.Call(hwnd, uintptr(windows.SW_SHOWDEFAULT))

	var msg MSG
	for {
		ret, _, _ := procGetMessageW.Call(uintptr(unsafe.Pointer(&msg)), 0, 0, 0)
		if int32(ret) <= 0 {
			break
		}
		procTranslateMessage.Call(uintptr(unsafe.Pointer(&msg)))
		procDispatchMessageW.Call(uintptr(unsafe.Pointer(&msg)))
	}
}
