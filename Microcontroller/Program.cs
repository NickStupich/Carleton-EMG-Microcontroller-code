using System;
using Microsoft.SPOT;
using System.IO.Ports;
using GHIElectronics.NETMF.Native;
using System.Threading;
using GHIElectronics.NETMF.Hardware;

using Microsoft.SPOT.Hardware;

namespace ComputerBluetoothNative
{
    public class Program
    {
        static SerialPort debugPort;
        static SerialPort dataPort;

        //static RLP.Procedure rlpStartAnalogRead;
        static RLP.Procedure rlpStop;
        static RLP.Procedure rlpStart;
        static RLP.Procedure rlpInit;
        //static RLP.Procedure rlpTest;

        const int START_BIT = 7;

        public static void Main()
        {

            RLP.Enable();
            // Unlock RLP
            RLP.Unlock("NICK.STUPICH@GMAIL.COM5B806A336D", new byte[] { 0x60, 0xEE, 0xDB, 0x53, 0x8A, 0x31, 0x71, 0x34, 0xAE, 0xB5, 0x6A, 0x4A, 0xA8, 0x54, 0x10, 0xA4, 0x67, 0x24, 0xFA, 0x0B, 0x8A, 0x4C, 0x8A, 0x78, 0x27, 0x1F, 0xBF, 0x59, 0xC3, 0x21, 0x54, 0x1A });
            Debug.EnableGCMessages(false);

            Debug.GC(true);
            byte[] elf_file = Resources.GetBytes(Resources.BinaryResources.RLP_test);
            RLP.LoadELF(elf_file);

            RLP.InitializeBSSRegion(elf_file);

            //rlpTest = RLP.GetProcedure(elf_file, "Test");
            rlpStop = RLP.GetProcedure(elf_file, "Stop");
            rlpStart = RLP.GetProcedure(elf_file, "Start");
            rlpInit = RLP.GetProcedure(elf_file, "Init");
            elf_file = null;
            Debug.GC(true);

            //sets up all analog pins as reading
            for (int i = 0; i < 6; i++)
            {
                new AnalogIn((AnalogIn.Pin)i);
            }

            debugPort = new SerialPort("COM2", 9600, System.IO.Ports.Parity.None, 8, StopBits.One);
            debugPort.Open();

            dataPort = new SerialPort("COM1", 57600, System.IO.Ports.Parity.None, 8, StopBits.One);
            dataPort.DataReceived += new SerialDataReceivedEventHandler(dataPort_DataReceived);
            dataPort.Open();

            Thread.Sleep(1000); //this makes it easier to install new stuff

            rlpInit.Invoke();
            
            int result = rlpStart.Invoke(1 | 2 | 4 | (1<<7));
            Debug.Print("Start return value: " + result);
            Thread.Sleep(1000);
            result = rlpStop.Invoke();
            Debug.Print("Stop return value: " + result);
            
            
            //int result = rlpStart.Invoke();
            //Debug.Print("result of start: " + result);
            Thread.Sleep(Timeout.Infinite);

        }

        static void dataPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] buffer = new byte[1];
            while (dataPort.BytesToRead > 0)
            {
                dataPort.Read(buffer, 0, 1);
                byte data = buffer[0];
                /*
                switch ((char)data)
                {
                    case 's':
                    case 'S':
                        int r1 = rlpStop.Invoke();
                        Debug.Print("Stopped with result " + r1);
                        break;
                    case 'b':
                    case 'B':
                        int r2 = rlpStart.Invoke();
                        Debug.Print("Started with result " + r2);
                        break;
                    default:
                        Debug.Print("unknown letter received");
                        Debug.Print(new String(new char[] { (char)data }));
                        break;
                }*/

                int result;
                switch (data & (1 << START_BIT))
                {
                    case 0:
                        result = rlpStop.Invoke();
                        Debug.Print("Stopped with result " + result);
                        break;
                    case (1<<START_BIT):
                        result = rlpStart.Invoke(data);
                        Debug.Print("Started with result " + result);
                        break;
                }

            }


        }

    }
}
