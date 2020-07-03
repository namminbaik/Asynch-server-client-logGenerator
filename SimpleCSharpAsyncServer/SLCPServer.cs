using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Threading;

namespace SimpleCSharpAsyncServer
{
    class StateObject
    {
        public Socket worker = null;
        public byte[] buffer = new byte[SocketConstants.BufferSize];
        public StringBuilder sb = new StringBuilder();
    }

    public delegate void DataReceiveHandler(string data);

    class SLCPServer
    {
        private Socket listener;
        private int Port;
        private List<Socket> clientSockets;

        public static event DataReceiveHandler DataReceiveEvent;

        public SLCPServer(int port)
        {
            Port = port;
            clientSockets = new List<Socket>();
            Policy.logCreationPolicy = "1";
            Policy.backupFileStorage = "0";
        }

        public void run()
        {
            Listen();
        }

        private void Listen()
        {
            try
            {
                IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
                IPAddress ipAddress = IPAddress.Parse("127.0.0.1");
                listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                listener.Bind(new IPEndPoint(ipAddress, Port));
                listener.Listen(SocketConstants.BackLog);

                DataReceiveEvent(string.Format("Server Started in {0}:{1}", ipAddress, Port));
                Accept();
            }
            catch (Exception e)
            {
                DataReceiveEvent("Failed to listen through the socket...");
            }
        }

        private void Accept()
        {
            while (true)
            {
                try
                {
                    Socket handler = listener.Accept();
                    DataReceiveEvent(string.Format("{0} is connected ...", handler.RemoteEndPoint.ToString()));
                    clientSockets.Add(handler);

                    StateObject state = new StateObject();
                    state.worker = handler;
                    handler.BeginReceive(state.buffer, 0, SocketConstants.BufferSize, 0,
                        new AsyncCallback(ReceiveCallback), state);
                }
                catch (Exception e)
                {
                    DataReceiveEvent("Error occurred when accept from client or begining receive ...");
                }
            }
        }

        private void ReceiveCallback(IAsyncResult ar)
        {
            StateObject so = (StateObject)ar.AsyncState;
            Socket handler = so.worker;
            string contents = string.Empty;
            int commCode = 0, offset = 0;

            try
            {
                int bytesRead = handler.EndReceive(ar);

                if (bytesRead > 0
                    && Encoding.Unicode.GetString(so.buffer, 0, 22) == SocketConstants.BufferStart)
                {
                    commCode = BitConverter.ToInt32(so.buffer, 22);
                    offset = (SocketConstants.BufferStart.Length) * 2 + 4;

                    so.sb.Append(Encoding.Unicode.GetString(so.buffer, offset, bytesRead - offset));
                    contents = so.sb.ToString();

                    DataReceiveEvent(string.Format("Message received: bytes[{0}], message: [{1}{2}{3}] from {4}"
                        , bytesRead, Encoding.Unicode.GetString(so.buffer, 0, 22)
                        , commCode, contents, handler.RemoteEndPoint.ToString()));

                    if (contents.IndexOf(SocketConstants.BufferEnd) > -1)
                    {
                        switch (commCode)
                        {
                            case SocketConstants.CheckStatus:
                                DataReceiveEvent(string.Format("Received the code {0} for status checking", commCode));
                                /*
                                  Do updating client status
                                   
                                  if error occur,
                                  SendMessage(handler, SocketConstants.CheckStatusFail);
                                */

                                SendMessage(handler, SocketConstants.CheckStatusSuccess);
                                break;

                            case SocketConstants.PolicyRequest:
                                DataReceiveEvent(string.Format("Received the code {0} for Policy request", commCode));
                                SendMessage(handler, SocketConstants.PolicySend,
                                    string.Format("logCreationPolicy:{0},backupFileStorage:{1}",
                                    Policy.logCreationPolicy, Policy.backupFileStorage));
                                break;

                            case SocketConstants.PolicySendSuccess:
                                DataReceiveEvent(string.Format("Received the code {0} for success of policy sending to {1}",
                                    commCode, handler.RemoteEndPoint.ToString()));
                                break;

                            case SocketConstants.PolicySendFail:
                                DataReceiveEvent(string.Format(
                                    "Received the code {0} for fail of policy sending to {1}, resending policy",
                                    commCode, handler.RemoteEndPoint.ToString()));
                                SendMessage(handler, SocketConstants.PolicySend,
                                    string.Format("logCreationPolicy:{0},backupFileStorage:{1}",
                                    Policy.logCreationPolicy, Policy.backupFileStorage));
                                break;

                            case SocketConstants.LogDataSend:
                                DataReceiveEvent(string.Format("Received the code {0} for sending log file", commCode));
                                string[] logData = contents.Split('\n');
                                string logFileName = logData[0].Split('[')[1].Replace("]", "");
                                /*
                                  Do checking validation
                                  
                                  if Something miss,
                                  SendMessage(handler, SocketConstants.LogDataSendFail, logFileName);
                                */
                                SendMessage(handler, SocketConstants.LogDataSendSuccess, logFileName);
                                break;

                            default:
                                break;
                        }

                        so.sb.Clear();
                    }
                }
                else
                {
                    DataReceiveEvent(string.Format("Wrong message received: bytes[{0}], message: [{1}] from {2}"
                        , so.buffer.Length, Encoding.Unicode.GetString(so.buffer, 0, bytesRead)
                        , handler.RemoteEndPoint.ToString()));
                }

                Array.Clear(so.buffer, 0, SocketConstants.BufferSize);

                handler.BeginReceive(so.buffer, 0, SocketConstants.BufferSize, 0,
                    new AsyncCallback(ReceiveCallback), so);
            }
            catch (Exception e)
            {
                DataReceiveEvent(string.Format("{0} is closed ...", handler.RemoteEndPoint.ToString()));
                clientSockets.Remove(handler);
            }
        }

