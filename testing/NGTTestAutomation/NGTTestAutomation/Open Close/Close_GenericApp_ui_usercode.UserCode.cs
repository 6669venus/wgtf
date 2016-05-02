﻿///////////////////////////////////////////////////////////////////////////////
//
// This file was automatically generated by RANOREX.
// Your custom recording code should go in this file.
// The designer will only add methods to this file, so your custom code won't be overwritten.
// http://www.ranorex.com
// 
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Drawing;
using System.Threading;
using WinForms = System.Windows.Forms;

using Ranorex;
using Ranorex.Core;
using Ranorex.Core.Testing;

namespace NGTTestAutomation.Open_Close
{
    public partial class Close_GenericApp_ui_usercode
    {
        /// <summary>
        /// This method gets called right after the recording has been started.
        /// It can be used to execute recording specific initialization code.
        /// </summary>
        private void Init()
        {
            // Your recording specific initialization code goes here.
        }

        public void Close_Application_MainWindow()
        { //repo.Dialog_Unknown.Visible
        	Report.Info("????????Close_Application_MainWindo?");
		
        	if (repo.crash_hasstoppedworking.Visible)
        	{
        		Report.Info("Unknown Exception occurred");
        		//Report.Screenshot(repo.Dialog_Unknown.CaptureCompressedImage());
				// Stop the running test with exit code “-1”
				Environment.Exit(-1);
        	}
        	
            Report.Log(ReportLevel.Info, "Application", "Closing application containing item 'Generic_app.MainWindow'.", repo.Generic_app.MainWindow.SelfInfo);
            Host.Local.CloseApplication(repo.Generic_app.MainWindow.Self, 30000);
        }


    }
}