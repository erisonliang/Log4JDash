﻿using System;
using System.Collections.Generic;

namespace Log4JParserNet
{
    public abstract class FilterBuilder
    {
        internal FilterBuilder ()
        {

        }

        internal abstract HandleGraph<FilterHandle> Build ();

        public abstract void AcceptVisitor (IFilterBuilderVisitor visitor);

        public static FilterAllBuilder All (params FilterBuilder[] filters)
            => All ((IEnumerable<FilterBuilder>) filters);

        public static FilterAllBuilder All (IEnumerable<FilterBuilder> filters)
        {
            var result = new FilterAllBuilder ();
            result.AddRange (filters);

            return result;
        }

        public static FilterAnyBuilder Any (params FilterBuilder[] filters)
            => Any ((IEnumerable<FilterBuilder>) filters);

        public static FilterAnyBuilder Any (IEnumerable<FilterBuilder> filters)
        {
            var result = new FilterAnyBuilder ();
            result.AddRange (filters);

            return result;
        }

        public static FilterLevelBuilder Level (string min, string max)
            => new FilterLevelBuilder (min, max);

        public static FilterLoggerBuilder Logger (string logger)
            => new FilterLoggerBuilder (logger);

        public static FilterMessageBuilder Message (string message)
            => new FilterMessageBuilder (message);

        public static FilterNotBuilder Not (FilterBuilder child)
            => new FilterNotBuilder (child);

        public static FilterTimestampBuilder Timestamp (Int64 min, Int64 max)
            => new FilterTimestampBuilder (min, max);

        public static FilterTimestampBuilder Timestamp (DateTime min, DateTime max)
            => new FilterTimestampBuilder (min, max);

        internal static AssociatedHandlesCollection<HandleGraph<FilterHandle>> Build (IReadOnlyList<FilterBuilder> filterBuilders)
        {
            var result = new AssociatedHandlesCollection<HandleGraph<FilterHandle>> (filterBuilders.Count);

            try
            {
                foreach (var filterBuilder in filterBuilders)
                {
                    var filter = filterBuilder.Build ();
                    result.Add (filter);
                }

                return result;
            }
            catch (Exception initEx)
            {
                try
                {
                    result.Dispose ();
                }
                catch (Exception cleanupEx)
                {
                    throw new AggregateException (initEx, cleanupEx);
                }

                throw;
            }
        }
    }
}
