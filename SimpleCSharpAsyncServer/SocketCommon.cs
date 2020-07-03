namespace SimpleCSharpAsyncServer
{
    class SocketConstants
    {
        public const int BackLog = 100;
        public const int BufferSize = 2048;
        public const string BufferStart = "|SLCPSTART|";
        public const string BufferEnd = "|SLCPEND|";

        public const int CheckStatus = 10;
        public const int CheckStatusSuccess = 11;
        public const int CheckStatusFail = 12;
        public const int PolicyRequest = 20;
        public const int PolicySend = 21;
        public const int PolicySendSuccess = 22;
        public const int PolicySendFail = 23;
        public const int LogDataSend = 50;
        public const int LogDataSendSuccess = 51;
        public const int LogDataSendFail = 52;
    }

    class Policy
    {
        public static string logCreationPolicy; // 0: malware only	1: everything
        public static string backupFileStorage; // 0: Delete		1: Storage
    }
}
