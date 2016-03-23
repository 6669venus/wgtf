/*
 * Created by Ranorex
 * User: adamm
 * Date: 2/03/2016
 * Time: 10:42 AM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */

using System;
using System.Threading;
using System.Drawing;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using WinForms = System.Windows.Forms;

using Ranorex;
using Ranorex.Core;
using Ranorex.Core.Reporting;
using Ranorex.Core.Testing;
//using Ranorex.PopupWatcher;


	
namespace NGTTestAutomation
{
    class Program
    {
        [STAThread]
        public static int Main(string[] args)
        {
        	 
            // Uncomment the following 2 lines if you want to automate Windows apps
            // by starting the test executable directly
            //if (Util.IsRestartRequiredForWinAppAccess)
            //    return Util.RestartWithUiAccess();

            Keyboard.AbortKey = System.Windows.Forms.Keys.Pause;
            int error = 0;
            
           /* Report.Info("Start simulating pop ups by pressing the 'Start' button.");
			repo.AppDialogSimulator.ButtonStart.Click();

            Thread dialogWatcher = new Thread(ClosePopUpDialogs);
            dialogWatcher.IsBackground = true;
			dialogWatcher.SetApartmentState(ApartmentState.STA);
			dialogWatcher.Start();*/

            /*Thread dialogWatcher = new Thread(ClosePopUpDialogs);
            dialogWatcher.IsBackground = true;
			dialogWatcher.SetApartmentState(ApartmentState.STA);
			dialogWatcher.Start();*/

            try
            {
                error = TestSuiteRunner.Run(typeof(Program), Environment.CommandLine);
            }
            catch (Exception e)
            {
                Report.Error("Unexpected exception occurred: " + e.ToString());
                error = -1;
            }
            return error;         
            
        }
       /* public static void ClosePopUpDialogs()
		{
        	NGTTestAutomationRepository repo = NGTTestAutomationRepository.Instance;
			while (true)
			{
				if (repo.crash_hasstoppedworking.Visible)
				//if (repo.Dialog_Unknown.SelfInfo.Exists() )
				{
					Report.Info("Unknown Exception occurred");
					//Report.Screenshot(repo.Dialog_Unknown.Self);
					// Stop the running test with exit code “-1”
					Environment.Exit(-1);
				}
			}
		}*/
        
       
        
       
        
    }
}
