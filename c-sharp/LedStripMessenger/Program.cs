using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LedStripMessenger
{
    class Program
    {
        static void Main(string[] args)
        {
            LedStripMessenger messenger = new LedStripMessenger { RunLoop = true };
            messenger.Setup();
            while (messenger.RunLoop) messenger.Loop();
            messenger.Exit();
        }
    }
}
