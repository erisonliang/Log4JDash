﻿using System;
using System.Collections.Generic;

namespace Log4JParserNet
{
    public sealed class FilterAllBuilder : FilterBuilder
    {
        private List<FilterBuilder> children_ = new List<FilterBuilder> ();

        public void Add (FilterBuilder child)
            => children_.Add (child);

        public void AddRange (IEnumerable<FilterBuilder> children)
            => children_.AddRange (children);

        public void Remove (FilterBuilder child)
            => children_.Remove (child);

        public void Clear ()
            => children_.Clear ();

        public override Filter Build ()
        {
            AssociatedFiltersCollection children = null;
            FilterHandle primaryFilter = null;

            // There can be a maximum of 3 exceptions if filter initialization fails:
            // * Initial exception that caused the initialization to fail.
            // * Exception raised when calling primaryFilter.Dispose ().
            // * Exception raised when calling children.Dispose ().
            var cleanupErrors = new List<Exception> (3);

            try
            {
                children = AssociatedFiltersCollection.Build (children_);

                Log4JParserC.Log4JFilterInitAll (out primaryFilter);
                foreach (var child in children)
                {
                    Log4JParserC.Log4JFilterAllAdd (primaryFilter, child.Handle);
                }

                return Filter.Composite (primaryFilter, children);
            }
            catch (Exception initEx)
            {
                cleanupErrors.Add (initEx);
                Disposable.TryDispose (primaryFilter, cleanupErrors);
                Disposable.TryDispose (children, cleanupErrors);

                if (cleanupErrors.Count > 1)
                {
                    throw new AggregateException (cleanupErrors);
                }

                throw;
            }
        }
    }
}
