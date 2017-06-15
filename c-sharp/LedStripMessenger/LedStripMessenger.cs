// *** SendandReceiveBinaryArguments ***

// This example expands the previous SendandReceiveArguments example. The PC will 
//  send and receive multiple Binary values, demonstrating that this is more compact and faster. Since the output is not human readable any more, 
//  the logging is disabled and the NewLines are removed
//
// It adds a demonstration of how to:
// - Receive multiple binary parameters,
// - Send multiple binary parameters
// - Callback events being handled while the main program waits
// - How to calculate milliseconds, similar to Arduino function Millis()

using System;
using System.Threading;
using CommandMessenger;
using CommandMessenger.Transport.Serial;

namespace LedStripMessenger
{
    // This is the list of recognized commands. These can be commands that can either be sent or received. 
    // In order to receive, attach a callback function to these events
    enum LedStripCommands
    {
        Ping,
        Pong,
        Unknown,
        Acknowledge,
        Error,
        SetPixel,
        FillSolid,
    };

    public class LedStripMessenger
    {
        public bool RunLoop { get; set; }
        private SerialTransport serialTransport;
        private CmdMessenger cmdMessenger;

        // ------------------ M A I N  ----------------------

        // Setup function
        public void Setup()
        {
            // Create Serial Port object
            serialTransport = new SerialTransport
            {
                CurrentSerialSettings = { PortName = "COM6", BaudRate = 9600 } // object initializer
            };

            // Initialize the command messenger with the Serial Port transport layer
            // Set if it is communicating with a 16- or 32-bit Arduino board
            cmdMessenger = new CmdMessenger(serialTransport, BoardType.Bit16);

            // Attach the callbacks to the Command Messenger
            AttachCommandCallBacks();

            // Start listening
            cmdMessenger.Connect();
            
            // Clear queues 
            cmdMessenger.ClearReceiveQueue();
            cmdMessenger.ClearSendQueue();
            
            var pingCommand = new SendCommand((int)LedStripCommands.Ping);
            cmdMessenger.SendCommand(pingCommand);
            //cmdMessenger.SendCommand(pingCommand);

            Thread.Sleep(2000);

            for (byte j = 0; j < 10; j++)
            {
                double avgTime = 0;
                for (byte i = 0; i < 60; i++)
                {
                    DateTime t1 = DateTime.Now;
                    /*
                    var fillSolid = new SendCommand((int)LedStripCommands.FillSolid);
                    fillSolid.AddBinArgument(false);
                    fillSolid.AddBinArgument(i);
                    fillSolid.AddBinArgument(i);
                    fillSolid.AddBinArgument(i);
                    fillSolid.ReqAc = true;
                    cmdMessenger.SendCommand(fillSolid);
                    */
                    var setPixel = new SendCommand((int)LedStripCommands.SetPixel);
                    setPixel.AddBinArgument(false);
                    setPixel.AddBinArgument(i);
                    setPixel.AddBinArgument((byte)(255 / 10 * j));
                    setPixel.AddBinArgument((byte)(255 / 10 * j));
                    setPixel.AddBinArgument((byte)(255 / 10 * j));
                    setPixel.ReqAc = false;
                    cmdMessenger.SendCommandSync(setPixel, SendQueue.WaitForEmptyQueue);
                    //cmdMessenger.SendCommand(setPixel);
                    
                    Thread.Sleep(10);
                    DateTime t2 = DateTime.Now;
                    avgTime += (t2 - t1).TotalMilliseconds;
                }
                Console.WriteLine(avgTime / 60.0);
            }
            
            Thread.Sleep(1000);
        }

        // Loop function
        public void Loop()
        {
            RunLoop = false;
        }

        // Exit function
        public void Exit()
        {
            // Stop listening
            cmdMessenger.Disconnect();

            // Dispose Command Messenger
            cmdMessenger.Dispose();

            // Dispose Serial Port object
            serialTransport.Dispose();

            // Pause before stop
            Console.WriteLine("Press any key to stop...");
            Console.ReadKey();
        }

        /// Attach command call backs. 
        private void AttachCommandCallBacks()
        {
            cmdMessenger.Attach(OnUnknown);
            cmdMessenger.Attach((int)LedStripCommands.Unknown, OnUnknown);
            cmdMessenger.Attach((int)LedStripCommands.Acknowledge, OnAcknowledge);
            cmdMessenger.Attach((int)LedStripCommands.Error, OnError);
            cmdMessenger.Attach((int)LedStripCommands.Pong, OnPong);
        }
        
        // ------------------  C A L L B A C K S ---------------------
        private void OnUnknown(ReceivedCommand arguments)
        {
            if (arguments.Available())
            {
                var unknownCommand = (LedStripCommands)arguments.ReadBinInt16Arg();
                Console.WriteLine(String.Format("The command \"{0}\"({1}) was unknown to the arduino.", unknownCommand.ToString(), (int)unknownCommand));
            }
            else
            {
                Console.WriteLine("Command without attached callback received.");
            }
        }

        private void OnAcknowledge(ReceivedCommand receivedCommand)
        {
            LedStripCommands acknowledgedCommand = (LedStripCommands)receivedCommand.ReadBinInt16Arg();
            Console.WriteLine(String.Format("Command \"{0}\"({1}) has been acknowledged.", acknowledgedCommand.ToString(), (int)acknowledgedCommand));
        }

        private void OnError(ReceivedCommand receivedCommand)
        {
            Console.Write("CmdError: ");
            var CmdId = (LedStripCommands)receivedCommand.ReadBinInt16Arg();

            if (CmdId == LedStripCommands.Unknown)
            {
                CmdId = (LedStripCommands)receivedCommand.ReadBinInt16Arg();
                Console.WriteLine(String.Format("\"{0}\"({1}) Command is unknown to the Arduino.", CmdId.ToString(), (int)CmdId));
            }
            else if (CmdId == LedStripCommands.Acknowledge)
            {
                Console.WriteLine("Arduino received Acknowledge command, which is invalid.");
            }
            else
            {
                Console.WriteLine(CmdId.ToString());
            }
        }

        private void OnPong(ReceivedCommand receivedCommand)
        {
            Console.WriteLine("Received pong from the arduino.");
        }
    }
}
