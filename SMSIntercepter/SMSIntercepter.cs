//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
#region Using directives

using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Data;
#endregion

using Microsoft.WindowsMobile.PocketOutlook.MessageInterception;
using Microsoft.WindowsMobile.PocketOutlook;
using Microsoft.WindowsMobile.Status;

namespace PhoneFinder
{
    /// <summary>
    /// Summary description for form.
    /// </summary>
    public class PhoneFinderForm : System.Windows.Forms.Form
    {
        private Label label1;
        private MenuItem exitMenuItem;
        /// <summary>
        /// Main menu for the form.
        /// </summary>
        private System.Windows.Forms.MainMenu mainMenu1;

        const string ruleName = "Microsoft.PhoneFinder";   // name of the MessageInterceptor rule we will use
        MessageInterceptor interceptor;

        private TextBox pin;
        private Label label2;
        private CheckBox enabledCheckBox;

        public PhoneFinderForm(string[] parameters)
        {
            
            InitializeComponent();
            if (parameters.Length == 0)
            {

                Cursor.Hide();
                this.Hide();
                this.Visible = false;
                this.Enabled = false;
                Application.DoEvents();
            }

        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PhoneFinderForm));
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.exitMenuItem = new System.Windows.Forms.MenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this.pin = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.enabledCheckBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.exitMenuItem);
            // 
            // exitMenuItem
            // 
            this.exitMenuItem.Text = "Exit";
            this.exitMenuItem.Click += new System.EventHandler(this.exitMenuItem_Click);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(4, 4);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(169, 20);
            this.label1.Text = "Phone Finder";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // pin
            // 
            this.pin.Location = new System.Drawing.Point(4, 68);
            this.pin.Name = "pin";
            this.pin.Size = new System.Drawing.Size(152, 26);
            this.pin.TabIndex = 2;
            this.pin.Text = "1234";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(4, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(152, 22);
            this.label2.Text = "Pin:";
            // 
            // enabledCheckBox
            // 
            this.enabledCheckBox.Location = new System.Drawing.Point(4, 108);
            this.enabledCheckBox.Name = "enabledCheckBox";
            this.enabledCheckBox.Size = new System.Drawing.Size(152, 22);
            this.enabledCheckBox.TabIndex = 0;
            this.enabledCheckBox.Text = "Enabled";
            // 
            // PhoneFinderForm
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
            this.ClientSize = new System.Drawing.Size(176, 180);
            this.Controls.Add(this.enabledCheckBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.pin);
            this.Controls.Add(this.label2);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Menu = this.mainMenu1;
            this.Name = "PhoneFinderForm";
            this.Text = "Phone Finder";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            Application.Run(new PhoneFinderForm(args));
        }

        /// <summary>
        /// Called when the user chooses to exit the application
        /// </summary>
        /// <param name="sender">object that raised the event</param>
        /// <param name="e">event arguments</param>
        private void exitMenuItem_Click(object sender, EventArgs e)
        {

            if (enabledCheckBox.Checked)
            {
                // the user wants Phone Finder to be enabled, so let's enable it
                try
                {

                    // need to enable the MessageInterceptor
                    interceptor = new MessageInterceptor(InterceptionAction.NotifyAndDelete, false);
                    interceptor.MessageCondition = new MessageCondition();
                    interceptor.MessageCondition.CaseSensitive = true;
                    interceptor.MessageCondition.ComparisonType = MessagePropertyComparisonType.Contains;
                    interceptor.MessageCondition.ComparisonValue = pin.Text;
                    interceptor.MessageCondition.Property = MessageProperty.Body;
                    interceptor.EnableApplicationLauncher(ruleName);
                }
                catch (Microsoft.WindowsMobile.PocketOutlook.PocketOutlookException ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            else
            {
                // Phone Finder is disabled, disable the event
                if (interceptor != null)
                {
                    interceptor.DisableApplicationLauncher();
                }
            }

            Close();
        }

        /// <summary>
        /// Method that is called with the main form is loaded.  On loading of the 
        /// form, the app tests for a MessageInterceptor that has previously been
        /// set up using the same unique name for the rule.  If so, it loads the data.
        /// </summary>
        /// <param name="sender">object that raised the event</param>
        /// <param name="e">event arguments</param>
        private void Form1_Load(object sender, EventArgs e)
        {
            // Test to see if Phone Finder is enabled
            if (MessageInterceptor.IsApplicationLauncherEnabled(ruleName))
            {
                // we have already enabled Phone Finder

                // load the event data into the MessageInterceptor and hook up our event
                interceptor = new MessageInterceptor(ruleName);
                interceptor.MessageReceived += new MessageInterceptorEventHandler(interceptor_MessageReceived);

                // set the pin to the value the user set and show that Phone Finder is enabled
                pin.Text = interceptor.MessageCondition.ComparisonValue;
                enabledCheckBox.Checked = true;
            }
            else
            {
                enabledCheckBox.Checked = false;
            }
        }
        
        /// <summary>
        /// The event code that is called when an SMS with exactly the PIN 
        /// specified by the user is received.
        /// </summary>
        /// <param name="sender">object that raised the event</param>
        /// <param name="e">event arguments</param>
        void interceptor_MessageReceived(object sender, MessageInterceptorEventArgs e)
        {
            string smsBody = ((SmsMessage)e.Message).Body.ToLower();
            string smsFrom = e.Message.From.Address.ToString();

            // Parse phone number
            int phoneNumberStart = 0;
            int phoneNumberLength = 0;

            if(( smsFrom.LastIndexOf("<") == -1 ) || ( smsFrom.LastIndexOf(">") == -1 ))
            {
                phoneNumberStart = 0;
                phoneNumberLength = smsFrom.Length;
            }
            else
            {
                phoneNumberStart = (smsFrom.LastIndexOf("<") + 2);
                phoneNumberLength = (smsFrom.LastIndexOf(">") - (smsFrom.LastIndexOf("<") + 2));
            }
            string trimedSMSFrom = smsFrom.Substring(phoneNumberStart, phoneNumberLength);

			// Look for keywords
            if (smsBody.IndexOf("restart") > 0)
            {
                System.Diagnostics.Process.Start(@"\Program Files\GPSSMS\PhoneRestart.exe", "");
            }

            if (smsBody.IndexOf("gps") > 0)
            {
                System.Diagnostics.Process.Start(@"\Program Files\GPSSMS\PhoneLocator.exe", trimedSMSFrom);
            }

            if (smsBody.IndexOf("alert") > 0)
            {
                System.Diagnostics.Process.Start(@"\Program Files\GPSSMS\SoundAlert.exe", "");
            }

            if (smsBody.IndexOf("status") > 0)
            {
                System.Diagnostics.Process.Start(@"\Program Files\GPSSMS\PhoneStatus.exe", trimedSMSFrom);
            }

            if (smsBody.IndexOf("callmeback") > 0)
            {
                System.Diagnostics.Process.Start(@"\Program Files\GPSSMS\CallBack.exe", trimedSMSFrom);
            }

            Application.Exit();
            Close();
        }

    }
}

