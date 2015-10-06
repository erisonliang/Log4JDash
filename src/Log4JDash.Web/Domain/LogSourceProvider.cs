﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Log4JDash.Web.Domain
{
    internal sealed class LogSourceProvider
    {
        private readonly ILogSourceProviderConfig config_;

        public LogSourceProvider (ILogSourceProviderConfig config)
        {
            if (config == null)
            {
                throw new ArgumentNullException (nameof (config));
            }

            config_ = config;
        }

        private IReadOnlyDictionary<string, string> DoGetSources ()
        {
            var result = new Dictionary<string, string> ();

            foreach (var directory in config_.Directories)
            {
                var files = Directory.GetFiles (directory.DirectoryPath);
                foreach (var fullPath in files)
                {
                    if (directory.FilenamePattern.IsMatch (fullPath))
                    {
                        var fileId = GetFileId (directory, fullPath);
                        result.Add (fileId, fullPath);
                    }
                }
            }

            return result;
        }

        private static string GetFileId (ILogDirectoryConfig directory, string fullPath)
        {
            var fileName = Path.GetFileName (fullPath);
            return String.Format ("{0}-{1}", directory.Name, fileName).ToLower ();
        }

        public IEnumerable<string> GetSources ()
        {
            return DoGetSources ().Keys.OrderBy (k => k);
        }

        public string GetFile (string sourceId)
        {
            var sources = DoGetSources ();
            var key = String.IsNullOrWhiteSpace (sourceId)
                ? GetSources ().First ()
                : sourceId;

            string result;
            try
            {
                result = sources[key];
            }
            catch (KeyNotFoundException ex)
            {
                throw new ArgumentOutOfRangeException ("Invalid log source.", ex);
            }

            return result;
        }
    }
}