        private void SendMessage(Socket handler, int netCode, string data = "")
        {
            IEnumerable<byte> byteData = Encoding.Unicode.GetBytes(SocketConstants.BufferStart)
                .Concat(BitConverter.GetBytes(netCode)).Concat(Encoding.Unicode.GetBytes(data))
                .Concat(Encoding.Unicode.GetBytes(SocketConstants.BufferEnd));
            byte[] buffer = byteData.ToArray();

            try
            {
                handler.BeginSend(buffer, 0, buffer.Length, 0,
                    new AsyncCallback(SendCallback), handler);

                DataReceiveEvent(string.Format("Try to send Message: bytes[{0}], message: [{1}{2}{3}] to {4}"
                    , buffer.Length, SocketConstants.BufferStart, netCode, SocketConstants.BufferEnd
                    , handler.RemoteEndPoint.ToString()));
            }
            catch (Exception e)
            {
                DataReceiveEvent(string.Format("Failed to begin sending data to {1}...", handler.RemoteEndPoint.ToString()));
                clientSockets.Remove(handler);
            }
        }

        public void SendPolicyAll()
        {
            foreach (Socket client in clientSockets)
            {
                SendMessage(client, SocketConstants.PolicySend,
                    string.Format("logCreationPolicy:{0},backupFileStorage:{1}",
                    Policy.logCreationPolicy, Policy.backupFileStorage));
            }
        }

        private void SendCallback(IAsyncResult ar)
        {
            Socket handler = (Socket)ar.AsyncState;

            try
            {
                int bytesSent = handler.EndSend(ar);
                DataReceiveEvent(string.Format("Sent message {0} bytes to {1}.", bytesSent, handler.RemoteEndPoint.ToString()));
            }
            catch (Exception e)
            {
                DataReceiveEvent(string.Format("{0} is closed ...", handler.RemoteEndPoint.ToString()));
                clientSockets.Remove(handler);
            }
        }

        public void Stop()
        {
            try
            {
                listener.Shutdown(SocketShutdown.Both);
                listener.Close();
            }
            catch (Exception e)
            {
            }
        }
    }
}
