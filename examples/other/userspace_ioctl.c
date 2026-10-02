/*  userspace_ioctl.c - the process to use ioctl's to control the kernel module
/*  userspace_ioctl.c - the process to use ioctl operations to control the kernel module
 *
 *  Until now we could have used cat for input and output.  But now
 *  we need to do ioctl's, which require writing our own process. 
 *  we need to do ioctl operations, which require writing our own process. 
