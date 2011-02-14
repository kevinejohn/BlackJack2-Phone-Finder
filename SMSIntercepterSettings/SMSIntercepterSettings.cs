using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using System.Text;

namespace PhoneFinderSettings
{
    class Program
    {
        static void Main(string[] args)
        {
            System.Diagnostics.Process.Start(@"\Program Files\GPSSMS\PhoneFinder.exe", "edit");
        }
    }
}
