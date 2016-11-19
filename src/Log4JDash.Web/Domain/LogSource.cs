﻿using System.IO;
using System.Text;
using Log4JParserNet;

namespace Log4JDash.Web.Domain
{
    internal sealed class LogSource
    {
        public string Name { get; }

        private readonly string file_;

        private readonly Encoding encoding_;

        public LogSource (string name, string file, Encoding encoding)
        {
            Name = name;
            file_ = file;
            encoding_ = encoding;
        }

        public Log4JFile Open (long? maxSize = null)
        {
            using (var sourceStream = new FileStream (file_, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                Log4JFile logFile = null;
                try
                {
                    logFile = Log4JFile.Create (sourceStream, maxSize);
                    logFile.Encoding = encoding_;

                    return logFile;
                }
                catch
                {
                    logFile?.Dispose ();
                    throw;
                }
            }
        }
    }
}
