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

namespace NGTTestAutomation
{
#pragma warning disable 0436 //(CS0436) The type 'type' in 'assembly' conflicts with the imported type 'type2' in 'assembly'. Using the type defined in 'assembly'.
    /// <summary>
    ///The OpenGenericApp recording.
    /// </summary>
    [TestModule("3a17130a-ec04-4660-a6a2-a4be9e4d2244", ModuleType.Recording, 1)]
    public partial class OpenGenericApp : ITestModule
    {
        /// <summary>
        /// Holds an instance of the NGTTestAutomationRepository repository.
        /// </summary>
        public static NGTTestAutomationRepository repo = NGTTestAutomationRepository.Instance;

        static OpenGenericApp instance = new OpenGenericApp();

        /// <summary>
        /// Constructs a new instance.
        /// </summary>
        public OpenGenericApp()
        {
            path = "";
        }

        /// <summary>
        /// Gets a static instance of this recording.
        /// </summary>
        public static OpenGenericApp Instance
        {
            get { return instance; }
        }

#region Variables

        string _path;

        /// <summary>
        /// Gets or sets the value of variable path.
        /// </summary>
        [TestVariable("50d4a791-38d2-4dcf-b6a4-7ee327b91776")]
        public string path
        {
            get { return _path; }
            set { _path = value; }
        }

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

            Report.Log(ReportLevel.Info, "Application", "Run application 'generic_app.exe' with arguments '--config plugins_ui.txt' in normal mode.", new RecordItemIndex(0));
            Host.Local.RunApplication("generic_app.exe", "--config plugins_ui.txt", path, false);
            Delay.Milliseconds(0);
            
        }

#region Image Feature Data
#endregion
    }
#pragma warning restore 0436
}