using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using System.Threading;
using System.Diagnostics;
using Windows.UI;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI.Xaml;
using Win2D_YUV;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Sample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        CanvasDevice device;

        CanvasSwapChain swapChain;

        CanvasRenderTarget renderTarget;

        byte[] dataY;

        byte[] dataU;

        byte[] dataV;

        Timer updateTimer;

        double timestamp = 0;

        UInt16 countsOfFPS = 0;

        UInt16 fpsRate = 0;

        Random rnd = new Random();

        public MainPage()
        {
            this.InitializeComponent();

            byte[] source = File.ReadAllBytes("sample.yuv");

            dataY = source.Skip(0).Take(4000 * 3000).ToArray();

            dataU = source.Skip(4000 * 3000).Take(2000 * 1500).ToArray();

            dataV = source.Skip(4000 * 3000 + 2000 * 1500).Take(2000 * 1500).ToArray();

            source = null;

            GC.Collect();

            this.device = CanvasDevice.GetSharedDevice();
        }

        private void CanvasSwapChainPanel_Loaded(object sender, RoutedEventArgs e)
        {
            var swapChainPanel = sender as CanvasSwapChainPanel;

            this.swapChain = new CanvasSwapChain(this.device, (int)swapChainPanel.ActualWidth, (int)swapChainPanel.ActualHeight, 96);

            swapChainPanel.SwapChain = this.swapChain;

            this.renderTarget = new CanvasRenderTarget(this.device, (float)this.swapChain.Size.Width, (float)this.swapChain.Size.Height, 96);

            this.draw();

            TimerCallback tcb = this.update;

            this.updateTimer = new Timer(tcb, null, TimeSpan.FromTicks(333333), TimeSpan.FromMilliseconds(-1));
        }

        private void CanvasSwapChainPanel_Unloaded(object sender, RoutedEventArgs e)
        {

        }

        private void CanvasSwapChainPanel_CompositionScaleChanged(SwapChainPanel sender, object args)
        {
            if (this.swapChain == null) { return; }

            this.draw();
        }

        private void CanvasSwapChainPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (this.swapChain == null) { return; }

            this.swapChain.ResizeBuffers(e.NewSize);

            this.draw();
        }

        private void update(object stateInfo)
        {
            Stopwatch sw = new Stopwatch();

            sw.Start();

            using (var session = this.renderTarget.CreateDrawingSession())
            {
                YUVDrawSession.SharedSession.DrawImage(session, this.dataY, this.dataU, this.dataV, 4000, 3000);
            }

            this.draw();

            timestamp += sw.ElapsedMilliseconds;

            if (this.timestamp >= 1000)
            {
                this.fpsRate = this.countsOfFPS;

                this.countsOfFPS = 0;

                this.timestamp = 0;
            }

            this.updateTimer.Change(0, Timeout.Infinite);
        }

        private void draw()
        {
            this.countsOfFPS++;

            using (var session = this.swapChain.CreateDrawingSession(Colors.Black))
            {
                session.Antialiasing = CanvasAntialiasing.Aliased;

                if (this.renderTarget != null)
                {
                    session.DrawImage(this.renderTarget, new Rect(0, 0, this.swapChain.Size.Width, this.swapChain.Size.Height));
                }

                session.DrawText("fps: " + this.fpsRate, 0, 0, Colors.Red);
            }

            this.swapChain.Present();
        }
    }
}
