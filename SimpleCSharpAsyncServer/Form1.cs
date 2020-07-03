using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Windows.Forms;

namespace SimpleCSharpAsyncServer
{
    public partial class Form1 : Form
    {
        SLCPServer slcp;

        public Form1()
        {
            InitializeComponent();

            this.FormClosed += new FormClosedEventHandler(Form1_Closed);
            Thread t = new Thread(Run);
            t.IsBackground = true;
            t.Start();
        }

        private void Run()
        {
            SLCPServer.DataReceiveEvent += dataReceiveEvent;
            slcp = new SLCPServer(10101);
            slcp.run();
        }

        private void dataReceiveEvent(string data)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new DataReceiveHandler(dataReceiveEvent), data);
                return;
            }

            logConsole.AppendText("> " + data + "\r\n");

            if (logConsole.Lines.Length > 50)
            {
                LinkedList<string> lines = new LinkedList<string>(logConsole.Lines);
                while (lines.Count > 50)
                    lines.RemoveFirst();

                logConsole.Lines = lines.ToArray();

            }

            logConsole.Select(logConsole.Text.Length, 0);
            logConsole.ScrollToCaret();
        }

    private void Form1_Closed(object sender, System.EventArgs e)
        {
            MessageBox.Show("Server closed", "Alert");
            slcp.Stop();
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            if (logDeletion.Checked == true)
                Policy.logCreationPolicy = "1";
            else
                Policy.logCreationPolicy = "0";

            if (backupStorage.Checked == true)
                Policy.backupFileStorage = "1";
            else
                Policy.backupFileStorage = "0";

            slcp.SendPolicyAll();
        }
    }
}
