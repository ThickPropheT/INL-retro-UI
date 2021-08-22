using INL.Retro.Ui.Infrastructure;
using System;
using System.Windows;

namespace INL.Retro.Ui
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// 
        /// [WATCH ITEM]
        /// This approach to providing custom bootstrapping logic via CompositionRoot
        /// MAY present issues in the future. Some advice (see 1st link below) suggests
        /// that these problems can be mitigated. Other advice (see 2nd link below)
        /// indicates further research may be required. YMMV.
        /// 
        /// Tutorial for implementing this approach:
        /// https://ludovic.chabant.com/devblog/2010/04/20/writing-a-custom-main-method-for-wpf-applications/
        /// 
        /// Additional advice for this approach:
        /// https://stackoverflow.com/a/6156665/1296733
        /// 
        /// </summary>
        [STAThread]
        public static void Main()
        {
            var app = new App();
            app.Run(CompositionRoot.Compose());
        }

        public App()
        {
            InitializeComponent();
        }
    }
}
