﻿///////////////////////////////////////////////////////////////////////////////
//
// This file was automatically generated by RANOREX.
// DO NOT MODIFY THIS FILE! It is regenerated by the designer.
// All your modifications will be lost!
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
using Ranorex.Core.Repository;

namespace NGTTestAutomation.Generic_app_UI
{
#pragma warning disable 0436 //(CS0436) The type 'type' in 'assembly' conflicts with the imported type 'type2' in 'assembly'. Using the type defined in 'assembly'.
    /// <summary>
    ///The LFT_ScrollBar recording.
    /// </summary>
    [TestModule("0cce184d-3ea9-46d8-91b7-56559d1f9d2e", ModuleType.Recording, 1)]
    public partial class LFT_ScrollBar : ITestModule
    {
        /// <summary>
        /// Holds an instance of the NGTTestAutomation.NGTTestAutomationRepository repository.
        /// </summary>
        public static NGTTestAutomation.NGTTestAutomationRepository repo = NGTTestAutomation.NGTTestAutomationRepository.Instance;

        static LFT_ScrollBar instance = new LFT_ScrollBar();

        /// <summary>
        /// Constructs a new instance.
        /// </summary>
        public LFT_ScrollBar()
        {
        }

        /// <summary>
        /// Gets a static instance of this recording.
        /// </summary>
        public static LFT_ScrollBar Instance
        {
            get { return instance; }
        }

#region Variables

#endregion

        /// <summary>
        /// Starts the replay of the static recording <see cref="Instance"/>.
        /// </summary>
        [System.CodeDom.Compiler.GeneratedCode("Ranorex", "5.4.5")]
        public static void Start()
        {
            TestModuleRunner.Run(Instance);
        }

        /// <summary>
        /// Performs the playback of actions in this recording.
        /// </summary>
        /// <remarks>You should not call this method directly, instead pass the module
        /// instance to the <see cref="TestModuleRunner.Run(ITestModule)"/> method
        /// that will in turn invoke this method.</remarks>
        [System.CodeDom.Compiler.GeneratedCode("Ranorex", "5.4.5")]
        void ITestModule.Run()
        {
            Mouse.DefaultMoveTime = 300;
            Keyboard.DefaultKeyPressTime = 100;
            Delay.SpeedFactor = 1.0;

            Init();

            Report.Log(ReportLevel.Info, "Mouse", "Mouse Left Down item 'MainWindow.LFT_panel.VerticalScrollBar' at 6;16.", repo.MainWindow.LFT_panel.VerticalScrollBarInfo, new RecordItemIndex(0));
            repo.MainWindow.LFT_panel.VerticalScrollBar.MoveTo("6;16");
            Mouse.ButtonDown(System.Windows.Forms.MouseButtons.Left);
            Delay.Milliseconds(200);
            
            Report.Log(ReportLevel.Info, "Mouse", "Mouse Left Up item 'MainWindow.LFT_panel.VerticalScrollBar' at 6;963.", repo.MainWindow.LFT_panel.VerticalScrollBarInfo, new RecordItemIndex(1));
            repo.MainWindow.LFT_panel.VerticalScrollBar.MoveTo("6;963");
            Mouse.ButtonUp(System.Windows.Forms.MouseButtons.Left);
            Delay.Milliseconds(200);
            
            try {
                Report.Log(ReportLevel.Info, "Validation", "(Optional Action)\r\nValidating CompareImage (Screenshot: 'Screenshot3' with region {X=0,Y=0,Width=1174,Height=922}) on item 'MainWindow.LFT_panel.List'.", repo.MainWindow.LFT_panel.ListInfo, new RecordItemIndex(2));
                Validate.CompareImage(repo.MainWindow.LFT_panel.ListInfo, List_Screenshot3, List_Screenshot3_Options, Validate.DefaultMessage, false);
                Delay.Milliseconds(0);
            } catch(Exception ex) { Report.Log(ReportLevel.Warn, "Module", "(Optional Action) " + ex.Message, new RecordItemIndex(2)); }
            
            Report.Log(ReportLevel.Info, "Mouse", "Mouse Left Down item 'MainWindow.LFT_panel.VerticalScrollBar' at 7;953.", repo.MainWindow.LFT_panel.VerticalScrollBarInfo, new RecordItemIndex(3));
            repo.MainWindow.LFT_panel.VerticalScrollBar.MoveTo("7;953");
            Mouse.ButtonDown(System.Windows.Forms.MouseButtons.Left);
            Delay.Milliseconds(200);
            
            Report.Log(ReportLevel.Info, "Mouse", "Mouse Left Move item 'MainWindow.LFT_panel.VerticalScrollBar' at 15;945.", repo.MainWindow.LFT_panel.VerticalScrollBarInfo, new RecordItemIndex(4));
            repo.MainWindow.LFT_panel.VerticalScrollBar.MoveTo("15;945");
            Delay.Milliseconds(200);
            
            Report.Log(ReportLevel.Info, "Mouse", "Mouse Left Up item 'MainWindow.LFT_panel.WGTextBox' at 1135;10.", repo.MainWindow.LFT_panel.WGTextBoxInfo, new RecordItemIndex(5));
            repo.MainWindow.LFT_panel.WGTextBox.MoveTo("1135;10");
            Mouse.ButtonUp(System.Windows.Forms.MouseButtons.Left);
            Delay.Milliseconds(200);
            
            try {
                Report.Log(ReportLevel.Info, "Validation", "(Optional Action)\r\nValidating CompareImage (Screenshot: 'Screenshot6' with region {X=0,Y=0,Width=1174,Height=922}) on item 'MainWindow.LFT_panel.List'.", repo.MainWindow.LFT_panel.ListInfo, new RecordItemIndex(6));
                Validate.CompareImage(repo.MainWindow.LFT_panel.ListInfo, List_Screenshot6, List_Screenshot6_Options, Validate.DefaultMessage, false);
                Delay.Milliseconds(0);
            } catch(Exception ex) { Report.Log(ReportLevel.Warn, "Module", "(Optional Action) " + ex.Message, new RecordItemIndex(6)); }
            
        }

#region Image Feature Data
        CompressedImage List_Screenshot3
        { get { return repo.MainWindow.LFT_panel.ListInfo.GetScreenshot3(new Rectangle(0, 0, 1174, 922)); } }

        Imaging.FindOptions List_Screenshot3_Options
        { get { return Imaging.FindOptions.Parse("0.8;EdgesSobel;0,0,1174,922;True;10000000;0ms"); } }

        CompressedImage List_Screenshot6
        { get { return repo.MainWindow.LFT_panel.ListInfo.GetScreenshot6(new Rectangle(0, 0, 1174, 922)); } }

        Imaging.FindOptions List_Screenshot6_Options
        { get { return Imaging.FindOptions.Parse("0.8;EdgesSobel;0,0,1174,922;True;10000000;0ms"); } }

#endregion
    }
#pragma warning restore 0436
}