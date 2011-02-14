using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Data;
using Microsoft.WindowsMobile.Status;

namespace SoundAlert
{
    class Program
    {
        #region PlaySound
        // To play a sound, we need to PInvoke into coredll.dll and call PlaySound
        [System.Runtime.InteropServices.DllImport("coredll.dll")]
        static extern int PlaySound(string pszSound, IntPtr hMod, int fdwSound);

        const int SND_FILENAME = 0x20000;
        const int SND_SYNC = 0x0;
        const int SND_ASYNC = 0x1;
        const int SND_LOOP = 0x8;
        #endregion


        static void Main(string[] args)
        {

            string soundFile = "\\Program Files\\GPSSMS\\overhere.wav"; // sound to play when event raised

            if (args.Length == 1)
            {
                soundFile = args[0];

            }


            // Play the sound in a loop
            PlaySound(soundFile, IntPtr.Zero, SND_FILENAME | SND_ASYNC | SND_LOOP);

            // While playing the sound, display owner information if we have it.

            // get the owner name, otherwise default to "the owner"
            string owner = SystemState.OwnerName;
            if (owner.Length == 0)
            {
                owner = "the owner";
            }

            // build the message to display on the phone
            string message = owner + " is looking for his/her phone.  ";
            message += "If you have found this phone, please contact " + owner + ". ";

            // look to see if we have contact information for the owner.  
            // if we do, then we'll display the ways the owner can be contacted.
            string email = SystemState.OwnerEmail;
            string phone = SystemState.OwnerPhoneNumber;

            if (phone.Length > 0 || email.Length > 0)
            {
                message += "You can contact " + owner + " via the following: \n";
            }

            if (phone.Length > 0)
            {
                message += phone + "\n";
            }

            if (email.Length > 0)
            {
                message += email + "\n";
            }

            // Show the message
            MessageBox.Show(message);

            // The message box has been dismissed, stop the sound and exit
            PlaySound(null, IntPtr.Zero, 0);



        }
    }
}
