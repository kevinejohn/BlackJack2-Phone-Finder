using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace PhoneRestart
{
    class Program
    {
        static void Main(string[] args)
        {
            ResetPocketPC();
        }

        [System.Runtime.InteropServices.DllImport("coredll.dll")]
        public static extern int KernelIoControl(int dwIoControlCode, IntPtr lpInBuf, int nInBufSize, IntPtr lpOutBuf, int nOutBufSize,ref int lpBytesReturned);
        
		static private int CTL_CODE(int DeviceType, int Func, int Method, int Access)
        {
           return (DeviceType << 16) | (Access << 14) | (Func << 2) | Method;
        }

        static private int ResetPocketPC()
        {
            const int FILE_DEVICE_HAL = 0x101;
            const int METHOD_BUFFERED = 0;
            const int FILE_ANY_ACCESS = 0;
            int bytesReturned = 0;
            int IOCTL_HAL_REBOOT;
            IOCTL_HAL_REBOOT = CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS);
            return KernelIoControl(IOCTL_HAL_REBOOT, IntPtr.Zero, 0, IntPtr.Zero, 0, ref bytesReturned);
        }
    }
}
