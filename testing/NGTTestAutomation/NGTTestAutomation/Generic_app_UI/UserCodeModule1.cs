/*
 * Created by Ranorex
 * User: francoism
 * Date: 22/03/2016
 * Time: 12:10 PM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
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

namespace NGTTestAutomation.Generic_app_UI
{
    /// <summary>
    /// Description of UserCodeModule1.
    /// </summary>
    [TestModule("92F24632-328E-48AC-A791-EE0CF26BBC68", ModuleType.UserCode, 1)]
    public class UserCodeModule1 : ITestModule
    {
        /// <summary>
        /// Constructs a new instance.
        /// </summary>
        public UserCodeModule1()
        {
            // Do not delete - a parameterless constructor is required!
        }

        /// <summary>
        /// Performs the playback of actions in this module.
        /// </summary>
        /// <remarks>You should not call this method directly, instead pass the module
        /// instance to the <see cref="TestModuleRunner.Run(ITestModule)"/> method
        /// that will in turn invoke this method.</remarks>
        void ITestModule.Run()
        {
        			  
		 // Create PopupWatcher  
		 PopupWatcher myPopupWatcher = new PopupWatcher();  
		   
		 // Add a Watch using a RanoreXPath and triggering the Method CloseUpdateCheckDialog  
		// myPopupWatcher.Watch("/form[@controlname='UpdateCheckForm']/button[@controlname='m_btnClose']", CloseUpdateCheckDialog);  
		 Report.Info("???---Start---"); 
		myPopupWatcher.Watch("/form[@title='generic_app.exe']", CloseUpdateCheckDialog);  
		  
		/*Thread dialogWatcher = new Thread(ClosePopUpDialogs);
            dialogWatcher.IsBackground = true;
			dialogWatcher.SetApartmentState(ApartmentState.STA);
			dialogWatcher.Start();*/
		 // Add a Watch using the info object of a button and triggering the Method CloseUpdateCheckDialog  
		 // myPopupWatcher.Watch(repo.UpdateCheckDialog.btCloseInfo, CloseUpdateCheckDialog);  
		   
		 // Add a Watch using the info object of the dialog and the info object of the button to click  
		 // myPopupWatcher.WatchAndClick(repo.UpdateCheckDialog.SelfInfo, repo.UpdateCheckDialog.btCloseInfo);  
		   
		 // Add a Watch using a repository folder object and the info object of the button to click  
		 // myPopupWatcher.WatchAndClick(repo.UpdateCheckDialog, repo.UpdateCheckDialog.btCloseInfo);  
		  
		 // Start PopupWatcher  
		 myPopupWatcher.Start();  
	 
          //  Mouse.DefaultMoveTime = 300;
          //  Keyboard.DefaultKeyPressTime = 100;
          //  Delay.SpeedFactor = 1.0;
        }
        
        public static void CloseUpdateCheckDialog(Ranorex.Core.Repository.RepoItemInfo myInfo, Ranorex.Core.Element myElement)  
		{  
		 	Report.Info("---Found an error1"); 
		}  
		  
		public static void CloseUpdateCheckDialog(Ranorex.Core.RxPath myPath, Ranorex.Core.Element myElement)  
		{  
			 Report.Info("---Found an error2"); 
		}  
		
		public static void ClosePopUpDialogs()
		{
        	NGTTestAutomationRepository repo = NGTTestAutomationRepository.Instance;
			while (true)
			{
				if (repo.crash_hasstoppedworking.Visible)
				//if (repo.Dialog_Unknown.SelfInfo.Exists() )
				{
					Report.Info("??????????????????????????????????Unknown Exception occurred");
					//Report.Screenshot(repo.Dialog_Unknown.Self);
					// Stop the running test with exit code “-1”
					Environment.Exit(-1);
				}
			}
		}
    }
}
